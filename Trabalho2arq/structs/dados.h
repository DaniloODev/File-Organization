#ifndef DADOS_H
#define DADOS_H

#include <stdio.h>

typedef struct _dados dados;

// Inicialização da struct dados
dados *dados_cria();
// Desaloca memória previamente alocada
void dados_apaga(dados *d);

// Pega valores individualmente de campos dos dados dos registros
char dados_get_removido(dados *d);
int dados_get_proxRemovido(dados *d);
int dados_get_codEstacao(dados *d);
int dados_get_codLinha(dados *d);
int dados_get_codProxEstacao(dados *d);
int dados_get_distProxEstacao(dados *d);
int dados_get_codLinhaIntegra(dados *d);
int dados_get_codEstIntegra(dados *d);
char *dados_get_nomeEstacao(dados *d);
char *dados_get_nomeLinha(dados *d);

// Seta  valores indviduais nos refistros
void dados_set_campos_fixos(dados *d, char removido, int proxRemovido, int codEstacao, int codLinha, int codProxEstacao, int distProxEstacao, int codLinhaIntegra, int codEstIntegra);
void dados_set_nomeEstacao(dados *d, char *nome);
void dados_set_nomeLinha(dados *d, char *nome);

// Leitura e grravação de dados no arquivo
dados *dados_le_binario(FILE *binFile);
void dados_grava_binario(dados *d, FILE *binFile);

// Atualiza campos e verifica se deu Mathc
void dados_atualiza_campos(dados *d, int p_campos, char nomesCamposAtualiza[][50], char valoresAtualiza[][100]);
int valida_registro(dados *d, int m_campos, char nomesCampos[][50], char valoresBusca[][100]);

#endif
