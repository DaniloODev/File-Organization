#include "cabecalho.h"
#include <stdio.h>
#include <stdlib.h>

struct _cabecalho
{
    char status;            // 0 inconsistente, 1 consistente
    int topo;               // Byte offset do registro removido ou -1
    int proxRRN;            // Próximo RRN disponível
    int nroEstacoes;        // Qtd de estações únicas
    int nroParesEstacao;    // Qtd de pares únicos
};

struct _cabecalho_arvb
{
    char status;
    int noRaiz;
    int topo;
    int proxRRN;
    int nroNos;
};

/// @brief  Aloca memória para o cabeçalho e inicializa a estrutura com valores padrão.
/// @return Ponteiro para o cabeçalho.
cabecalho* inicializaCabecalho()
{
    cabecalho *c = malloc (sizeof(cabecalho)); // Aloca memória para o cabeçalho.
    if (c == NULL){
        fprintf(stderr, "Erro ao alocar memória para o cabeçalho.\n");
        exit(EXIT_FAILURE);
    }
    // Inicializa os campos com valores padrão.
    c->status = '0';        // 1 Byte
    c->topo = -1;           // 4 Bytes
    c->proxRRN = 0;         // 4 Bytes
    c->nroEstacoes = 0;     // 4 Bytes
    c->nroParesEstacao = 0; // 4 Bytes
    return c;
}

/// @brief Desaloca a memória com um simples free(), já que foi alocado apenas uma simples struct.
void finalizaCabecalho(cabecalho *c){
    free(c);
}

/// @brief              Atualiza os campos do cabeçalho para que sejam gravados futurarmente
/// @param c            Ponteiro do cabeçalho a ser atualizado.
/// @param status       Valor do status, consistente ou inconsistente.
/// @param topo         RRN do último excluído
/// @param proxRRN      RRN da posição em branco (após o ultimo registro do arquivo)
/// @param nroEstacoes  Número de estações ativas 
/// @param nroPares     Número de pares de estações
void atualizaCabecalho(cabecalho *c, char status, int topo, int proxRRN, int nroEstacoes, int nroPares)
{   
    // Atualiza campos com os valores passados
    c->status = status;
    c->topo = topo;
    c->proxRRN = proxRRN;
    c->nroEstacoes = nroEstacoes;
    c->nroParesEstacao = nroPares;
}

/// @brief          Responsável por gravar o cabeçalho no arquivo, de forma que ocupe exatamente 17 Bytes.
/// @param binFile  Arquivo préviamente aberto.
/// @param c        Ponteiro para o cabeçalho previamente atualizado.
void gravaCabecalho(FILE *binFile, cabecalho *c)
{
    // Pega o inicio do arquivo, no byte 0
    fseek(binFile, 0, SEEK_SET);

    // Grava os campos do cabeçalho
    fwrite(&c->status, sizeof(char), 1, binFile);
    fwrite(&c->topo, sizeof(int), 1, binFile);
    fwrite(&c->proxRRN, sizeof(int), 1, binFile);
    fwrite(&c->nroEstacoes, sizeof(int), 1, binFile);
    fwrite(&c->nroParesEstacao, sizeof(int), 1, binFile);
}

//  Daqui pra frente precisa comentar direitinho.
// Funções do cabeçalho da árvore B: 
cabecalho_arvb* inicializaCabecalhoArvb()
{
    cabecalho_arvb *c_arvb = malloc (sizeof(cabecalho_arvb)); // Aloca memória para o cabeçalho da árvore B.
    if (c_arvb == NULL){
        printf("Erro ao alocar memória para o cabeçalho.\n");
        return NULL;
    }
    // Inicializa os campos com valores padrão.
    c_arvb->status = '0';        // 1 Byte
    c_arvb->noRaiz = -1;          // 4 Bytes
    c_arvb->topo = -1;           // 4 Bytes
    c_arvb->proxRRN = 0;         // 4 Bytes
    c_arvb->nroNos = 0;     // 4 Bytes
    return c_arvb;
}


void finalizaCabecalhoArvb(cabecalho_arvb *c_arvb){
    free(c_arvb);
}

void atualizaCabecalhoArvb(cabecalho_arvb *c_arvb, char status, int noRaiz, int topo, int proxRRN, int nroNos){
    c_arvb->status = status;
    c_arvb->noRaiz = noRaiz;
    c_arvb->topo = topo;
    c_arvb->proxRRN = proxRRN;
    c_arvb->nroNos = nroNos;
}
void gravaCabecalhoArvb(FILE *binFile, cabecalho_arvb *c_arvb)
{
    // Pega o inicio do arquivo, no byte 0
    fseek(binFile, 0, SEEK_SET);

    // Grava os campos do cabeçalho
    fwrite(&c_arvb->status, sizeof(char), 1, binFile);
    fwrite(&c_arvb->noRaiz, sizeof(int), 1, binFile);
    fwrite(&c_arvb->topo, sizeof(int), 1, binFile);
    fwrite(&c_arvb->proxRRN, sizeof(int), 1, binFile);
    fwrite(&c_arvb->nroNos, sizeof(int), 1, binFile);

}
