#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef ARVORE_BH
#define ARVORE_BH

typedef struct _no
{
  char removido;
  int proximo;
  int tipoNo;
  int nroChaves; 
  int C[3]; // Chaves de busca (codEstacao).
  int Pr[3];// Ponteiros de referência para o arquivo de dados (offset do registro correspondente a cada chave).
  int P[4]; // Ponteiros de referência (RRN) para as subárvores filhas. 
} pagina;

#endif