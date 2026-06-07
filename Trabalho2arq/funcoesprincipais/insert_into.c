#include "delete.h"
#include "dados.h"
#include "fornecidas.h"
#include "auxiliares.h"
#include "calcula.h"
#include "cabecalho.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/// @brief              Insere um novo registro, insere na posição do RRNtopo 
///                     caso topo != -1, se não insere no proxRRN, e atualiza o topo
/// @param nomeArqBin   Nome do arquivo a ser modificado
/// @param n_insercoes  Número de inserções 
void INSERT_INTO(char *nomeArqBin, int n_insercoes)
{
    FILE *file = abre_verifica_rbplus(nomeArqBin);  // Abre e verifica integridade do arquivo
 
    int topo, proxRRN, nroEstacoes, nroPares;
    
    fread(&topo, sizeof(int), 1, file);             // Leitura dos dados que serão inseridos
    fread(&proxRRN, sizeof(int), 1, file);
    fread(&nroEstacoes, sizeof(int), 1, file);
    fread(&nroPares, sizeof(int), 1, file);

    char status = '0';                              // Muda o cabeçalho para inconsistente
    fseek(file, 0, SEEK_SET);
    fwrite(&status, sizeof(char), 1, file);

    for (int b = 0; b < n_insercoes; b++)
    {
        dados trem;
        trem.removido = '0';
        trem.proxRemovido = -1; 
        
        char bufferNulo[50]; 

        scanf("%d", &trem.codEstacao);

        char bufferNomeEst[200];
        ScanQuoteString(bufferNomeEst);

        scanf("%s", bufferNulo);
        trem.codLinha = (strcmp(bufferNulo, "NULO") == 0) ? -1 : atoi(bufferNulo);

        char bufferNomeLinha[200];
        ScanQuoteString(bufferNomeLinha);

        scanf("%s", bufferNulo);
        trem.codProxEstacao = (strcmp(bufferNulo, "NULO") == 0) ? -1 : atoi(bufferNulo);

        scanf("%s", bufferNulo);
        trem.distProxEstacao = (strcmp(bufferNulo, "NULO") == 0) ? -1 : atoi(bufferNulo);

        scanf("%s", bufferNulo);
        trem.codLinhaIntegra = (strcmp(bufferNulo, "NULO") == 0) ? -1 : atoi(bufferNulo);

        scanf("%s", bufferNulo);
        trem.codEstIntegra = (strcmp(bufferNulo, "NULO") == 0) ? -1 : atoi(bufferNulo);

        if (strcmp(bufferNomeEst, "") == 0) {
            trem.tamNomeEstacao = 0;
            trem.nomeEstacao = NULL;
        } else {
            trem.tamNomeEstacao = strlen(bufferNomeEst);
            trem.nomeEstacao = malloc(trem.tamNomeEstacao + 1);
            strcpy(trem.nomeEstacao, bufferNomeEst);
        }

        if (strcmp(bufferNomeLinha, "") == 0) {
            trem.tamNomeLinha = 0;
            trem.nomeLinha = NULL;
        } else {
            trem.tamNomeLinha = strlen(bufferNomeLinha);
            trem.nomeLinha = malloc(trem.tamNomeLinha + 1);
            strcpy(trem.nomeLinha, bufferNomeLinha);
        }

        long offset_escrita;

        if (topo != -1) {
            offset_escrita = 17 + (long)(topo * 80);    // Calculando o Byteoffset da escrita,
                                                        // caso haja topo
            fseek(file, offset_escrita + 1, SEEK_SET); 
            int rrn_proximo_removido;
            fread(&rrn_proximo_removido, sizeof(int), 1, file);
            
            topo = rrn_proximo_removido;
        } else {
            offset_escrita = 17 + (long)(proxRRN * 80); // Calculando o Byteoffset da escrita,
            proxRRN++;                                  // no final do arquivo
        }
        fseek(file, offset_escrita, SEEK_SET);

        fwrite(&trem.removido, sizeof(char), 1, file);  // Escrevendo o novo registro
        fwrite(&trem.proxRemovido, sizeof(int), 1, file);
        fwrite(&trem.codEstacao, sizeof(int), 1, file);
        fwrite(&trem.codLinha, sizeof(int), 1, file);
        fwrite(&trem.codProxEstacao, sizeof(int), 1, file);
        fwrite(&trem.distProxEstacao, sizeof(int), 1, file);
        fwrite(&trem.codLinhaIntegra, sizeof(int), 1, file);
        fwrite(&trem.codEstIntegra, sizeof(int), 1, file);
        
        fwrite(&trem.tamNomeEstacao, sizeof(int), 1, file);
        if (trem.tamNomeEstacao > 0) fwrite(trem.nomeEstacao, sizeof(char), trem.tamNomeEstacao, file);
        
        fwrite(&trem.tamNomeLinha, sizeof(int), 1, file);
        if (trem.tamNomeLinha > 0) fwrite(trem.nomeLinha, sizeof(char), trem.tamNomeLinha, file);

        // Inserindo o lixo $$$
        int bytesEscritos = 37 + trem.tamNomeEstacao + trem.tamNomeLinha;
        char lixo = '$';
        for (int k = 0; k < (80 - bytesEscritos); k++) {
            fwrite(&lixo, sizeof(char), 1, file);
        }

        if (trem.nomeEstacao) free(trem.nomeEstacao);   // Desalocando memória préviamente alocada
        if (trem.nomeLinha) free(trem.nomeLinha);
    }

    recalcula_e_grava_cabecalho(file, topo, proxRRN);   // Atualizando cabeçalho

    fclose(file);
    BinarioNaTela(nomeArqBin);
}