#include "update.h"
#include "dados.h"
#include "auxiliares.h"
#include "fornecidas.h"
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

    char status = '0';  // Atualiza o arquivo para inconsistente
    fseek(file, 0, SEEK_SET);
    fwrite(&status, sizeof(char), 1, file);

    for (int b = 0; b < n_buscas; b++)  
{ 
    // Busca
    int m_campos;
    scanf("%d", &m_campos);
    char nomesCamposBusca[m_campos][50];
    char valoresBusca[m_campos][100];

    leitura_campos(m_campos, nomesCamposBusca, valoresBusca);   // Leitura dos campos

    // Atualiza valores
    int p_campos;
    scanf("%d", &p_campos);
    char nomesCamposAtualiza[p_campos][50];
    char valoresAtualiza[p_campos][100];

    leitura_campos(p_campos, nomesCamposAtualiza, valoresAtualiza); // Leitura dos campos atualizados

        fseek(file, 17, SEEK_SET);  // Busca no arquivo 
        dados trem;
        int rrn = 0;

        while (fread(&trem.removido, sizeof(char), 1, file)) 
        {
            long offset_atual = 17 + (rrn * 80);    // Cálculo do Byteoffset atual 

            // Le o registro
            fread(&trem.proxRemovido, sizeof(int), 1, file);
            fread(&trem.codEstacao, sizeof(int), 1, file);
            fread(&trem.codLinha, sizeof(int), 1, file);
            fread(&trem.codProxEstacao, sizeof(int), 1, file);
            fread(&trem.distProxEstacao, sizeof(int), 1, file);
            fread(&trem.codLinhaIntegra, sizeof(int), 1, file);
            fread(&trem.codEstIntegra, sizeof(int), 1, file);
            
            fread(&trem.tamNomeEstacao, sizeof(int), 1, file);
            if (trem.tamNomeEstacao > 0 && trem.tamNomeEstacao <= 80) {
                trem.nomeEstacao = malloc(trem.tamNomeEstacao + 1);
                fread(trem.nomeEstacao, sizeof(char), trem.tamNomeEstacao, file);
                trem.nomeEstacao[trem.tamNomeEstacao] = '\0';
            } else trem.nomeEstacao = NULL;

            fread(&trem.tamNomeLinha, sizeof(int), 1, file);
            if (trem.tamNomeLinha > 0 && trem.tamNomeLinha <= 80) {
                trem.nomeLinha = malloc(trem.tamNomeLinha + 1);
                fread(trem.nomeLinha, sizeof(char), trem.tamNomeLinha, file);
                trem.nomeLinha[trem.tamNomeLinha] = '\0';
            } else trem.nomeLinha = NULL;

            int bytesLidos = 37 + trem.tamNomeEstacao + trem.tamNomeLinha;  // Pula o lixo
            fseek(file, 80 - bytesLidos, SEEK_CUR);

            if (trem.removido == '0')   // verifica o registro e valida
            {
                int match = valida_registro(&trem, m_campos, nomesCamposBusca, valoresBusca);

                if (match)
                {
                    // Modifica na memória
                    for (int k = 0; k < p_campos; k++) {
                        int valorInt_Novo = (strcmp(valoresAtualiza[k], "NULO") == 0) ? -1 : atoi(valoresAtualiza[k]);

                        if (strcmp(nomesCamposAtualiza[k], "codEstacao") == 0)
                            trem.codEstacao = valorInt_Novo;
                        else if (strcmp(nomesCamposAtualiza[k], "codLinha") == 0)
                            trem.codLinha = valorInt_Novo;
                        else if (strcmp(nomesCamposAtualiza[k], "codProxEstacao") == 0)
                            trem.codProxEstacao = valorInt_Novo;
                        else if (strcmp(nomesCamposAtualiza[k], "distProxEstacao") == 0)
                            trem.distProxEstacao = valorInt_Novo;
                        else if (strcmp(nomesCamposAtualiza[k], "codLinhaIntegra") == 0)
                            trem.codLinhaIntegra = valorInt_Novo;
                        else if (strcmp(nomesCamposAtualiza[k], "codEstIntegra") == 0)
                            trem.codEstIntegra = valorInt_Novo;

                        else if (strcmp(nomesCamposAtualiza[k], "nomeEstacao") == 0)
                        {
                            if (trem.nomeEstacao != NULL) free(trem.nomeEstacao);
                            if (strcmp(valoresAtualiza[k], "") == 0) {
                                trem.tamNomeEstacao = 0;
                                trem.nomeEstacao = NULL;
                            } else {
                                trem.tamNomeEstacao = strlen(valoresAtualiza[k]);
                                trem.nomeEstacao = malloc(trem.tamNomeEstacao + 1);
                                strcpy(trem.nomeEstacao, valoresAtualiza[k]);
                            }
                        }
                        else if (strcmp(nomesCamposAtualiza[k], "nomeLinha") == 0)
                        {
                            if (trem.nomeLinha != NULL) free(trem.nomeLinha);
                            if (strcmp(valoresAtualiza[k], "") == 0) {
                                trem.tamNomeLinha = 0;
                                trem.nomeLinha = NULL;
                            } else {
                                trem.tamNomeLinha = strlen(valoresAtualiza[k]);
                                trem.nomeLinha = malloc(trem.tamNomeLinha + 1);
                                strcpy(trem.nomeLinha, valoresAtualiza[k]);
                            }
                        }
                    }

                    // Escreve de volta no arquivo
                    fseek(file, offset_atual, SEEK_SET);

                    fwrite(&trem.removido, sizeof(char), 1, file);
                    fwrite(&trem.proxRemovido, sizeof(int), 1, file);
                    fwrite(&trem.codEstacao, sizeof(int), 1, file);
                    fwrite(&trem.codLinha, sizeof(int), 1, file);
                    fwrite(&trem.codProxEstacao, sizeof(int), 1, file);
                    fwrite(&trem.distProxEstacao, sizeof(int), 1, file);
                    fwrite(&trem.codLinhaIntegra, sizeof(int), 1, file);
                    fwrite(&trem.codEstIntegra, sizeof(int), 1, file);
                    
                    fwrite(&trem.tamNomeEstacao, sizeof(int), 1, file);
                    if(trem.tamNomeEstacao > 0) fwrite(trem.nomeEstacao, sizeof(char), trem.tamNomeEstacao, file);
                    
                    fwrite(&trem.tamNomeLinha, sizeof(int), 1, file);
                    if(trem.tamNomeLinha > 0) fwrite(trem.nomeLinha, sizeof(char), trem.tamNomeLinha, file);

                    // Ajusta novamente os lixos 
                    int bytesEscritos = 37 + trem.tamNomeEstacao + trem.tamNomeLinha;
                    char lixo = '$';
                    for (int k = 0; k < (80 - bytesEscritos); k++)
                        fwrite(&lixo, sizeof(char), 1, file);

                    // Reposiciona o ponteiro pro fim do registro para a proxima leitura não estourar
                    fseek(file, offset_atual + 80, SEEK_SET);
                }
            }

            if(trem.nomeEstacao) free(trem.nomeEstacao);
            if(trem.nomeLinha) free(trem.nomeLinha);
            rrn++;
        }
    }

    // Grava o status de consistente e fecha
    status = '1';
    fseek(file, 0, SEEK_SET);
    fwrite(&status, sizeof(char), 1, file);
    fclose(file);

    BinarioNaTela(nomeArqBin);
}
