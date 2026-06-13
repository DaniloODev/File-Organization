#ifndef DELETE_H
#define DELETE_H

// Exclui a partir dos filtros passados pelo usuário, não sendo obrigatório o uso
// de um arquivo de indexação, apesar de um desempenho superior quando utilizado
// sendo capaz de efetuar remoção sequencial e através da estrutura de árvore.
void DELETE_WHERE(char *nomeArqBin, char *nomeArqArvoreB, int n_buscas);

#endif