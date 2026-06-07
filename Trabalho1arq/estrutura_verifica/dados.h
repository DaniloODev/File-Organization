#include <stdio.h>
#ifndef DADOS_H
#define DADOS_H

typedef struct _dados
{
  char removido;
  int proxRemovido;
  int codEstacao;
  int codLinha;
  int codProxEstacao;
  int distProxEstacao;
  int codLinhaIntegra;
  int codEstIntegra;
  int tamNomeEstacao;
  char *nomeEstacao;
  int tamNomeLinha;
  char *nomeLinha;
}dados;

// Função para validar se um registro atende aos criterios da busca
int valida_registro(dados *trem, int m_campos, char nomesCampos[][50], char valoresBusca[][100]);   // Função validar registro

#endif