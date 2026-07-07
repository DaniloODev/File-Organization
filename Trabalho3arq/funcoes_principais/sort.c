#include "sort.h"
#include "../aux/auxiliares.h"
#include "../fornecidas/fornecidas.h"
#include "../structs/dados.h"
#include "../structs/cabecalho.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static char campo_atual[30];

// Função de comparação unificada para o qsort
int compara_registros(const void *a, const void *b) {
    dados *regA = *(dados **)a;
    dados *regB = *(dados **)b;

    int vA = (strcmp(campo_atual, "codEstacao") == 0) ? dados_get_codEstacao(regA) : dados_get_codProxEstacao(regA);
    int vB = (strcmp(campo_atual, "codEstacao") == 0) ? dados_get_codEstacao(regB) : dados_get_codProxEstacao(regB);

    // Trata valores nulos (-1) colocando-os adequadamente na ordenação
    if (vA == -1 && vB != -1) return -1;
    if (vA != -1 && vB == -1) return 1;

    // Evita vA - vB para prevenir Integer Overflow/Underflow
    if (vA < vB) return -1;
    if (vA > vB) return 1;
    return 0;
}

// Carrega os dados ativos de um arquivo binário para a RAM e ordena usando qsort
dados** carregar_e_ordenar(char *nomeArq, char *nomeCampo, int *tamOut, cabecalho **cabOut) {
    FILE *f = abre_verifica(nomeArq, "rb");
    if (!f) return NULL;

    *cabOut = leCabecalhoDados(f);
    if (getStatusCabecalho(*cabOut) == '0') {
        finalizaCabecalho(*cabOut);
        *cabOut = NULL;
        fclose(f);
        return NULL;
    }

    fseek(f, 17, SEEK_SET);

    int cap = 100;
    int tam = 0;
    dados **array = malloc(cap * sizeof(dados *));
    if (!array) {
        finalizaCabecalho(*cabOut);
        *cabOut = NULL;
        fclose(f);
        return NULL;
    }

    dados *reg = NULL;
    while ((reg = dados_le_binario(f)) != NULL) {
        if (dados_get_removido(reg) == '1') {
            dados_apaga(reg);
            continue;
        }

        if (tam >= cap) {
            cap *= 2;
            dados **novo_array = realloc(array, cap * sizeof(dados *));
            if (!novo_array) {
                // Se falhar o realloc, limpa tudo o que já foi alocado
                for (int i = 0; i < tam; i++) dados_apaga(array[i]);
                free(array);
                finalizaCabecalho(*cabOut);
                *cabOut = NULL;
                fclose(f);
                return NULL;
            }
            array = novo_array;
        }
        array[tam++] = reg;
    }

    fclose(f);

    if (tam == 0) {
        free(array);
        finalizaCabecalho(*cabOut);
        *cabOut = NULL;
        *tamOut = 0;
        return NULL; // Retorna NULL indicando que não há registros a ordenar
    }

    // Configura a variável global antes da chamada ao qsort
    strncpy(campo_atual, nomeCampo, sizeof(campo_atual) - 1);
    campo_atual[sizeof(campo_atual) - 1] = '\0';

    qsort(array, tam, sizeof(dados *), compara_registros);

    *tamOut = tam;
    return array;
}

// Funcionalidade 13: Ordenação em RAM
void ordena_arquivo_ram(char *nomeArqIn, char *nomeCampo, char *nomeArqSaida) {
    int tam = 0;
    cabecalho *cabIn = NULL;
    dados **array = carregar_e_ordenar(nomeArqIn, nomeCampo, &tam, &cabIn);

    // Se carregar_e_ordenar retornar NULL, ou houve erro ou o arquivo está vazio/inexistente
    if (array == NULL) {
        printf("Registro inexistente.\n");
        return;
    }

    FILE *fSaida = abre_verifica(nomeArqSaida, "wb");
    if (!fSaida) {
        for (int i = 0; i < tam; i++) dados_apaga(array[i]);
        free(array);
        finalizaCabecalho(cabIn);
        printf("Falha no processamento do arquivo.\n");
        return;
    }

    // Grava o cabeçalho como consistente no arquivo de saída
    setStatus(cabIn, '1');
    gravaCabecalho(fSaida, cabIn);

    for (int i = 0; i < tam; i++) {
        dados_grava_binario(array[i], fSaida);
        dados_apaga(array[i]);
    }

    free(array);
    finalizaCabecalho(cabIn);
    fclose(fSaida);

    BinarioNaTela(nomeArqSaida);
}

// Funcionalidade 14: Sort-Merge Join
void SELECT_SORT_MERGE(char *nomeArq1, char *nomeCampo1, char *nomeArq2, char *nomeCampo2) {
    int tam1 = 0, tam2 = 0; 
    cabecalho *c1 = NULL, *c2 = NULL;
    
    dados **arr1 = carregar_e_ordenar(nomeArq1, nomeCampo1, &tam1, &c1);
    dados **arr2 = carregar_e_ordenar(nomeArq2, nomeCampo2, &tam2, &c2);

    if (!arr1 || !arr2) { 
        printf("Falha no processamento do arquivo.\n");
        if (arr1) { for(int i=0; i<tam1; i++) dados_apaga(arr1[i]); free(arr1); finalizaCabecalho(c1); }
        if (arr2) { for(int i=0; i<tam2; i++) dados_apaga(arr2[i]); free(arr2); finalizaCabecalho(c2); }
        return; 
    }

    int i = 0, j = 0, encontrou = 0;
    while (i < tam1 && j < tam2) {
        int v1 = dados_get_codProxEstacao(arr1[i]);
        int v2 = dados_get_codEstacao(arr2[j]);

        if (v1 == -1) { i++; continue; }
        if (v2 == -1) { j++; continue; }

        if (v1 < v2) {
            i++;
        } else if (v1 > v2) {
            j++;
        } else {
            // Encontrou uma correspondência. Como pode haver valores duplicados em arr2,
            // precisamos varrer o bloco de elementos iguais.
            int k = j;
            while (k < tam2 && dados_get_codEstacao(arr2[k]) == v1) {
                encontrou = 1;
                dados_printa_juncao(arr1[i], arr2[k]);
                k++;
            }
            i++;
        }
    }

    if (!encontrou) {
        printf("Registro inexistente.\n");
    }

    // Liberação total da memória alocada para os vetores e registros
    for (int idx = 0; idx < tam1; idx++) dados_apaga(arr1[idx]);
    free(arr1);
    finalizaCabecalho(c1);

    for (int idx = 0; idx < tam2; idx++) dados_apaga(arr2[idx]);
    free(arr2);
    finalizaCabecalho(c2);
}