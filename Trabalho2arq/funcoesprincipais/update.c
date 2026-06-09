#include "update.h"
#include "dados.h"
#include "auxiliares.h"
#include "fornecidas.h"
#include "busca.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/// @brief              Realiza uma busca no arquivo, em seguida, atualiza
///                     o campo desejado com o conteúdo desejado.
/// @param nomeArqBin   Nome do arquivo a ser manipulado.
/// @param n_buscas     Número de buscas no arquivo.
void UPDATE(char *nomeArqBin, int n_buscas)
{
    FILE *file = abre_verifica_rbplus(nomeArqBin);

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

        // Valores para atualização
        int p_campos;
        scanf("%d", &p_campos);
        char **nomesCamposAtualiza = malloc(p_campos * sizeof(char *));
        char **valoresAtualiza = malloc(p_campos * sizeof(char *));
        for (int j = 0; j < p_campos; j++) {
            nomesCamposAtualiza[j] = malloc(50 * sizeof(char));
            valoresAtualiza[j] = malloc(100 * sizeof(char));
        }
        leitura_campos(p_campos, nomesCamposAtualiza, valoresAtualiza); // Leitura dos campos atualizados

        dados trem;
        busca(trem, file, 2, valoresBusca, nomesCampos, m_campos, nomesCamposAtualiza, valoresAtualiza, p_campos); // funcao: 2 para update

        // Libera a memória de cada string
        for (int j = 0; j < m_campos; j++) {
            free(nomesCampos[j]);
            free(valoresBusca[j]);
        }
        free(nomesCampos);
        free(valoresBusca);

        for (int j = 0; j < p_campos; j++) {
            free(nomesCamposAtualiza[j]);
            free(valoresAtualiza[j]);
        }
        free(nomesCamposAtualiza);
        free(valoresAtualiza);
    }
    fclose(file);
    BinarioNaTela(nomeArqBin);
}
