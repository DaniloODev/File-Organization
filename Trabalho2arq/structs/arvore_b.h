#ifndef ARVORE_B_H
#define ARVORE_B_H

#include <stdio.h>
#include "cabecalho_arvb.h"

typedef struct _no pagina;

long busca_arvore_b(FILE *arq_indice, int rrn_atual, int chave_busca);
void insere_arvore_b(FILE *arq_indice, cabecalho_arvb *cab, int chave, long byte_offset);

#endif