#include "delete.h"
#include "dados.h"
#include "fornecidas.h"
#include "auxiliares.h"
#include "calcula.h"
#include "busca.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/// @brief              Exclui de acordo com o conteúdo desejado, do campo selecionado.
/// @param nomeArqBin   Nome do arquivo a ser manipulado.
/// @param n_buscas     Número de buscas a serem efetuadas.
void DELETE_WHERE(char *nomeArqBin, int n_buscas)
{
    FILE *file = abre_verifica_rbplus(nomeArqBin);   // Verifica integridade do arquivo

    for (int b = 0; b < n_buscas; b++)
    {
        int m_campos;
        scanf("%d", &m_campos);

        char **nomesCampos = malloc(m_campos * sizeof(char *));
        char **valoresBusca = malloc(m_campos * sizeof(char *));
        for (int j = 0; j < m_campos; j++) {
            nomesCampos[j] = malloc(50 * sizeof(char));
            valoresBusca[j] = malloc(100 * sizeof(char));
        }

        leitura_campos(m_campos, nomesCampos, valoresBusca);   // Leitura dos campos

        dados trem;
        // funcao: 3 para delete
        busca(trem, file, 3, valoresBusca, nomesCampos, m_campos, NULL, NULL, 0);

        for (int j = 0; j < m_campos; j++) {
            free(nomesCampos[j]);
            free(valoresBusca[j]);
        }
        free(nomesCampos);
        free(valoresBusca);
    }
    fclose(file);
    BinarioNaTela(nomeArqBin);
}