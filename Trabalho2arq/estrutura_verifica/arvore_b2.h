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
  int C1;
  int Pr1;
  int C2;
  int Pr2;
  int C3;
  int Pr3;
  int P1;
  int P2;
  int P3;
  int P4;
} no;

#endif