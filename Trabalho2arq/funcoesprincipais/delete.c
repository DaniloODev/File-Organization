#include "delete.h"
#include "dados.h"
#include "fornecidas.h"
#include "auxiliares.h"
#include "calcula.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/// @brief              Exclui de acordo com o conteúdo desejado, do campo selecionado.
/// @param nomeArqBin   Nome do arquivo a ser manipulado.
/// @param n_buscas     Número de buscas a serem efetuadas.
void DELETE_WHERE(char *nomeArqBin, int n_buscas)
{
    FILE *binFile = abre_verifica_rbplus(nomeArqBin);   // Verifica integridade do arquivo

    int topo, proxRRN, nroEstacoes, nroParesEstacao;
    fread(&topo, sizeof(int), 1, binFile);
    fread(&proxRRN, sizeof(int), 1, binFile);
    fread(&nroEstacoes, sizeof(int), 1, binFile);
    fread(&nroParesEstacao, sizeof(int), 1, binFile);

    // Status inconsistente durante a alteração
    char status = '0';
    fseek(binFile, 0, SEEK_SET);
    fwrite(&status, sizeof(char), 1, binFile);

    for (int b = 0; b < n_buscas; b++)
    {
        int m_campos;
        scanf("%d", &m_campos);

        char nomesCampos[m_campos][50];
        char valoresBusca[m_campos][100];

        leitura_campos(m_campos, nomesCampos, valoresBusca);   // Leitura dos campos

        // SEMPRE volte para o início dos registros e zere o RRN para cada busca
        fseek(binFile, 17, SEEK_SET); 
        dados trem;
        int rrn = 0; 

        while (fread(&trem.removido, sizeof(char), 1, binFile))
        {
            long offset_registro_atual = 17 + (long)(rrn * 80);

            // Leitura dos campos fixos
            fread(&trem.proxRemovido, sizeof(int), 1, binFile);
            fread(&trem.codEstacao, sizeof(int), 1, binFile);
            fread(&trem.codLinha, sizeof(int), 1, binFile);
            fread(&trem.codProxEstacao, sizeof(int), 1, binFile);
            fread(&trem.distProxEstacao, sizeof(int), 1, binFile);
            fread(&trem.codLinhaIntegra, sizeof(int), 1, binFile);
            fread(&trem.codEstIntegra, sizeof(int), 1, binFile);
            
            // Leitura dos campos variáveis
            fread(&trem.tamNomeEstacao, sizeof(int), 1, binFile);
            trem.nomeEstacao = malloc(trem.tamNomeEstacao + 1);
            fread(trem.nomeEstacao, sizeof(char), trem.tamNomeEstacao, binFile);
            trem.nomeEstacao[trem.tamNomeEstacao] = '\0';

            fread(&trem.tamNomeLinha, sizeof(int), 1, binFile);
            if (trem.tamNomeLinha > 0) {
                trem.nomeLinha = malloc(trem.tamNomeLinha + 1);
                fread(trem.nomeLinha, sizeof(char), trem.tamNomeLinha, binFile);
                trem.nomeLinha[trem.tamNomeLinha] = '\0';
            } else {
                trem.nomeLinha = NULL;
            }

            // Pula o lixo para terminar os 80 bytes
            int bytesLidos = 37 + trem.tamNomeEstacao + trem.tamNomeLinha;
            fseek(binFile, 80 - bytesLidos, SEEK_CUR);

            if (trem.removido == '0') // Só tenta deletar se não estiver deletado
            {
                if (valida_registro(&trem, m_campos, nomesCampos, valoresBusca))
                {
                    char flag_removido = '1';
                    int prox_na_lista = topo; // O registro atual aponta para onde o topo apontava

                    fseek(binFile, offset_registro_atual, SEEK_SET);
                    fwrite(&flag_removido, sizeof(char), 1, binFile);
                    fwrite(&prox_na_lista, sizeof(int), 1, binFile);

                    topo = rrn; // Atualiza topo para o rrn atual, que foi o ultimo a ser deletado
                    
                    // Garante que o ponteiro volte para o fim do registro para a próxima leitura do while
                    fseek(binFile, offset_registro_atual + 80, SEEK_SET);
                }
            }

            if(trem.nomeEstacao) free(trem.nomeEstacao);
            if(trem.nomeLinha) free(trem.nomeLinha);
            rrn++;
        }
    }

    recalcula_e_grava_cabecalho(binFile, topo, proxRRN);

    fclose(binFile);
    BinarioNaTela(nomeArqBin);
}