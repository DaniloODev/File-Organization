#include "fornecidas.h"
#include "auxiliares.h"
#include "arvore_b.h"
#include "cabecalho.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>



/// @brief Procura a chave dentro de um único nó da Árvore-B.
/// @param n O nó (página) atualmente carregado na memória.
/// @param chave_busca O codEstacao que queremos encontrar.
/// @param pos Ponteiro para armazenar a posição encontrada. 
///            Se a chave existir, 'pos' guarda o índice dela em C e Pr.
///            Se NÃO existir, 'pos' guarda o índice do ponteiro P para o qual devemos descer.
/// @return 1 se encontrou a chave, 0 se não encontrou.
int valida_no(pagina *no, int chave_busca, int *pos) {
    for (int i = 0; i < no->nroChaves; i++) {
        if (chave_busca == no->C[i]) {
            *pos = i;
            return 1; // Encontrou! A informação está em no->Pr[i]
        }
        if (chave_busca < no->C[i]) {
            *pos = i; // A chave é menor que C[i], então temos que descer pelo filho à esquerda dele: P[i]
            return 0; // Não está neste nó
        }
    }
    // Se a chave for maior que todas as chaves do nó, devemos descer pelo último ponteiro ativo (P à direita da última chave)
    *pos = no->nroChaves;
    return 0;
}

/// @brief Busca um codEstacao no índice Árvore-B.
/// @param arq_indice Arquivo da árvore B já aberto para leitura.
/// @param rrn_atual O RRN da página atual (na primeira chamada, passe o noRaiz do cabeçalho).
/// @param chave_busca O codEstacao a ser procurado.
/// @return O byte offset (Pr) no arquivo de dados se encontrar, ou -1 se não encontrar.
long busca_arvore_b(FILE *arq_indice, int rrn_atual, int chave_busca) {
    // Condição de parada 1: Chegamos numa folha e o ponteiro desceu para o nulo (-1).
    if (rrn_atual == -1) {
        return -1; // A chave não existe na árvore
    }

    // Calcula onde a página está no arquivo e pula pra ela (17 bytes do cabeçalho + RRN * 53 bytes)
    long offset_pagina = 17 + ((long)rrn_atual * 53);
    fseek(arq_indice, offset_pagina, SEEK_SET);

    pagina no;
    fread(&no.removido, sizeof(char), 1, arq_indice);
    fread(&no.proximo, sizeof(int), 1, arq_indice);
    fread(&no.tipoNo, sizeof(int), 1, arq_indice);
    fread(&no.nroChaves, sizeof(int), 1, arq_indice);
    fread(no.C, sizeof(int), 3, arq_indice);  // Lê os 3 ints de uma vez para o array
    fread(no.Pr, sizeof(int), 3, arq_indice); // Lê os 3 ints de uma vez para o array
    fread(no.P, sizeof(int), 4, arq_indice);  // Lê os 4 ints de uma vez para o array

    int pos;
    int encontrou = valida_no(&no, chave_busca, &pos);

    if (encontrou) {
        // Condição de parada 2: Encontramos a chave! Retorna o ponteiro para o arquivo de dados.
        return no.Pr[pos];
    } else {
        // Não encontrou neste nó, vamos descer recursivamente para o filho correspondente
        return busca_arvore_b(arq_indice, no.P[pos], chave_busca);
    }
}