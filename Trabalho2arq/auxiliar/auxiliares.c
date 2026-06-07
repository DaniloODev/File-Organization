#include "auxiliares.h"
#include "fornecidas.h"
#include <string.h>

/// @brief          Serve para ler o arquivo palavra por palavra, 
///                 retornando 0 ou 1 que é conveniente para usar dentro de um loop
/// @param file     Ponteiro para o arquivo na stack 
/// @param buffer   Ponteiro para pegar a string contida no arquivo
/// @return         Retorna 0 ao acabar o arquivo e 1 enquanto nao terminar a leitura do arquivo
int leitura(FILE *file, char *buffer)
{
    int i = 0;
    char c = fgetc(file);

    if (c == EOF) return 0; // Acabou o arquivo

    while (c != ',' && c != '\n' && c != '\r' && c != EOF) {
        buffer[i++] = c;
        c = fgetc(file);
    }

    if (c == '\r') fgetc(file); // Limpa o \n do Windows se houver

    buffer[i] = '\0';
    return 1;
}

/// @brief      Calcula a quantidade de caracteres em uma string e retorna este valor
/// @param str  O vetor de caracteres a ser contado 
/// @return     O número correspondente ao tamanho da string 
int calcTam(char *str)
{
  int tam = 0;
  if(str == NULL) return 0;
  for(int i = 0; str[i] != '\0'; i++)
    tam++;

  return tam;
}

/// @brief            Abre o arquivo, e efetua a verificação se foi possível abrir e a respeito da integridade.
/// @param nomeArqBin Nome do arquivo a ser manipulado.
/// @return           Retorna o ponteiro do arquivo.
FILE *abre_verifica_rb(char *nomeArqBin)
{
  FILE *file = fopen(nomeArqBin, "rb");
    if (file == NULL) {
        printf("Falha no processamento do arquivo.\n");
        return NULL;
    }
    char status;
    fread(&status, sizeof(char), 1, file);
    if (status == '0') {  // Verifica a consistencia do arquivo
        printf("Falha no processamento do arquivo.\n");
        fclose(file);
        return NULL;
    }
  return file;
}

/// @brief            Abre o arquivo, e efetua a verificação se foi possível abrir e a respeito da integridade.
/// @param nomeArqBin Nome do arquivo a ser manipulado
/// @return           Retorna o ponteiro do arquivo
FILE *abre_verifica_rbplus(char *nomeArqBin)
{
    FILE *file = fopen(nomeArqBin, "rb+");
    if (file == NULL){
        printf("Falha no processamento do arquivo.\n");
        return NULL;
    }

    char status;
    fread(&status, sizeof(char), 1, file);
    if (status == '0'){
        printf("Falha no processamento do arquivo.\n");
        fclose(file);
        return NULL;
    }
  return file;
}

/// @brief          Faz a leitura dos campos variados
/// @param n        Número de campos
/// @param nomes    Nome do campo
/// @param valores  Valores contidos no campo 
void leitura_campos(int n, char nomes[][50], char valores[][100]) 
{
    for (int j = 0; j < n; j++)
    {
        scanf("%s", nomes[j]);
        
        // Lógica de distinção entre string com aspas e campos comuns
        if (strcmp(nomes[j], "nomeEstacao") == 0 || strcmp(nomes[j], "nomeLinha") == 0)
            ScanQuoteString(valores[j]);
        else
            scanf("%s", valores[j]); 
    }
}