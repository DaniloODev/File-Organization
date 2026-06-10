#include <stdio.h>
#include "../structs/dados.h"
#ifndef AUXILIARES_H
#define AUXILIAReS_H

void leitura_campos(int n, char nomes[][50], char valores[][100]);   // Leitura de campos variados
int leitura(FILE *file, char *buffer);          // Nossa função que substitui o fscanf para o CSV
int calcTam(char *str);                         // Calcula tamanho da string
FILE *abre_verifica_rb(char *nomeArqBin);       // Abre o arquivo e verifica a integridade rb
FILE *abre_verifica_rbplus(char *nomeArqBin);    // Abre o arquivo e verifica integridade rb+ 
FILE *abre_verifica_wb(char *nomeArqBin);       // Abre o arquivo e verifica integridade wb
FILE *abre_verifica_wbplus(char *nomeArqBin);    // Abre o arquivo e verifica integridade wb+
FILE *abre_verifica_r(char *nomeArq);    // Abre o arquivo e verifica integridade r
void printa_estacao(dados *trem);

#endif