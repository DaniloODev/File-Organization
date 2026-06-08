#include <stdio.h>
#ifndef CABECALHO_H
#define CABECALHO_H

typedef struct _cabecalho cabecalho;
typedef struct _cabecalho_arvb cabecalho_arvb;

// Funções para inicializar os cabeçalhos com valores padrão
cabecalho* inicializaCabecalho();
cabecalho_arvb* inicializaCabecalhoArvb();
// Funções para liberar a memória alocada para os cabeçalhos
void finalizaCabecalho(cabecalho *c);
void finalizaCabecalhoArvb(cabecalho_arvb *c_arvb);
// Funções para gravar os cabeçalhos
void gravaCabecalho(FILE *binFile, cabecalho *c);
void gravaCabecalhoArvb(FILE *binFile, cabecalho_arvb *c_arvb);

// Funções para atualizar dados dos cabeçalhos antes de gravar
void atualizaCabecalho(cabecalho *c, char status, int topo,int proxRRN, int nroEstacoes, int nroPares);
void atualizaCabecalhoArvb(cabecalho_arvb *c_arvb, char status, int noRaiz, int topo, int proxRRN, int nroNos);

#endif
