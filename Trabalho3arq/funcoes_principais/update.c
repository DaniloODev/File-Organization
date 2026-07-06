#include "update.h"
#include "../structs/dados.h"
#include "../aux/auxiliares.h"
#include "../fornecidas/fornecidas.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/// @brief              Realiza uma busca no arquivo e atualiza os campos 
///                     dos registros correspondentes diretamente no binário.
/// @param nomeArqBin   Nome do arquivo a ser manipulado.
/// @param n_buscas     Número de iterações de update.
void UPDATE(char *nomeArqBin, int n_buscas)
{
    FILE *file = abre_verifica_rbplus(nomeArqBin);  // Abre e verifica o arquivo de dados
    if (file == NULL) return;

    // Altera o status para inconsistente no início das operações
    char status = '0';  
    fseek(file, 0, SEEK_SET);
    fwrite(&status, sizeof(char), 1, file);

    for (int b = 0; b < n_buscas; b++)  
    { 
        // critérios de Busca
        int m_campos;
        scanf("%d", &m_campos);
        char nomesCamposBusca[m_campos][50];
        char valoresBusca[m_campos][100];
        leitura_campos(m_campos, nomesCamposBusca, valoresBusca);   

        // valores a serem atualizados
        int p_campos;
        scanf("%d", &p_campos);
        char nomesCamposAtualiza[p_campos][50];
        char valoresAtualiza[p_campos][100];
        leitura_campos(p_campos, nomesCamposAtualiza, valoresAtualiza); 

        // Reposiciona no início dos dados antes de varrer o arquivo
        fseek(file, 17, SEEK_SET);  
        dados *trem = NULL;
        int rrn = 0;

        while ((trem = dados_le_binario(file)) != NULL) 
        {
            long offset_atual = 17 + (long)(rrn * 80);    

            // Só atualiza se o registro não estiver deletado
            if (dados_get_removido(trem) == '0')   
            {
                int match = valida_registro(trem, m_campos, nomesCamposBusca, valoresBusca);

                if (match)
                {
                    // Modifica o dado
                    dados_atualiza_campos(trem, p_campos, nomesCamposAtualiza, valoresAtualiza);

                    // Reposiciona o ponteiro no início do registro atual e sobrescreve
                    fseek(file, offset_atual, SEEK_SET);
                    dados_grava_binario(trem, file);

                    // Força o ponteiro a voltar para o final do registro atual (pronto para a próxima leitura)
                    fseek(file, offset_atual + 80, SEEK_SET);
                }
            }

            // libera a memória alocada para o registro lido
            dados_apaga(trem);
            rrn++;
        }
    }

    // Grava o status de consistente no cabeçalho e fecha
    status = '1';
    fseek(file, 0, SEEK_SET);
    fwrite(&status, sizeof(char), 1, file);
    fclose(file);

    BinarioNaTela(nomeArqBin);
}