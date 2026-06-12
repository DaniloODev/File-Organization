#include "delete.h"
#include "../structs/dados.h"
#include "../fornecidas/fornecidas.h"
#include "./aux/auxiliares.h"
#include "./aux/calcula.h"
#include "../structs/cabecalho.h"
#include "../structs/cabecalho_arvb.h"
#include "../structs/arvore_b.h"
#include "../funcoes_arvb/create_index.h"
#include "insert_into.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/// @brief              Insere um novo registro, reaproveitando espaços logicamente
///                     removidos (pilha) ou inserindo no fim do arquivo (proxRRN).
///                     Agora também insere a chave na Árvore B.
/// @param nomeArqBin   Nome do arquivo binário de dados
/// @param nomeArqIndice Nome do arquivo binário de índice da Árvore B
/// @param n_insercoes  Número de inserções a serem feitas
void INSERT_INTO(char *nomeArqBin, char *nomeArqIndice, int n_insercoes)
{
    // Abre e verifica a integridade do arquivo de dados
    FILE *file = abre_verifica_rbplus(nomeArqBin);
    if (file == NULL) return;

    FILE *fileIndex = NULL;
    cabecalho_arvb *cab_arvb = NULL;
    
    // Se não houver arquivo de index passado ele fará a inserção sem ele,
    // Para não quebrar o INSERT_INTO do primeiro trabalho 
    if (nomeArqIndice != NULL) {
        fileIndex = abre_verifica_rbplus(nomeArqIndice);
        if (fileIndex == NULL) {
            fclose(file);
            return;
        }
        cab_arvb = leCabecalhoArvb(fileIndex);
    }
    
    // Carrega informações do cabecalho do arquivo de dados
    cabecalho *cab_dados = leCabecalhoDados(file);
    int topo = getTopoCabecalho(cab_dados);
    int proxRRN = getProxRRNCabecalho(cab_dados);

    setStatus(cab_dados, '1');  // Muda o status para inconsistente
    gravaCabecalho(file, cab_dados);
    
    if (fileIndex != NULL && cab_arvb != NULL) {
        setStatusArvb(cab_arvb, '0');
        escreveCabecalhoArvb(fileIndex, cab_arvb);
    }

    for (int b = 0; b < n_insercoes; b++)
    {
        dados *reg = dados_cria();
        
        char bufferNulo[50]; 
        int codEstacao, codLinha, codProxEstacao, distProxEstacao, codLinhaIntegra, codEstIntegra;

        scanf("%d", &codEstacao);

        char bufferNomeEst[200];
        ScanQuoteString(bufferNomeEst);
        dados_set_nomeEstacao(reg, bufferNomeEst);

        scanf("%s", bufferNulo);
        codLinha = (strcmp(bufferNulo, "NULO") == 0) ? -1 : atoi(bufferNulo);

        char bufferNomeLinha[200];
        ScanQuoteString(bufferNomeLinha);
        dados_set_nomeLinha(reg, (strcmp(bufferNomeLinha, "") == 0) ? "" : bufferNomeLinha);

        scanf("%s", bufferNulo);
        codProxEstacao = (strcmp(bufferNulo, "NULO") == 0) ? -1 : atoi(bufferNulo);

        scanf("%s", bufferNulo);
        distProxEstacao = (strcmp(bufferNulo, "NULO") == 0) ? -1 : atoi(bufferNulo);

        scanf("%s", bufferNulo);
        codLinhaIntegra = (strcmp(bufferNulo, "NULO") == 0) ? -1 : atoi(bufferNulo);

        scanf("%s", bufferNulo);
        codEstIntegra = (strcmp(bufferNulo, "NULO") == 0) ? -1 : atoi(bufferNulo);

        dados_set_campos_fixos(reg, '0', -1, codEstacao, codLinha, codProxEstacao, distProxEstacao, codLinhaIntegra, codEstIntegra);

        long offset_escrita = -1;
        int ja_existe = 0;

        // Verifica se a estação ja existe
        if (fileIndex != NULL && cab_arvb != NULL) {
            int raiz_atual = getNoRaizArvb(cab_arvb);
            
            // Passa o arquivo, o RRN da raiz e a chave buscada
            long offset_encontrado = busca_arvore_b(fileIndex, raiz_atual, codEstacao);
            
            if (offset_encontrado != -1) {
                offset_escrita = offset_encontrado;
                ja_existe = 1; // Registro já existe, ativa o modo substituição
            }
        }

        // Se não existe, aloca um espaço novo (seja na pilha ou no final do arquivo)
        if (!ja_existe) {
            if (topo != -1) {
                offset_escrita = 17 + (long)(topo * 80);    
                
                fseek(file, offset_escrita, SEEK_SET);
                dados *reg_removido = dados_le_binario(file);
                topo = dados_get_proxRemovido(reg_removido);
                dados_apaga(reg_removido); 
            } else {
                offset_escrita = 17 + (long)(proxRRN * 80); 
                proxRRN++;                                  
            }
        }

        // Move o ponteiro para a posição correta e grava
        fseek(file, offset_escrita, SEEK_SET);
        dados_grava_binario(reg, file);

        // Só insere na Árvore se o registro for novo, já que ela é indexada pelo código unico
        if (!ja_existe && fileIndex != NULL && cab_arvb != NULL) {
            int offset_conversao_int = (int)offset_escrita;
            insere_arvore_b(fileIndex, cab_arvb, codEstacao, offset_conversao_int);
        }
        
        dados_apaga(reg); 
    }

    // Grava o cabeçalho e fecha o arquivo
    recalcula_e_grava_cabecalho(file, topo, proxRRN);   
    fclose(file);

    // Grava o cabeçalho da Árvore e fecha o raquivo
    if (fileIndex != NULL && cab_arvb != NULL) {
        setStatusArvb(cab_arvb, '1');
        escreveCabecalhoArvb(fileIndex, cab_arvb);
        finalizaCabecalhoArvb(cab_arvb);
        fclose(fileIndex);
    }

    BinarioNaTela(nomeArqBin);
    if (nomeArqIndice != NULL)  BinarioNaTela(nomeArqIndice);
}