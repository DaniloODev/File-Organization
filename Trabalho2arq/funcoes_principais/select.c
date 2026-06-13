#include "../aux/auxiliares.h"
#include "../fornecidas/fornecidas.h"
#include "../structs/dados.h"
#include "../structs/arvore_b.h"
#include "../structs/cabecalho_arvb.h"
#include "select.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/// @brief              Abre o arquivo, percorre e printa os registros ativos.
/// @param nomeArqBin   Nome do arquivo binário.
void SELECT_FROM(char *nomeArqBin)
{
    FILE *file = abre_verifica_rb(nomeArqBin);  // Abre e verifica o arquivo de dados
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

        dados_apaga(trem); // Libera memória previamente alocada
    }

    if (!encontrou)
        printf("Registro inexistente.\n");  // Se não encontrou, não existe

    fclose(file);
}

/// @brief                  Abre o arquivo, percorre e lista os resultados filtrados pelas buscas.
/// @param nomeArqBin       Nome do arquivo binário.
/// @param numero_buscas    Número de buscas a serem realizadas.
void SELECT_WHERE(char *nomeArqBin, char *nomeArqArvoreB, int numero_buscas)
{
    FILE *file = abre_verifica_rb(nomeArqBin);  // Avre e verifica o arquivo de dados
    if (file == NULL) return;

    for (int b = 0; b < numero_buscas; b++)
    {
        // reseta variáveis a cada nova busca
        int indice_codEstacao = -1;
        int encontrouAoMenosUm = 0;
        int m_campos;
        
        scanf("%d", &m_campos); // Numero de buscas a serem realizadas

        char nomesCampos[m_campos][50];
        char valoresBusca[m_campos][100];

        // Faz a leitura do que será buscado
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

        int noRaiz;

        // Se for passado um arquivo de indexação e um codigo de estação
        // (primary key), fazemos a busca utilizando a arvore
        if (indice_codEstacao != -1 && nomeArqArvoreB != NULL) {
            FILE *file_arvb = abre_verifica_rb(nomeArqArvoreB); // Abre e verifica o arquivio de indexação
            if (file_arvb == NULL) continue; 

            int chave_procurada = atoi(valoresBusca[indice_codEstacao]);

            // Carrgando o cabeçalho da arvore e pegando informações
            cabecalho_arvb *cab_arvb = leCabecalhoArvb(file_arvb);
            noRaiz = getNoRaizArvb(cab_arvb);

            long offset = busca_arvore_b(file_arvb, noRaiz, chave_procurada);
            if(offset != -1){   // Se encontrou o ofsset, achou na busca
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
        
            if (!encontrouAoMenosUm) {  // Se não encontrou
                printf("Registro inexistente.\n");
            }
            
            fclose(file_arvb);
        } else {
            // Busca sequencial, caso não tenha sido passado um arquivo de index
            fseek(file, 17, SEEK_SET); // Reposiciona no início dos registros de dados para cada busca
            
            dados *trem = NULL;

            while ((trem = dados_le_binario(file)) != NULL) // Enquanto der para ler o arquivo
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
            
            if (!encontrouAoMenosUm) {  // Se não encontrou, printa que não existe
                printf("Registro inexistente.\n");
            }
        }
        printf("\n");
    }
    fclose(file);
}