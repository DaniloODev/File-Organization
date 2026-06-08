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
  int C[2];
  int Pr[2];
  int P[3];
} no;

#endif