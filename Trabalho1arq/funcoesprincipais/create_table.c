#include "create_table.h"
#include "cabecalho.h"
#include "dados.h"
#include "fornecidas.h"
#include "auxiliares.h"
#include "calcula.h"
#include <stdlib.h>
#include <string.h>

/// @brief          Dado um arquivo, ele faz a leitura e transforma em binário
/// @param nomeArq  Nome do arquivo CSV a ser lido.
/// @param arqBin   Nome do arquivo binário que será gerado.
void CREATE_TABLE(char *nomeArq, char *arqBin)
{
  FILE *file = fopen(nomeArq, "r"); //Abre o arquivo para leitura.

  if (file == NULL){ // Se houver algum erro, ja avisa o usuario
    printf("Falha no processamento do arquivo.\n");
    return;
  }

  FILE *binFile = fopen(arqBin, "wb"); // Abre o arquivo binário para escrita
    if (binFile == NULL){              // Se houver algum erro, retorna nulo
        fclose(file);
        return;
    }
  //Pula o cabeçalho:
  char ignore[1024];
  fgets(ignore, sizeof(ignore), file); // Lê a primeira linha inteira e não faz nada com ela.

  cabecalho *cab; 
  cab = inicializaCabecalho();  // Inicializa o cabeçalho
  gravaCabecalho(binFile, cab); // Grava o cabeçalho no arquivo binário (com status '0' e dados iniciais)
  
  dados *trem = malloc(sizeof(dados));  // Aloca memória para grava pelo menos 1 registro na memória.
  char buffer[100];                     // buffer para leitura dos campos do arquivo csv. 

  int i = 0; // Variável que contabiliza a quantidade de registros lidos.
  while(leitura(file, buffer))
  { //Enquanto ouver registros para ler:
    
    if (calcTam(buffer) == 0) break;  // Trava de segurança para linhas vazias no fim do CSV

    trem = realloc(trem, (i + 2) * sizeof(dados)); //Aloca memória para leitura de (i + 1) + 1 registros
    
    trem[i].codEstacao = atoi(buffer);  // O buffer já tem o primeiro campo. Vamos converter de texto para int.

    // Lemos do arquivo direto para a variável da struct.
    leitura(file, buffer);                    // É feita a leitura utilizando o buffer para evitar a leitura de lixo na memória.
    trem[i].tamNomeEstacao = calcTam(buffer); // Calcula o tamanho do nome da estação e salva.
    trem[i].nomeEstacao = malloc((trem[i].tamNomeEstacao + 1) * sizeof(char)); // Aloca memória para salvar o nome da estação.
    strcpy(trem[i].nomeEstacao, buffer);      //Salva o nome da estação copiando-o do buffer.

    leitura(file, buffer);  // Lemos para o buffer, testamos se é nulo e convertemos.
    trem[i].codLinha = (calcTam(buffer) > 0) ? atoi(buffer) : -1;

    leitura(file, buffer);                  // Lemos do arquivo direto para a struct.
    trem[i].tamNomeLinha = calcTam(buffer); // Calcula o tamanho do nome da linha.
    if(trem[i].tamNomeLinha > 0) {
      trem[i].nomeLinha = malloc((trem[i].tamNomeLinha + 1) * sizeof(char)); // Aloca memória para o nome da linha.
      strcpy(trem[i].nomeLinha, buffer);    // Salva o nome da linha copiando-o do buffer.
    } else {  // Se for nulo:
      trem[i].tamNomeLinha = 0; // O tamanho do nome da linha recebe 0, conforme o PDF.
      trem[i].nomeLinha = NULL; // O ponteiro da string não aponta para nada e nem aloca memória.
    }

    // Dos campos 5 ao 8 a forma de leitura é mesma. Lê-se o buffer e converte-se para o tipo apropriado.
    leitura(file, buffer);  // codProxEstacao
    trem[i].codProxEstacao = (calcTam(buffer) > 0) ? atoi(buffer) : -1;

    leitura(file, buffer);  // distProxEstacao
    trem[i].distProxEstacao = (calcTam(buffer) > 0) ? atoi(buffer) : -1;

    leitura(file, buffer);  // 7. codLinhaIntegra
    trem[i].codLinhaIntegra = (calcTam(buffer) > 0) ? atoi(buffer) : -1;

    leitura(file, buffer);  // 8. codEstIntegra
    trem[i].codEstIntegra = (calcTam(buffer) > 0) ? atoi(buffer) : -1;

    // Variáveis de controle de remoção
    trem[i].removido = '0';
    trem[i].proxRemovido = -1;

    // Gravando campo a campo do registro
    fwrite(&trem[i].removido, sizeof(char), 1, binFile); 
    fwrite(&trem[i].proxRemovido, sizeof(int), 1, binFile);
    fwrite(&trem[i].codEstacao, sizeof(int), 1, binFile);
    fwrite(&trem[i].codLinha, sizeof(int), 1, binFile);
    fwrite(&trem[i].codProxEstacao, sizeof(int), 1, binFile);
    fwrite(&trem[i].distProxEstacao, sizeof(int), 1, binFile);
    fwrite(&trem[i].codLinhaIntegra, sizeof(int), 1, binFile);
    fwrite(&trem[i].codEstIntegra, sizeof(int), 1, binFile);
    fwrite(&trem[i].tamNomeEstacao, sizeof(int), 1, binFile);
    fwrite(trem[i].nomeEstacao, sizeof(char), trem[i].tamNomeEstacao, binFile);
    fwrite(&trem[i].tamNomeLinha, sizeof(int), 1, binFile);

    // Só grava o nomeLinha se não for vazio. Se for nulo, tamanho = 0, preenche com lixo.
    if(trem[i].tamNomeLinha > 0) {
        fwrite(trem[i].nomeLinha, sizeof(char), trem[i].tamNomeLinha, binFile);
    }

    // Escrita de lixo "$$$"
    int bytesEscritos = 1 + 4 + (4 * 6) + 4 + trem[i].tamNomeEstacao + 4 + trem[i].tamNomeLinha;
    char lixo = '$';
    for (int k = 0; k < (80 - bytesEscritos); k++)
        fwrite(&lixo, sizeof(char), 1, binFile);
    
    i++;
  }
  
  atualizaCabecalho(cab, '1', -1, i, calc_nroEstacoes(trem, i), calc_nroPares(trem, i));  // Atualiza com valores corretos
  gravaCabecalho(binFile, cab); // Grava o cabeçalho atualizado no arquivo
  finalizaCabecalho(cab);       // Desaloca a memória do cabeçalho

  fclose(file);     // Fecha o *.alguma coisa, no nosso caso CSV
  fclose(binFile);  // Fecha o *.bin
  for(int j = 0; j < i; j++) {
    if(trem[j].nomeEstacao != NULL) free(trem[j].nomeEstacao);  // Libera memória da string do nome da estação
    if(trem[j].nomeLinha != NULL) free(trem[j].nomeLinha);      // Libera memória da string do nome da linha
  }

  free(trem);             // Libera a memória da struct
  BinarioNaTela(arqBin);  // Imprime os binários
}