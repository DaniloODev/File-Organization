#include "delete.h"
#include "../structs/dados.h"
#include "../fornecidas/fornecidas.h"
#include "./aux/auxiliares.h"
#include "./aux/calcula.h"
#include "../structs/cabecalho.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/// @brief              Insere um novo registro, reaproveitando espaços logicamente
///                     removidos (pilha) ou inserindo no fim do arquivo (proxRRN).
/// @param nomeArqBin   Nome do arquivo binário a ser modificado
/// @param n_insercoes  Número de inserções a serem feitas
void INSERT_INTO(char *nomeArqBin, int n_insercoes)
{
    FILE *file = abre_verifica_rbplus(nomeArqBin);
    if (file == NULL) return;
 
    int topo, proxRRN, nroEstacoes, nroPares;
    
    fseek(file, 1, SEEK_SET);
    fread(&topo, sizeof(int), 1, file);             
    fread(&proxRRN, sizeof(int), 1, file);
    fread(&nroEstacoes, sizeof(int), 1, file);
    fread(&nroPares, sizeof(int), 1, file);

    // Muda o status do cabeçalho para inconsistente no início da operação
    char status = '0';
    fseek(file, 0, SEEK_SET);
    fwrite(&status, sizeof(char), 1, file);

    for (int b = 0; b < n_insercoes; b++)
    {
        dados *reg = dados_cria(); // Aloca um novo registro
        
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

        // Define os campos de tamanho fixo obtidos
        dados_set_campos_fixos(reg, '0', -1, codEstacao, codLinha, codProxEstacao, distProxEstacao, codLinhaIntegra, codEstIntegra);

        long offset_escrita;

        // Verifica se há espaço na pilha de removidos para reaproveitar
        if (topo != -1) {
            // Reaproveita o espaço do topo da pilha
            offset_escrita = 17 + (long)(topo * 80);    
            
            // Vai até a posição para ler qual é o PRÓXIMO RRN da pilha antes de sobrescrever
            fseek(file, offset_escrita, SEEK_SET);
            dados *reg_removido = dados_le_binario(file);
            
            // Atualiza o topo da lista encadeada com o valor que estava guardado lá
            topo = dados_get_proxRemovido(reg_removido);
            
            dados_apaga(reg_removido); // Libera o registro temporário lido
        } else {
            // Não há removidos, insere no fim do arquivo (proxRRN)
            offset_escrita = 17 + (long)(proxRRN * 80); 
            proxRRN++;                                  
        }

        // Move o ponteiro do arquivo para o local correto determinado e grava o registro
        fseek(file, offset_escrita, SEEK_SET);
        dados_grava_binario(reg, file);

        dados_apaga(reg); // Libera a memória do registro inserido
    }

    // Recalcula e grava o cabeçalho
    recalcula_e_grava_cabecalho(file, topo, proxRRN);   

    fclose(file);
    BinarioNaTela(nomeArqBin);
}