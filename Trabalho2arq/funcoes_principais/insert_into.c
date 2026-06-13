#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "delete.h"
#include "../structs/dados.h"
#include "../fornecidas/fornecidas.h"
#include "../aux/auxiliares.h"
#include "../aux/calcula.h"
#include "../structs/cabecalho.h"
#include "../structs/cabecalho_arvb.h"
#include "../structs/arvore_b.h"
#include "../funcoes_arvb/create_index.h"
#include "insert_into.h"

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

    FILE *fileIndex = NULL; // Cria um arquivo de indexação nulo
    cabecalho_arvb *cab_arvb = NULL;
    
    // Se for um insert com arquivo de indexação
    // para não quebrar o insert antigo
    if (nomeArqIndice != NULL) {    // Abre e verifica a integridade do arquivo de index 
        fileIndex = abre_verifica_rbplus(nomeArqIndice);
        if (fileIndex == NULL) {
            fclose(file);
            return;
        }
        cab_arvb = leCabecalhoArvb(fileIndex);  // Carrega o cabeçalho do arquivo de index
    }
    
    // Carrega informações do cabeçalho do arquivo de dados
    cabecalho *cab_dados = leCabecalhoDados(file);
    int topo = getTopoCabecalho(cab_dados);
    int proxRRN = getProxRRNCabecalho(cab_dados);

    // Atualiza o status para inconsistente do arquivo de dados
    setStatus(cab_dados, '0');  
    gravaCabecalho(file, cab_dados);
    finalizaCabecalho(cab_dados);
    
    // Se tiver arquivo de index, seta o sttus para inconsistente
    if (fileIndex != NULL && cab_arvb != NULL) {
        setStatusArvb(cab_arvb, '0');
        escreveCabecalhoArvb(fileIndex, cab_arvb);
    }

    for (int b = 0; b < n_insercoes; b++)
    {
        char bufferNulo[50]; 
        int codEstacao, codLinha, codProxEstacao, distProxEstacao, codLinhaIntegra, codEstIntegra;

        // Leitura dos campos
        if (scanf("%d", &codEstacao) != 1) break;

        char bufferNomeEst[200];
        ScanQuoteString(bufferNomeEst);

        scanf("%s", bufferNulo);
        codLinha = (strcmp(bufferNulo, "NULO") == 0) ? -1 : atoi(bufferNulo);

        char bufferNomeLinha[200];
        ScanQuoteString(bufferNomeLinha);

        scanf("%s", bufferNulo);
        codProxEstacao = (strcmp(bufferNulo, "NULO") == 0) ? -1 : atoi(bufferNulo);

        scanf("%s", bufferNulo);
        distProxEstacao = (strcmp(bufferNulo, "NULO") == 0) ? -1 : atoi(bufferNulo);

        scanf("%s", bufferNulo);
        codLinhaIntegra = (strcmp(bufferNulo, "NULO") == 0) ? -1 : atoi(bufferNulo);

        scanf("%s", bufferNulo);
        codEstIntegra = (strcmp(bufferNulo, "NULO") == 0) ? -1 : atoi(bufferNulo);

        // Checagem de duplicidade de regostro (codigo de estação repetido)
        int ja_existe_ativo = 0;
        if (fileIndex != NULL && cab_arvb != NULL) {    // Se tiver indexação
            int raiz_atual = getNoRaizArvb(cab_arvb);
            long offset_encontrado = busca_arvore_b(fileIndex, raiz_atual, codEstacao); // procura pela primary key
            
            if (offset_encontrado != -1) {
                // A chave existe na Árvore B. Verifica se ela está ativa
                fseek(file, offset_encontrado, SEEK_SET);
                dados *reg_verificacao = dados_le_binario(file);
                
                if (dados_get_removido(reg_verificacao) == '0') {   // Verifica se é ativo ou não
                    ja_existe_ativo = 1; // Registro duplicado e ativo
                }
                dados_apaga(reg_verificacao);
            }
        }

        // Se o registro já existir e for ativo, ignora o insert
        if (ja_existe_ativo) {
            continue;
        }

        // Cria registro para inserir
        dados *reg = dados_cria();
        dados_set_nomeEstacao(reg, bufferNomeEst);
        dados_set_nomeLinha(reg, (strcmp(bufferNomeLinha, "") == 0) ? "" : bufferNomeLinha);
        dados_set_campos_fixos(reg, '0', -1, codEstacao, codLinha, codProxEstacao, distProxEstacao, codLinhaIntegra, codEstIntegra);

        long offset_escrita = -1;

        //  Se tiver topo, aloca espaço pegando da pilha de removidos
        if (topo != -1) {
            offset_escrita = 17 + (long)(topo * 80);    
            
            fseek(file, offset_escrita, SEEK_SET);
            dados *reg_removido = dados_le_binario(file);
            topo = dados_get_proxRemovido(reg_removido);    // Atualiza topo
            dados_apaga(reg_removido); 
        } else {    // Se não, escreve no final do arquivo
            offset_escrita = 17 + (long)(proxRRN * 80); 
            proxRRN++;                                  
        }

        // Grava no arquivo de dados o registro
        fseek(file, offset_escrita, SEEK_SET);
        dados_grava_binario(reg, file);

        // Se tiver arquivo de indexação, insere nele o novo index
        if (fileIndex != NULL && cab_arvb != NULL) {
            int raiz_atual = getNoRaizArvb(cab_arvb);
            if (busca_arvore_b(fileIndex, raiz_atual, codEstacao) == -1) {  // Se retornar -1, insere, se não, não faz nada
                int offset_conversao_int = (int)offset_escrita;
                insere_arvore_b(fileIndex, cab_arvb, codEstacao, offset_conversao_int);
            }
        }
        
        dados_apaga(reg);   // Desaloca memória previamente alocada
    }

    // Grava o cabeçalho da Árvore como estável e fecha o arquivo de índice
    if (fileIndex != NULL && cab_arvb != NULL) {
        setStatusArvb(cab_arvb, '1');
        escreveCabecalhoArvb(fileIndex, cab_arvb);
        finalizaCabecalhoArvb(cab_arvb);
        fclose(fileIndex);
    }

    // Recalcula e grava o cabeçalho
    recalcula_e_grava_cabecalho(file, topo, proxRRN);   
    fclose(file);

    BinarioNaTela(nomeArqBin);
    if (nomeArqIndice != NULL) BinarioNaTela(nomeArqIndice);    // Se tiver index, mostra o binário
}