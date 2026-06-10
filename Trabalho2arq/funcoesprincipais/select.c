#include "../aux/auxiliares.h"
#include "../fornecidas/fornecidas.h"
#include "../structs/dados.h"
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

    fseek(file, 17, SEEK_SET);  // Pula o cabeçalho (17 bytes)

    dados *trem = NULL;
    int encontrou = 0;

    // O TAD gerencia toda a leitura do registro físico e o avanço dos bytes do lixo
    while ((trem = dados_le_binario(file)) != NULL)
    {
        // Verifica se está marcado como removido usando a interface do TAD
        if (dados_get_removido(trem) == '1') {
            dados_apaga(trem); // Desaloca e continua para o próximo
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
void SELECT_WHERE(char *nomeArqBin, int numero_buscas)
{
    FILE *file = abre_verifica_rb(nomeArqBin);
    if (file == NULL) return;

    for (int b = 0; b < numero_buscas; b++)
    {
        int m_campos;
        scanf("%d", &m_campos);

        char nomesCampos[m_campos][50];
        char valoresBusca[m_campos][100];

        // Coleta os filtros da busca
        for (int j = 0; j < m_campos; j++)
        {
            scanf("%s", nomesCampos[j]);
            
            if (strcmp(nomesCampos[j], "nomeEstacao") == 0 || strcmp(nomesCampos[j], "nomeLinha") == 0)
                ScanQuoteString(valoresBusca[j]);
            else 
                scanf("%s", valoresBusca[j]); 
        }

        fseek(file, 17, SEEK_SET); // Reposiciona no início dos registros de dados para cada busca
        
        dados *trem = NULL;
        int encontrouAoMenosUm = 0;

        while ((trem = dados_le_binario(file)) != NULL)
        {
            // Pula registros logicamente removidos
            if (dados_get_removido(trem) == '1') {
                dados_apaga(trem);
                continue;
            }

            // A função valida_registro agora opera sobre a interface do TAD de forma segura
            int match = valida_registro(trem, m_campos, nomesCampos, valoresBusca);
            if (match)
            {
                encontrouAoMenosUm = 1;
                printa_estacao(trem);
            }
            
            dados_apaga(trem); // Libera a memória alocada por dados_le_binario
        }        
        
        if (!encontrouAoMenosUm)
            printf("Registro inexistente.\n");

        printf("\n");
    }
    fclose(file);
}