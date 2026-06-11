#include "create_table.h"
#include "../structs/cabecalho.h"
#include "../structs/dados.h"
#include "../fornecidas/fornecidas.h"
#include "./aux/auxiliares.h"
#include "./aux/calcula.h"
#include <stdlib.h>
#include <string.h>

/// @brief          Dado um arquivo, ele faz a leitura e transforma em binário
/// @param nomeArq  Nome do arquivo CSV a ser lido.
/// @param arqBin   Nome do arquivo binário que será gerado.
void CREATE_TABLE(char *nomeArq, char *arqBin)
{
  FILE *file = abre_verifica_r(nomeArq);
  if (file == NULL) return;

  FILE *binFile = abre_verifica_wbplus(arqBin);
  if (binFile == NULL){ fclose(file); return; }

  char ignore[1024];
  fgets(ignore, sizeof(ignore), file);

  cabecalho *cab = inicializaCabecalho();  
  gravaCabecalho(binFile, cab); 
  
  char buffer[100];                     
  int i = 0; 

  while(leitura(file, buffer))
  { 
    if (calcTam(buffer) == 0) break;

    dados *reg = dados_cria(); // Aloca um novo registro limpo

    int codEstacao = atoi(buffer);
    
    leitura(file, buffer);
    dados_set_nomeEstacao(reg, buffer);

    leitura(file, buffer);
    int codLinha = (calcTam(buffer) > 0) ? atoi(buffer) : -1;

    leitura(file, buffer);
    dados_set_nomeLinha(reg, (calcTam(buffer) > 0) ? buffer : "");

    leitura(file, buffer);
    int codProxEstacao = (calcTam(buffer) > 0) ? atoi(buffer) : -1;

    leitura(file, buffer);
    int distProxEstacao = (calcTam(buffer) > 0) ? atoi(buffer) : -1;

    leitura(file, buffer);
    int codLinhaIntegra = (calcTam(buffer) > 0) ? atoi(buffer) : -1;

    leitura(file, buffer);
    int codEstIntegra = (calcTam(buffer) > 0) ? atoi(buffer) : -1;

    // Seta todos os campos de tamanho fixo de uma vez
    dados_set_campos_fixos(reg, '0', -1, codEstacao, codLinha, codProxEstacao, distProxEstacao, codLinhaIntegra, codEstIntegra);

    // Manda o registro escrever no arquivo binário
    dados_grava_binario(reg, binFile);

    // Como já gravou no disco, pode liberar a memória préviamente alocada
    dados_apaga(reg); 
    
    i++;
  }
  
  recalcula_e_grava_cabecalho(binFile, -1, i); 

  fclose(file);
  fclose(binFile);
  BinarioNaTela(arqBin);
}
