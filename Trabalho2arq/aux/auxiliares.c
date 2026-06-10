#include "auxiliares.h"
#include "../fornecidas/fornecidas.h"
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

FILE *abre_verifica_r(char *nomeArq)
{
  FILE *file = fopen(nomeArq, "r");
    if (file == NULL) {
        printf("Falha no processamento do arquivo.\n");
        return NULL;
    }
  return file;
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

FILE *abre_verifica_wb(char *nomeArqBin)
{
    FILE *file = fopen(nomeArqBin, "wb");
    if (file == NULL){
        printf("Falha no processamento do arquivo.\n");
        return NULL;
    }
  return file;
}

FILE *abre_verifica_wbplus(char *nomeArqBin)
{
    FILE *file = fopen(nomeArqBin, "wb+");
    if (file == NULL){
        printf("Falha no processamento do arquivo.\n");
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

/// @brief      Recebe a estação por ponteiro e printa os seus dados.
/// @param trem Ponteiro para a estação encapsulada.
void printa_estacao(dados *trem)
{
    // Extrai os campos necessários
    int codEstacao = dados_get_codEstacao(trem);
    char *nomeEstacao = dados_get_nomeEstacao(trem);
    int codLinha = dados_get_codLinha(trem);
    char *nomeLinha = dados_get_nomeLinha(trem);
    int codProxEstacao = dados_get_codProxEstacao(trem);
    int distProxEstacao = dados_get_distProxEstacao(trem);
    int codLinhaIntegra = dados_get_codLinhaIntegra(trem);
    int codEstIntegra = dados_get_codEstIntegra(trem);

    printf("%d %s ", codEstacao, nomeEstacao ? nomeEstacao : "NULO");
    
    if (codLinha != -1) printf("%d ", codLinha); else printf("NULO ");
    if (nomeLinha != NULL) printf("%s ", nomeLinha); else printf("NULO ");
    if (codProxEstacao != -1) printf("%d ", codProxEstacao); else printf("NULO ");
    if (distProxEstacao != -1) printf("%d ", distProxEstacao); else printf("NULO ");
    if (codLinhaIntegra != -1) printf("%d ", codLinhaIntegra); else printf("NULO ");
    if (codEstIntegra != -1) printf("%d\n", codEstIntegra); else printf("NULO\n");
}