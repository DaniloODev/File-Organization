#ifndef SORT_H
#define SORT_H

#include <stdio.h>

// Ordenação em Memória RAM
void ordena_arquivo_ram(char *nomeArqEntrada, char *nomeCampo, char *nomeArqSaida);

// Junção por Ordenação-Intercalação (Sort-Merge Join)
void SELECT_SORT_MERGE(char *nomeArq1, char *nomeCampo1, char *nomeArq2, char *nomeCampo2);

#endif