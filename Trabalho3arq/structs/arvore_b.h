#ifndef ARVORE_B_H
#define ARVORE_B_H

#include <stdio.h>
#include "cabecalho_arvb.h"

typedef struct _no pagina;

// Inicializa um nó da árvore com os valores padrões 
void inicializa_no_vazio(pagina *no);
// Lê um nó da arvore do arquivo de indexaçao e passa para o objeto
void le_no(FILE *arq_indice, int rrn, pagina *no);
// Escreve o nó da arvore no arquivo de indexação
void escreve_no(FILE *arq_indice, int rrn, pagina *no);
// Faz a busca na Árvore e retorna o offset do registro se caso encontrar, se não, retorna -1
long busca_arvore_b(FILE *arq_indice, int rrn_atual, int chave_busca);
// Insere na árvore seguindo o criterio de ocupação minima e caso de transbordar o nó
void insere_arvore_b(FILE *arq_indice, cabecalho_arvb *cab, int chave, long byte_offset);
// Verifica se o próximo offset disponivel é do topo ou no final do arquivo
int pega_rrn_disponivel_arvb(FILE *arq_indice, cabecalho_arvb *cab);
void remover_arvore_b(FILE *arq_indice, cabecalho_arvb *cab, int chave_procurada);

#endif