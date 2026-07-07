#include <stdio.h>
#include <stdlib.h>
#include "cabecalho_arvb.h"
#include "arvore_b.h"
#include "../aux/auxiliares.h"
#include "../structs/dados.h"
#include "../fornecidas/fornecidas.h"
#include "create_index.h"

/// @brief Cria um índice para uma árvore-B a partir de um arquivo de dados.
/// @param nome_arq_dados  Nome do arquivo de dados.
/// @param nome_arq_indice Nome do arquivo de índice.
void criar_indice_arvore_b(char *nome_arq_dados, char *nome_arq_indice)
{
    FILE *arq_dados = abre_verifica(nome_arq_dados, "rb");         // Abre e verifica o arquivo de dados
    if (arq_dados == NULL) return;

    FILE *arq_indice = abre_verifica(nome_arq_indice, "wb+");   // Cria e verifica o arquivo de index
    if (arq_indice == NULL) { fclose(arq_dados); return; }

    // Inicializa e escreve cabeçalho
    cabecalho_arvb *cab = inicializaCabecalhoArvb();
    escreveCabecalhoArvb(arq_indice, cab);

    // Salta o cabeçalho dos dados para iniciar a leitura
    fseek(arq_dados, 17, SEEK_SET);

    long offset_atual = 17;

    // leitura
    while (1) {
        // Lê os registros
        dados *d = dados_le_binario(arq_dados);
        
        // Se retornar NULL, chegou no fim do arquivo
        if (d == NULL) {
            break; 
        }

        // Verifica se o registro não está removido
        if (dados_get_removido(d) == '0') { 
            // Passa o offset para arvore
            insere_arvore_b(arq_indice, cab, dados_get_codEstacao(d), offset_atual);
        }

        // Libera memória previamente alocadda
        dados_apaga(d);

        offset_atual += 80;
    }

    // Atualiza e graga cabeçalho
    setStatusArvb(cab, '1'); 
    escreveCabecalhoArvb(arq_indice, cab);

    // Desaloca memória e fecha arquivos
    finalizaCabecalhoArvb(cab);
    fclose(arq_dados);
    fclose(arq_indice);
    BinarioNaTela(nome_arq_indice);
}