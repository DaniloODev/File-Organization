#include "dados.h"
#include <stdio.h>
#ifndef CALCULA_H
#define CALCULA_H

int calc_nroEstacoes(dados *trem, int totalRegistros);
int calc_nroPares(dados *trem, int totalRegistros);
void recalcula_e_grava_cabecalho(FILE *binFile, int topo, int proxRRN);

#endif