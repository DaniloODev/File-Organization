#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "delete.h"
#include "../structs/dados.h"
#include "../structs/cabecalho.h"
#include "../fornecidas/fornecidas.h"
#include "./aux/auxiliares.h"
#include "./aux/calcula.h"
#include "./structs/arvore_b.h"

/// @brief              Exclui registros logicamente de acordo com filtros de busca,
///                     adicionando-os na pilha de remoção.
/// @param nomeArqBin   Nome do arquivo binário a ser manipulado.
/// @param nomeArqArvoreB Nome do arquivo da árvore B a ser manipulado.
/// @param n_buscas     Número de buscas/iterações de remoção a serem efetuadas.
void DELETE_WHERE(char *nomeArqBin, char *nomeArqArvoreB, int n_buscas)
{
    // Abre e verifica a integridade do arquivo de dados
    FILE *binFile = abre_verifica_rbplus(nomeArqBin);
    if (binFile == NULL) return;
    
    // Carrega informações do cabeçalho do arquivo de dados
    cabecalho *cab_dados = leCabecalhoDados(binFile);
    int topo = getTopoCabecalho(cab_dados);
    int proxRRN = getProxRRNCabecalho(cab_dados);

    // Atualiza o status para inconsistente do arquivo de dados
    setStatus(cab_dados, '0');  
    gravaCabecalho(binFile, cab_dados);
    finalizaCabecalho(cab_dados);

    // Inicializa um arquivo de index como nulo
    FILE *file_arvb = NULL;
    cabecalho_arvb *cab_arvb = NULL;

    // Para não quebrar o delete antigo, fazemos a verificação
    // para ver se foi passado um arquivo de index ou não
    if (nomeArqArvoreB != NULL) {
        file_arvb = abre_verifica_rbplus(nomeArqArvoreB);
        if (file_arvb != NULL) {
            cab_arvb = leCabecalhoArvb(file_arvb);
            setStatusArvb(cab_arvb, '0');
            escreveCabecalhoArvb(file_arvb, cab_arvb);
        }
    }

    for (int b = 0; b < n_buscas; b++)
    {   
        // Leitura dos campos
        int indice_codEstacao = -1;
        int m_campos;
        if (scanf("%d", &m_campos) != 1) break;

        char nomesCampos[m_campos][50];
        char valoresBusca[m_campos][100];

        leitura_campos(m_campos, nomesCampos, valoresBusca);

        for (int j = 0; j < m_campos; j++) {
            if (strcmp(nomesCampos[j], "codEstacao") == 0) {
                indice_codEstacao = j; 
            }
        }
        
        // Busca e remoção pelo codigo da estação (Primary key) utilizando a indexação
        if(indice_codEstacao != -1 && file_arvb != NULL && cab_arvb != NULL){
            int chave_procurada = atoi(valoresBusca[indice_codEstacao]);
            
            // Busca o código da estação no arquivo da árvore
            long offset = busca_arvore_b(file_arvb, getNoRaizArvb(cab_arvb), chave_procurada);

            if (offset != -1) { // Se retornou um ofsset é porque achou
                fseek(binFile, offset, SEEK_SET);
                dados *trem = dados_le_binario(binFile);
                if(trem != NULL){
                    // Se o registro ainda for ativo e der match, remove
                    if(dados_get_removido(trem) == '0' && valida_registro(trem, m_campos, nomesCampos, valoresBusca)){
                        char flag_removido = '1';
                        int prox_na_lista = topo;
                        int rrn_excluido = (int)((offset - 17) / 80);
                        
                        fseek(binFile, offset, SEEK_SET);
                        fwrite(&flag_removido, sizeof(char), 1, binFile);
                        fwrite(&prox_na_lista, sizeof(int), 1, binFile);
                        
                        topo = rrn_excluido;
                        remover_arvore_b(file_arvb, cab_arvb, chave_procurada);
                    }
                    dados_apaga(trem);
                }
            }
        } 
        // Busca e remoção Sequencial, a remoção antiga
        else {
            fseek(binFile, 17, SEEK_SET); 
            dados *trem = NULL;
            int rrn = 0; 

            // enquanto tem arquivo para ler
            while ((trem = dados_le_binario(binFile)) != NULL)
            {
                long offset_registro_atual = 17 + (long)(rrn * 80);

                if (dados_get_removido(trem) == '0')  // Se estiver ativo
                {
                    if (valida_registro(trem, m_campos, nomesCampos, valoresBusca)) // É quem procuramos
                    {
                        char flag_removido = '1';
                        int prox_na_lista = topo; 

                        fseek(binFile, offset_registro_atual, SEEK_SET);
                        fwrite(&flag_removido, sizeof(char), 1, binFile);
                        fwrite(&prox_na_lista, sizeof(int), 1, binFile);

                        topo = rrn; 
                        
                        // Caso em que escolheu a remoção pela arvore mas não
                        // passou um codigo unico comom parametro
                        if (file_arvb != NULL && cab_arvb != NULL) {
                            int chave_a_remover = dados_get_codEstacao(trem);
                            remover_arvore_b(file_arvb, cab_arvb, chave_a_remover);
                        }
                    }
                }
                dados_apaga(trem);
                rrn++;
                
                fseek(binFile, 17 + (long)(rrn * 80), SEEK_SET);    // Atualiza cursor no arquivo
            }
        }
    }

    // Se foi passado um arquivo de indexação, atualiza o status e cabeçalho
    if (file_arvb != NULL && cab_arvb != NULL) {
        setStatusArvb(cab_arvb, '1');
        escreveCabecalhoArvb(file_arvb, cab_arvb);
        fclose(file_arvb);
        finalizaCabecalhoArvb(cab_arvb);
    }
    recalcula_e_grava_cabecalho(binFile, topo, proxRRN);    // Recalcula e grava o cabecalho de dados

    char status_consistente = '1';
    fseek(binFile, 0, SEEK_SET);
    fwrite(&status_consistente, sizeof(char), 1, binFile);  // Atualiza o cabecalho para consistente
    
    fclose(binFile);

    BinarioNaTela(nomeArqBin);
    if(nomeArqArvoreB != NULL){ // Caso for com indexação
        BinarioNaTela(nomeArqArvoreB);
    }
}