#include <stdio.h>
#include <stdlib.h>
#include "cabecalho_arvb.h"
#include "arvore_b.h"
#include "../aux/auxiliares.h"
#include "../structs/dados.h"
#include "../fornecidas/fornecidas.h"

#define TAM_CAB_DADOS 17 
#define TAM_REGISTRO 80 // Definição baseada no tamanho fixo do seu registro

void criar_indice_arvore_b(char *nome_arq_dados, char *nome_arq_indice)
{
    FILE *arq_dados = abre_verifica_rb(nome_arq_dados);
    if (arq_dados == NULL) return;

    FILE *arq_indice = abre_verifica_wbplus(nome_arq_indice);
    if (arq_indice == NULL) {
        fclose(arq_dados);
        return;
    }

    // Inicializa e escreve cabeçalho provisório da árvore com status '0'
    cabecalho_arvb *cab = inicializaCabecalhoArvb();
    escreveCabecalhoArvb(arq_indice, cab);

    // Salta o cabeçalho dos dados para iniciar a leitura sequencial
    fseek(arq_dados, TAM_CAB_DADOS, SEEK_SET);

    // --- CÁLCULO MANUAL DE OFFSET ---
    // O primeiro registro começa exatamente onde termina o cabeçalho (byte 17)
    long offset_atual = TAM_CAB_DADOS;

    // Loop de leitura sequencial dos dados
    while (1) {
        // Usa a sua função para ler o registro estruturado
        dados *d = dados_le_binario(arq_dados);
        
        // Se retornar NULL, atingiu o fim do arquivo (EOF)
        if (d == NULL) {
            break; 
        }

        // Verifica se o registro está ativo
        if (dados_get_removido(d) == '0') { 
            // Passa o offset calculado manualmente para a Árvore-B
            insere_arvore_b(arq_indice, cab, dados_get_codEstacao(d), offset_atual);
        }

        // Libera a memória alocada por dados_le_binario
        dados_apaga(d);

        // --- ATUALIZAÇÃO DO OFFSET ---
        // Como o registro ocupa sempre 80 bytes, o próximo começará 80 bytes adiante
        offset_atual += TAM_REGISTRO;
    }

    // Finalização e consolidação da escrita segura do índice
    setStatusArvb(cab, '1'); 
    escreveCabecalhoArvb(arq_indice, cab);

    // Liberação de ponteiros e descritores de arquivos
    finalizaCabecalhoArvb(cab);
    fclose(arq_dados);
    fclose(arq_indice);
    BinarioNaTela(nome_arq_indice);
}