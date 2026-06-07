#include <stdio.h>
#ifndef CABECALHO_H
#define CABECALHO_H

typedef struct _cabecalho cabecalho;

cabecalho* inicializaCabecalho();                   // Função para inicializar o cabeçalho com valores padrão
void finalizaCabecalho(cabecalho *c);               // Função para liberar a memória alocada para o cabeçalho
void gravaCabecalho(FILE *binFile, cabecalho *c);   // Função para gravar o cabeçalho

// Função para atualizar dados do cabeçalho antes de gravar
void atualizaCabecalho(cabecalho *c, char status, int topo,int proxRRN, int nroEstacoes, int nroPares);

#endif
