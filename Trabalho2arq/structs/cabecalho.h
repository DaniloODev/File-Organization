#include <stdio.h>
#ifndef CABECALHO_H
#define CABECALHO_H

typedef struct _cabecalho cabecalho;

// Funções para inicializar os cabeçalhos com valores padrão
cabecalho* inicializaCabecalho();
// Funções para liberar a memória alocada para os cabeçalhos
void finalizaCabecalho(cabecalho *c);
// Funções para gravar os cabeçalhos
void gravaCabecalho(FILE *binFile, cabecalho *c);
// Funções para atualizar dados dos cabeçalhos antes de gravar
void atualizaCabecalho(cabecalho *c, char status, int topo,int proxRRN, int nroEstacoes, int nroPares);

#endif
