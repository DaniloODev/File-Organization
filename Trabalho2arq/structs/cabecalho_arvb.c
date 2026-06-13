#include <stdlib.h>
#include "cabecalho_arvb.h"

struct _cabecalho_arvb {
    char status;   // 1 Byte
    int noRaiz;    // 4 Bytes
    int topo;      // 4 Bytes
    int proxRRN;   // 4 Bytes
    int nroNos;    // 4 Bytes
};

/// @brief  Inicializa o cabeçalho com valores padrão
/// @return Retorna o ponteiro do cabeçalho
cabecalho_arvb* inicializaCabecalhoArvb() {
    cabecalho_arvb *c_arvb = malloc(sizeof(cabecalho_arvb));
    if (c_arvb == NULL) return NULL;
    
    // Setando os valores padrão
    c_arvb->status = '0';
    c_arvb->noRaiz = -1;
    c_arvb->topo = -1;
    c_arvb->proxRRN = 0;
    c_arvb->nroNos = 0;
    return c_arvb;
}

/// @brief          Desaloca a memória previamente alocada para o cabeçalho
/// @param c_arvb   Ponteiro do cabeçalho a ser desalocado
void finalizaCabecalhoArvb(cabecalho_arvb *c_arvb) {
    if (c_arvb != NULL) free(c_arvb);
}

/// @brief          Atualiza o cabeçalho com os valores passados
/// @param c_arvb   Ponteiro do cabeçalho
/// @param status   Status (ativo ou não)
/// @param noRaiz   Nó raiz
/// @param topo     RRN do ultimo removido
/// @param proxRRN  Próximo lugar a inserir quando não tiver topo
/// @param nroNos   Total de nós da arvore 
void atualizaCabecalhoArvb(cabecalho_arvb *c_arvb, char status, int noRaiz, int topo, int proxRRN, int nroNos) {
    if (c_arvb == NULL) return;
    c_arvb->status = status;
    c_arvb->noRaiz = noRaiz;
    c_arvb->topo = topo;
    c_arvb->proxRRN = proxRRN;
    c_arvb->nroNos = nroNos;
}

/// @brief          Grava o cabeçalho no arquivo de índice
/// @param fp       Ponterio para o arquivo
/// @param c_arvb   Ponteiro para o cabeçalho
void escreveCabecalhoArvb(FILE *fp, cabecalho_arvb *c_arvb) {
    if (fp == NULL || c_arvb == NULL) return;
    fseek(fp, 0, SEEK_SET);
    fwrite(&c_arvb->status, sizeof(char), 1, fp);
    fwrite(&c_arvb->noRaiz, sizeof(int), 1, fp);
    fwrite(&c_arvb->topo, sizeof(int), 1, fp);
    fwrite(&c_arvb->proxRRN, sizeof(int), 1, fp);
    fwrite(&c_arvb->nroNos, sizeof(int), 1, fp);
}

/// @brief      Lê o cabeçaho do arquivo e retorna o objeto Cabeçalho
/// @param fp   Ponteiro para o arquivo da árvore
/// @return     Ponteiro para o cabecalho
cabecalho_arvb* leCabecalhoArvb(FILE *fp) {
    if (fp == NULL) return NULL;
    cabecalho_arvb *c_arvb = malloc(sizeof(cabecalho_arvb));
    if (c_arvb == NULL) return NULL;

    fseek(fp, 0, SEEK_SET);
    fread(&c_arvb->status, sizeof(char), 1, fp);
    fread(&c_arvb->noRaiz, sizeof(int), 1, fp);
    fread(&c_arvb->topo, sizeof(int), 1, fp);
    fread(&c_arvb->proxRRN, sizeof(int), 1, fp);
    fread(&c_arvb->nroNos, sizeof(int), 1, fp);
    return c_arvb;
}

// Pegar informações do cabeçalho da arvore b, de forma individual
char getStatusArvb(cabecalho_arvb *c_arvb) { return c_arvb->status; }
int getNoRaizArvb(cabecalho_arvb *c_arvb) { return c_arvb->noRaiz; }
int getTopoArvb(cabecalho_arvb *c_arvb) { return c_arvb->topo; }
int getProxRRNArvb(cabecalho_arvb *c_arvb) { return c_arvb->proxRRN; }
int getNroNosArvb(cabecalho_arvb *c_arvb) { return c_arvb->nroNos; }

// Adicionar coisas ao cabeçalho da arvore b, de forma individual
void setNoRaizArvb(cabecalho_arvb *c_arvb, int noRaiz) { c_arvb->noRaiz = noRaiz; }
void setProxRRNArvb(cabecalho_arvb *c_arvb, int proxRRN) { c_arvb->proxRRN = proxRRN; }
void setNroNosArvb(cabecalho_arvb *c_arvb, int nroNos) { c_arvb->nroNos = nroNos; }
void setStatusArvb(cabecalho_arvb *c_arvb, char status) { c_arvb->status = status; }
void setTopoArvb(cabecalho_arvb *c_arvb, int topo) { c_arvb->topo = topo; }