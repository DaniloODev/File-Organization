#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "auxiliares.h"
#include "fornecidas.h"
#include "dados.h"
#include "printaEstacao.h"
#include "busca.h"
#include "select.h"
#include "calcula.h"

void busca (dados trem, FILE *file, int funcao, char **valoresBusca, char **nomesCampos, int m_campos, char **nomesCamposAtualiza, char **valoresAtualiza, int p_campos) // funcao: 0 para selectFrom, 1 para selectWhere, 2 para update, 3 para delete
{
    int encontrou = 0, rrn = 0;
    int topo, proxRRN, nroEstacoes, nroParesEstacao;

    if (funcao == 2 || funcao == 3) { 
        // Status inconsistente durante a alteração
        char status = '0';
        fseek(file, 0, SEEK_SET);
        fwrite(&status, sizeof(char), 1, file);
    }

    if (funcao == 3) { // Se for delete, precisa ler o topo e o proxRRN para atualizar a lista de removidos
        fread(&topo, sizeof(int), 1, file);
        fread(&proxRRN, sizeof(int), 1, file);
        fread(&nroEstacoes, sizeof(int), 1, file);
        fread(&nroParesEstacao, sizeof(int), 1, file);
    }

    fseek(file, 17, SEEK_SET);  // Pula o cabeçalho (17 bytes) para chegar nos registros de dados

    while (fread(&trem.removido, sizeof(char), 1, file))
    {   
        long offset_registro_atual = 17 + (long)(rrn * 80);
        // Leitura dos campos fixos
        fread(&trem.proxRemovido, sizeof(int), 1, file);
        fread(&trem.codEstacao, sizeof(int), 1, file);
        fread(&trem.codLinha, sizeof(int), 1, file);
        fread(&trem.codProxEstacao, sizeof(int), 1, file);
        fread(&trem.distProxEstacao, sizeof(int), 1, file);
        fread(&trem.codLinhaIntegra, sizeof(int), 1, file);
        fread(&trem.codEstIntegra, sizeof(int), 1, file);
        
        // Leitura do nome da estação
        fread(&trem.tamNomeEstacao, sizeof(int), 1, file);
        trem.nomeEstacao = malloc((trem.tamNomeEstacao + 1) * sizeof(char));
        fread(trem.nomeEstacao, sizeof(char), trem.tamNomeEstacao, file);
        trem.nomeEstacao[trem.tamNomeEstacao] = '\0';
        // Leitura do nome da linha
        fread(&trem.tamNomeLinha, sizeof(int), 1, file);
        if (trem.tamNomeLinha > 0) {
            trem.nomeLinha = malloc((trem.tamNomeLinha + 1) * sizeof(char));
            fread(trem.nomeLinha, sizeof(char), trem.tamNomeLinha, file);
            trem.nomeLinha[trem.tamNomeLinha] = '\0';
        } else trem.nomeLinha = NULL;
        // Pula o lixo $ até completar os 80 bytes do registro (37 bytes fixos + tamanhos das strings)
        int bytesLidos = 37 + trem.tamNomeEstacao + trem.tamNomeLinha;
        fseek(file, 80 - bytesLidos, SEEK_CUR);

        if (funcao == 0) {
            if (trem.removido == '1') {   // Se estiver removido, limpa a memória e pula para o próximo
                if (trem.nomeEstacao != NULL) free(trem.nomeEstacao);
                if (trem.nomeLinha != NULL) free(trem.nomeLinha);
                continue;
            }
            encontrou = 1;
            printa_estacao(trem);   // Mostrando as estações encontradas

            // Desalocando a memória préviamente alocada
            if (trem.nomeEstacao != NULL) free(trem.nomeEstacao);
            if (trem.nomeLinha != NULL) free(trem.nomeLinha);
        }

        if (funcao == 1) {
            if (trem.removido == '1') {
                free(trem.nomeEstacao);
                if (trem.nomeLinha) free(trem.nomeLinha);
                continue;
            }

            int match = valida_registro(&trem, m_campos, nomesCampos, valoresBusca); // Assume que o registro serve, a menos que falhe em um dos critérios.
            if (match)
            {
                encontrou = 1;
                printa_estacao(trem);   // Mostrando as estações encontradas

            }
            free(trem.nomeEstacao);
            if (trem.nomeLinha)
                free(trem.nomeLinha);
                
        }

        if (funcao == 2) {
            if (trem.removido == '0')   // verifica o registro e valida
            {
                int match = valida_registro(&trem, m_campos, nomesCampos, valoresBusca);

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
                    fseek(file, offset_registro_atual, SEEK_SET);

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
                    fseek(file, offset_registro_atual + 80, SEEK_SET);
                }
            }

            if(trem.nomeEstacao) free(trem.nomeEstacao);
            if(trem.nomeLinha) free(trem.nomeLinha);
            rrn++;
        }

        if (funcao == 3) {
            if (trem.removido == '0') // Só tenta deletar se não estiver deletado
            {
                if (valida_registro(&trem, m_campos, nomesCampos, valoresBusca))
                {
                    char flag_removido = '1';
                    int prox_na_lista = topo; // O registro atual aponta para onde o topo apontava

                    fseek(file, offset_registro_atual, SEEK_SET);
                    fwrite(&flag_removido, sizeof(char), 1, file);
                    fwrite(&prox_na_lista, sizeof(int), 1, file);

                    topo = rrn; // Atualiza topo para o rrn atual, que foi o ultimo a ser deletado
                    
                    // Garante que o ponteiro volte para o fim do registro para a próxima leitura do while
                    fseek(file, offset_registro_atual + 80, SEEK_SET);
                }
            }

            if(trem.nomeEstacao) free(trem.nomeEstacao);
            if(trem.nomeLinha) free(trem.nomeLinha);
            rrn++;
        }
    }

    if (funcao == 0 || funcao == 1) {
        if (!encontrou) printf("Registro inexistente.\n");
        return; // Retorna se encontrou ou não, para o select
    }

    if (funcao == 2) {
        char status = '1';
        fseek(file, 0, SEEK_SET);
        fwrite(&status, sizeof(char), 1, file);
        return;
    }
    if (funcao == 3) {
        char status = '1';
        fseek(file, 0, SEEK_SET);
        fwrite(&status, sizeof(char), 1, file);
        recalcula_e_grava_cabecalho(file, topo, proxRRN);
        return; // Atualiza o status para consistente e retorna 1 para indicar que a operação foi realizada
    }
}