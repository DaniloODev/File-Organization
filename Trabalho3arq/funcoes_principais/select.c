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
    FILE *file = abre_verifica(nomeArqBin, "rb");  // Abre e verifica o arquivo de dados
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
    FILE *file = abre_verifica(nomeArqBin, "rb");  // Avre e verifica o arquivo de dados
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
            FILE *file_arvb = abre_verifica(nomeArqArvoreB, "rb"); // Abre e verifica o arquivio de indexação
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
            
            finalizaCabecalhoArvb(cab_arvb);
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

void SELECT_ANINHADO(char *nomeArq, char *nome_campo1, char *nomeArq2, char *nome_campo2) {
    FILE *file1 = abre_verifica(nomeArq, "rb");
    FILE *file2 = abre_verifica(nomeArq2, "rb");
    
    // Tratamento de erro na abertura dos arquivos
    if (file1 == NULL || file2 == NULL) {
        if (file1) fclose(file1);
        if (file2) fclose(file2);
        return;
    }

    fseek(file1, 17, SEEK_SET);  // Pula o cabeçalho do arquivo 1

    dados *trem1 = NULL;
    dados *trem2 = NULL;
    int encontrou = 0;

    while ((trem1 = dados_le_binario(file1)) != NULL) {
        
        // Verifica se o registro externo está removido logicamente
        if (dados_get_removido(trem1) == '1') {
            dados_apaga(trem1); 
            continue;
        }

        int codProx = dados_get_codProxEstacao(trem1); 
        
        if (codProx == -1) { 
            dados_apaga(trem1);
            continue;
        }

        fseek(file2, 17, SEEK_SET);
        
        while ((trem2 = dados_le_binario(file2)) != NULL) {
            if (dados_get_removido(trem2) == '1') {
                dados_apaga(trem2);
                continue;
            }

            int codEst = dados_get_codEstacao(trem2);
            if (codProx == codEst) {
                encontrou = 1;
                
                dados_printa_juncao(trem1, trem2); 
                
                dados_apaga(trem2);
                break;
            }
            dados_apaga(trem2);
        }
        dados_apaga(trem1);
    }

    if (!encontrou) {
        printf("Registro inexistente.\n");
    }

    fclose(file1);
    fclose(file2);
    
}

void SELECT_LOOP_UNICO(char *nomeArq, char *nome_campo1, char *nomeArq2, char *nome_campo2, char *nomeArqIndice) {
    FILE *file1 = abre_verifica(nomeArq, "rb");
    FILE *file2 = abre_verifica(nomeArq2, "rb");
    FILE *file_arvb = abre_verifica(nomeArqIndice, "rb");

    if (file1 == NULL || file2 == NULL || file_arvb == NULL) {
        if (file1) fclose(file1);
        if (file2) fclose(file2);
        if (file_arvb) fclose(file_arvb);
        return;
    }

    // Carregando dados da árvore-B
    cabecalho_arvb *cab_arvb = leCabecalhoArvb(file_arvb);
    int noRaiz = getNoRaizArvb(cab_arvb);

    fseek(file1, 17, SEEK_SET);  // Pula o cabeçalho do arquivo 1
    
    dados *trem1 = NULL;
    int encontrou = 0;

    while ((trem1 = dados_le_binario(file1)) != NULL) {
        
        if (dados_get_removido(trem1) == '1') {
            dados_apaga(trem1);
            continue;
        }

        int codProx = dados_get_codProxEstacao(trem1);
        
        if (codProx != -1) {
            // Usa o índice árvore-B definido sobre o arquivo estacao2 para buscar o offset
            long offset = busca_arvore_b(file_arvb, noRaiz, codProx);
            
            // Se a busca retornar um offset válido
            if (offset != -1) {
                fseek(file2, offset, SEEK_SET);
                dados *trem2 = dados_le_binario(file2);
                
                // Valida o registro apontado pelo offset
                if (trem2 != NULL && dados_get_removido(trem2) == '0') {
                    encontrou = 1;
                    
                    // Mostra os campos desejados de estacao1 e estacao2
                    dados_printa_juncao(trem1, trem2);
                }
                if (trem2 != NULL) dados_apaga(trem2);
            }
        }
        dados_apaga(trem1);
    }

    if (!encontrou) {
        printf("Registro inexistente.\n");
    }

    fclose(file1);
    fclose(file2);
    fclose(file_arvb);

    finalizaCabecalhoArvb(cab_arvb);
}