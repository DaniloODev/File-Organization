#ifndef ARVORE_B_H
#define ARVORE_B_H

#include <stdio.h>
#include "cabecalho_arvb.h"

typedef struct _no pagina;

void inicializa_no_vazio(pagina *no);
void le_no(FILE *arq_indice, int rrn, pagina *no);
void escreve_no(FILE *arq_indice, int rrn, pagina *no);
long busca_arvore_b(FILE *arq_indice, int rrn_atual, int chave_busca);  // Faz a busca na Árvore
void insere_arvore_b(FILE *arq_indice, cabecalho_arvb *cab, int chave, long byte_offset);   // Insere na árvore
int pega_rrn_disponivel_arvb(FILE *arq_indice, cabecalho_arvb *cab);
void remover_arvore_b(FILE *arq_indice, cabecalho_arvb *cab, int chave_procurada);

#endif