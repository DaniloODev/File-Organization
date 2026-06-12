#include "../aux/auxiliares.h"
#include "../fornecidas/fornecidas.h"
#include "../structs/dados.h"
#include "../structs/arvore_b.h"
#include "select.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/// @brief              Abre o arquivo, percorre e printa os registros ativos.
/// @param nomeArqBin   Nome do arquivo binário.
void SELECT_FROM(char *nomeArqBin)
{
    FILE *file = abre_verifica_rb(nomeArqBin);
    if (file == NULL) return;

    fseek(file, 17, SEEK_SET);  // Pula o cabeçalh

    dados *trem = NULL;
    int encontrou = 0;

    // leitura sequencial.
    while ((trem = dados_le_binario(file)) != NULL)
    {
        // Verifica se está removido logicalmente
        if (dados_get_removido(trem) == '1') {
            dados_apaga(trem); // Desaloca memória previamente alocada
            continue;
        }

        encontrou = 1;
        printa_estacao(trem);

        dados_apaga(trem); // Libera o registro após o uso
    }

    if (!encontrou)
        printf("Registro inexistente.\n");

    fclose(file);
}

/// @brief                  Abre o arquivo, percorre e lista os resultados filtrados pelas buscas.
/// @param nomeArqBin       Nome do arquivo binário.
/// @param numero_buscas    Número de buscas a serem realizadas.
void SELECT_WHERE(char *nomeArqBin, char *nomeArqArvoreB, int numero_buscas)
{
    FILE *file = abre_verifica_rb(nomeArqBin);
    if (file == NULL) return;

    for (int b = 0; b < numero_buscas; b++)
    {
        // reseta variáveis a cada nova busca
        int indice_codEstacao = -1;
        int encontrouAoMenosUm = 0;
        int m_campos;
        
        scanf("%d", &m_campos);

        char nomesCampos[m_campos][50];
        char valoresBusca[m_campos][100];

        // Coleta os filtros da busca
        for (int j = 0; j < m_campos; j++)
        {
            scanf("%s", nomesCampos[j]);
            
            if (strcmp(nomesCampos[j], "nomeEstacao") == 0 || strcmp(nomesCampos[j], "nomeLinha") == 0) {
                ScanQuoteString(valoresBusca[j]);
            } else {
                scanf("%s", valoresBusca[j]); 
            }

            if(strcmp(nomesCampos[j], "codEstacao") == 0){
                indice_codEstacao = j;
            }
        }

        if (indice_codEstacao != -1 && nomeArqArvoreB != NULL) {
            FILE *file_arvb = abre_verifica_rb(nomeArqArvoreB);
            if (file_arvb == NULL) continue; 

            int chave_procurada = atoi(valoresBusca[indice_codEstacao]);
            char status_arvb;
            int noRaiz, topo_arvb, proxRRN_arvb, nroNos_arvb;

            fseek(file_arvb, 0, SEEK_SET);
            fread(&status_arvb, sizeof(char), 1, file_arvb);
            fread(&noRaiz, sizeof(int), 1, file_arvb);
            fread(&topo_arvb, sizeof(int), 1, file_arvb);
            fread(&proxRRN_arvb, sizeof(int), 1, file_arvb);
            fread(&nroNos_arvb, sizeof(int), 1, file_arvb);

            long offset = busca_arvore_b(file_arvb, noRaiz, chave_procurada);
            if(offset != -1){
                fseek(file, offset, SEEK_SET);
                
                dados *trem = dados_le_binario(file);
                
                if (trem != NULL) {
                    // Ve se deu match
                    if (dados_get_removido(trem) == '0' && valida_registro(trem, m_campos, nomesCampos, valoresBusca)) {
                        encontrouAoMenosUm = 1;
                        printa_estacao(trem);
                    }
                    dados_apaga(trem); // Desalocca memória previamente alocada
                }
            }
        
            if (!encontrouAoMenosUm) {
                printf("Registro inexistente.\n");
            }
            
            fclose(file_arvb);
        } else {
            // Busca sequencial
            fseek(file, 17, SEEK_SET); // Reposiciona no início dos registros de dados para cada busca
            
            dados *trem = NULL;

            while ((trem = dados_le_binario(file)) != NULL)
            {
                // Pula registros logicamente removidos
                if (dados_get_removido(trem) == '1') {
                    dados_apaga(trem);
                    continue;
                }

                // Vê se deu match
                int match = valida_registro(trem, m_campos, nomesCampos, valoresBusca);
                if (match)
                {
                    encontrouAoMenosUm = 1;
                    printa_estacao(trem);
                }
                
                dados_apaga(trem); // Libera a memória previamente alocada
            }        
            
            if (!encontrouAoMenosUm) {
                printf("Registro inexistente.\n");
            }
        }
        printf("\n");
    }
    fclose(file);
}