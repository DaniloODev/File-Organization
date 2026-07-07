#include <string.h>
#include "auxiliares.h"
#include "../fornecidas/fornecidas.h"

/// @brief          Serve para ler o arquivo palavra por palavra
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

/// @brief          Abre e verifica a integridade do arquivo
/// @param nomeArq  Nome do arquivo a ser lido
/// @param modo     Modo de leitura ex: r, rb, rb+, etc...
/// @return         Retorna um ponteiro para o arquivo
FILE *abre_verifica(char *nomeArq, char *modo)
{
  FILE *file = fopen(nomeArq, modo);
    if (file == NULL) {
        printf("Falha no processamento do arquivo.\n");
        return NULL;
    }

    if (strchr(modo, 'r') && strchr(modo, 'b')) {   // Caso necessite checar o cabeçalho
        char status;                                // Ocorre somente no rb, rb+
        if (fread(&status, sizeof(char), 1, file) != 1 || status == '0') {
            printf("Falha no processamento do arquivo.\n");
            fclose(file);
            return NULL;
        }
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