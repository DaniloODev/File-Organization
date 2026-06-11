#ifndef CABECALHO_ARVB_H
#define CABECALHO_ARVB_H

#include <stdio.h>

typedef struct _cabecalho_arvb cabecalho_arvb;

cabecalho_arvb* inicializaCabecalhoArvb();
void finalizaCabecalhoArvb(cabecalho_arvb *c_arvb);
void atualizaCabecalhoArvb(cabecalho_arvb *c_arvb, char status, int noRaiz, int topo, int proxRRN, int nroNos);

// Funções de Persistência (I/O)
void escreveCabecalhoArvb(FILE *fp, cabecalho_arvb *c_arvb);
cabecalho_arvb* leCabecalhoArvb(FILE *fp);

// Getters essenciais para a Árvore-B
char getStatusArvb(cabecalho_arvb *c_arvb);
int getNoRaizArvb(cabecalho_arvb *c_arvb);
int getTopoArvb(cabecalho_arvb *c_arvb);
int getProxRRNArvb(cabecalho_arvb *c_arvb);
int getNroNosArvb(cabecalho_arvb *c_arvb);

void setNoRaizArvb(cabecalho_arvb *c_arvb, int noRaiz);
void setProxRRNArvb(cabecalho_arvb *c_arvb, int proxRRN);
void setNroNosArvb(cabecalho_arvb *c_arvb, int nroNos);
void setStatusArvb(cabecalho_arvb *c_arvb, char status);

#endif