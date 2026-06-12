#include "delete.h"
#include "../structs/dados.h"
#include "../fornecidas/fornecidas.h"
#include "./aux/auxiliares.h"
#include "./aux/calcula.h"
#include "./structs/arvore_b.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/// @brief              Exclui registros logicamente de acordo com filtros de busca,
///                     adicionando-os na pilha de remoção.
/// @param nomeArqBin   Nome do arquivo binário a ser manipulado.
/// @param nomeArqArvoreB Nome do arquivo da árvore B a ser manipulado.
/// @param n_buscas     Número de buscas/iterações de remoção a serem efetuadas.
void DELETE_WHERE(char *nomeArqBin, char *nomeArqArvoreB, int n_buscas)
{
    FILE *binFile = abre_verifica_rbplus(nomeArqBin);
    if (binFile == NULL) return;

    int topo, proxRRN, nroEstacoes, nroParesEstacao;
    
    // Pula o byte do status para ler os metadados do cabeçalho
    fseek(binFile, 1, SEEK_SET);
    fread(&topo, sizeof(int), 1, binFile);
    fread(&proxRRN, sizeof(int), 1, binFile);
    fread(&nroEstacoes, sizeof(int), 1, binFile);
    fread(&nroParesEstacao, sizeof(int), 1, binFile);

    // Altera o status do cabeçalho para inconsistente no início da operação
    char status = '0';
    fseek(binFile, 0, SEEK_SET);
    fwrite(&status, sizeof(char), 1, binFile);

    for (int b = 0; b < n_buscas; b++)
    {
        int indice_codEstacao = -1;
        int m_campos;
        scanf("%d", &m_campos);

        char nomesCampos[m_campos][50];
        char valoresBusca[m_campos][100];

        leitura_campos(m_campos, nomesCampos, valoresBusca);

        for (int j = 0; j < m_campos; j++) {
            if (strcmp(nomesCampos[j], "codEstacao") == 0) {
                indice_codEstacao = j; 
            }
        }
        if(indice_codEstacao != -1 && nomeArqArvoreB != NULL){
            FILE *file_arvb = abre_verifica_rbplus(nomeArqArvoreB);
            if(file_arvb == NULL) continue;

            cabecalho_arvb *cab_arvb = leCabecalhoArvb(file_arvb);

            setStatusArvb(cab_arvb, '0');
            escreveCabecalhoArvb(file_arvb, cab_arvb);

            int chave_procurada = atoi(valoresBusca[indice_codEstacao]);
            
            long offset = busca_arvore_b(file_arvb, getNoRaizArvb(cab_arvb), chave_procurada);

            if (offset != -1) {
                fseek(binFile, offset, SEEK_SET);
                dados *trem = dados_le_binario(binFile);
                if(trem != NULL){
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
            
            setStatusArvb(cab_arvb, '1');
            escreveCabecalhoArvb(file_arvb, cab_arvb);
            
            fclose(file_arvb);
            
            // IMPORTANTE: Como leCabecalhoArvb alocou memória com malloc, precisamos liberar!
            finalizaCabecalhoArvb(cab_arvb);
        } else {
            // SEMPRE volta para o início dos registros e zera o RRN para cada nova busca
            fseek(binFile, 17, SEEK_SET); 
            dados *trem = NULL;
            int rrn = 0; 

            FILE *file_arvb = NULL;
            
            cabecalho_arvb *cab_arvb = NULL;

           if (nomeArqArvoreB != NULL) {
                file_arvb = abre_verifica_rbplus(nomeArqArvoreB);
                
                if (file_arvb != NULL) {
                    cab_arvb = leCabecalhoArvb(file_arvb);
                    setStatusArvb(cab_arvb, '0');
                    escreveCabecalhoArvb(file_arvb, cab_arvb);
                }
            }

            while ((trem = dados_le_binario(binFile)) != NULL)
            {
                // Calcula o byteoffset exato onde este registro começa no arquivo binário
                long offset_registro_atual = 17 + (long)(rrn * 80);

                // Só tenta deletar se o registro ainda estiver ativo
                if (dados_get_removido(trem) == '0') 
                {
                // Valida o match com os critérios da busca usando
                if (valida_registro(trem, m_campos, nomesCampos, valoresBusca))
                {
                    char flag_removido = '1';
                    int prox_na_lista = topo; // O registro atual passa a apontar para o antigo topo

                    // Volta o ponteiro do arquivo para o início deste registro específico
                    fseek(binFile, offset_registro_atual, SEEK_SET);
                    
                    // Escreve a flag de removido e o proxRRN do topo
                    fwrite(&flag_removido, sizeof(char), 1, binFile);
                    fwrite(&prox_na_lista, sizeof(int), 1, binFile);

                    topo = rrn; // O topo da pilha passa a ser o RRN deste registro recém excluído
                    
                    // Força o ponteiro do arquivo a voltar para o final do registro
                    fseek(binFile, offset_registro_atual + 80, SEEK_SET);
                }
            }
            // Libera a memória alocada para o registro atual
            dados_apaga(trem);
            rrn++;
        }
    }
    }
        
    recalcula_e_grava_cabecalho(binFile, topo, proxRRN);
    fclose(binFile);

    BinarioNaTela(nomeArqBin);
    if(nomeArqArvoreB != NULL){
        BinarioNaTela(nomeArqArvoreB);
    }
}