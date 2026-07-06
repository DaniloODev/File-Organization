#ifndef CABECALHO_ARVB_H
#define CABECALHO_ARVB_H

#include <stdio.h>

typedef struct _cabecalho_arvb cabecalho_arvb;

cabecalho_arvb* inicializaCabecalhoArvb();          // Inicializa cabeçalho
void finalizaCabecalhoArvb(cabecalho_arvb *c_arvb); // Finaliza cabeçalho
// Atualiza valores da estrutura cabeçalho da arvore b
void atualizaCabecalhoArvb(cabecalho_arvb *c_arvb, char status, int noRaiz, int topo, int proxRRN, int nroNos);

void escreveCabecalhoArvb(FILE *fp, cabecalho_arvb *c_arvb);    // Escreve o cabeçalho no arquivo de indexação
cabecalho_arvb* leCabecalhoArvb(FILE *fp);                      // Lê o arquivo e preenche o cabeçalho da arvore b

// Retorna informaçoes do cabeçalho do arquivo de indexação
char getStatusArvb(cabecalho_arvb *c_arvb);
int getNoRaizArvb(cabecalho_arvb *c_arvb);
int getTopoArvb(cabecalho_arvb *c_arvb);
int getProxRRNArvb(cabecalho_arvb *c_arvb);
int getNroNosArvb(cabecalho_arvb *c_arvb);

// Seta valores passados no cabeçalho do arquivo de indexação
void setNoRaizArvb(cabecalho_arvb *c_arvb, int noRaiz);
void setProxRRNArvb(cabecalho_arvb *c_arvb, int proxRRN);
void setNroNosArvb(cabecalho_arvb *c_arvb, int nroNos);
void setStatusArvb(cabecalho_arvb *c_arvb, char status);
void setTopoArvb(cabecalho_arvb *c_arvb, int topo);

#endif