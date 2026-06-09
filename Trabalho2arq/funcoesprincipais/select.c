#include "auxiliares.h"
#include "fornecidas.h"
#include "dados.h"
#include "select.h"
#include "busca.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/// @brief              Abre o arquivo, percorre, se achou os registros correspondentes, printa os registros,
///                     se não, printa que não foi encontrado.
/// @param nomeArqBin   Nome do arquivo passado para manipulação.
void SELECT_FROM(char *nomeArqBin)
{
    FILE *file = abre_verifica_rb(nomeArqBin);
    dados trem;

    busca(trem, file, 0, NULL, NULL, 0, NULL, NULL, 0); // funcao: 0 para selectFrom 
    fclose(file);
}

/// @brief                  Abre o arquivo, percorre, lista todas as N buscas com todos os resultados
///                         correspondentes, se não encontrar, printa que não existe. 
/// @param nomeArqBin       Nome do arquivo passado para manipulação.
/// @param numero_buscas    Número de buscas, onde poderão ser filtrados por campos diferentes.
void SELECT_WHERE(char *nomeArqBin, int numero_buscas)
{
    FILE *file = abre_verifica_rb(nomeArqBin);

    for (int b = 0; b < numero_buscas; b++)
    {
        int m_campos;
        scanf("%d", &m_campos); // Lê quantos campos essa busca vai ter

        char **nomesCampos = malloc(m_campos * sizeof(char *));
        char **valoresBusca = malloc(m_campos * sizeof(char *));
        for (int j = 0; j < m_campos; j++) {
            nomesCampos[j] = malloc(50 * sizeof(char));
            valoresBusca[j] = malloc(100 * sizeof(char));
        }

        leitura_campos(m_campos, nomesCampos, valoresBusca);

        dados trem;
        busca(trem, file, 1, valoresBusca, nomesCampos, m_campos, NULL, NULL, 0); // funcao: 1 para selectWhere 

        printf("\n");
        // Libera a memória de cada string
        for (int j = 0; j < m_campos; j++) {
            free(nomesCampos[j]);
            free(valoresBusca[j]);
        }
        // Libera os vetores de ponteiros
        free(nomesCampos);
        free(valoresBusca);
    }
    fclose(file);
}