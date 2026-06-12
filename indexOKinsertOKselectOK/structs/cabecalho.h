#include <stdio.h>
#ifndef CABECALHO_H
#define CABECALHO_H

typedef struct _cabecalho cabecalho;

cabecalho* inicializaCabecalho();                   // Funções para inicializar os cabeçalhos com valores padrão
void finalizaCabecalho(cabecalho *c);               // Funções para liberar a memória alocada para os cabeçalhos
void gravaCabecalho(FILE *binFile, cabecalho *c);   // Funções para gravar os cabeçalhos
cabecalho *leCabecalhoDados(FILE *binFile);         // Função para ler o cabeçalho

// Funções para atualizar dados dos cabeçalhos antes de gravar
void atualizaCabecalho(cabecalho *c, char status, int topo,int proxRRN, int nroEstacoes, int nroPares);

// Retorna dados especificos da struct
char getStatusCabecalho(const cabecalho *c);
int getTopoCabecalho(const cabecalho *c);
int getProxRRNCabecalho(const cabecalho *c);
int getNroEstacoesCabecalho(const cabecalho *c);
int getNroParesEstacaoCabecalho(const cabecalho *c);

// Seta dados específicos da struct
void setStatus(cabecalho *c, char status);
void setTopo(cabecalho *c, int topo);
void setProxRRN(cabecalho *c, int proxRRN);
void setNroEstacoes(cabecalho *c, int nroEstacoes);
void setNroParesEstacao(cabecalho *c, int nroParesEstacao);

#endif
