#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dados.h"

struct _dados {
    char removido;          // '0' para ativo, '1' para removido
    int proxRemovido;       // RRN do próximo registro removido ou -1 se não houver
    int codEstacao;         // Código da estação
    int codLinha;           // Código da linha
    int codProxEstacao;     // Código da próxima estação
    int distProxEstacao;    // Distância para a próxima estação
    int codLinhaIntegra;    // Código da linha de integração
    int codEstIntegra;      // Código da estação de integração
    int tamNomeEstacao;     // Tamanho do nome da estação (0 se nulo)
    char *nomeEstacao;      // Nome da estação (NULL se nulo)
    int tamNomeLinha;       // Tamanho do nome da linha (0 se nulo)
    char *nomeLinha;        // Nome da linha (NULL se nulo)
};

/// @brief  Aloca memória para os dados e inicializa a estrutura com valores padrão.
/// @return Ponteiro para os dados.
dados *dados_cria() {
    dados *d = (dados *)malloc(sizeof(dados));
    if (!d) return NULL;
    d->removido = '0';
    d->proxRemovido = -1;
    d->codEstacao = -1;
    d->codLinha = -1;
    d->codProxEstacao = -1;
    d->distProxEstacao = -1;
    d->codLinhaIntegra = -1;
    d->codEstIntegra = -1;
    d->tamNomeEstacao = 0;
    d->nomeEstacao = NULL;
    d->tamNomeLinha = 0;
    d->nomeLinha = NULL;
    return d;
}

/// @brief  Libera a memória alocada para os dados e suas strings internas.
/// @param  d Ponteiro para os dados.
void dados_apaga(dados *d) {
    if (d == NULL) return;
    if (d->nomeEstacao != NULL) free(d->nomeEstacao);
    if (d->nomeLinha != NULL) free(d->nomeLinha);
    free(d);
}

// Pega informações individualmente dos campos da struc dados
char dados_get_removido(dados *d) { return d->removido; }
int dados_get_proxRemovido(dados *d) { return d->proxRemovido; }
int dados_get_codEstacao(dados *d) { return d->codEstacao; }
int dados_get_codLinha(dados *d) { return d->codLinha; }
int dados_get_codProxEstacao(dados *d) { return d->codProxEstacao; }
int dados_get_distProxEstacao(dados *d) { return d->distProxEstacao; }
int dados_get_codLinhaIntegra(dados *d) { return d->codLinhaIntegra; }
int dados_get_codEstIntegra(dados *d) { return d->codEstIntegra; }
char *dados_get_nomeEstacao(dados *d) { return d->nomeEstacao; }
char *dados_get_nomeLinha(dados *d) { return d->nomeLinha; }

/// @brief                  Seta os campos fixos do objeto
/// @param d                Ponteiro do objeto
/// @param removido         status de removido
/// @param proxRemovido     pŕoximo removido
/// @param codEstacao       Código da estação atual
/// @param codLinha         Código da linha que a estação pertence
/// @param codProxEstacao   Código da próxima estação
/// @param distProxEstacao  Distância
/// @param codLinhaIntegra  Código da linha integra
/// @param codEstIntegra    Código estacão integra
void dados_set_campos_fixos(dados *d, char removido, int proxRemovido, int codEstacao, int codLinha, int codProxEstacao, int distProxEstacao, int codLinhaIntegra, int codEstIntegra) {
    if (!d) return;
    d->removido = removido;
    d->proxRemovido = proxRemovido;
    d->codEstacao = codEstacao;
    d->codLinha = codLinha;
    d->codProxEstacao = codProxEstacao;
    d->distProxEstacao = distProxEstacao;
    d->codLinhaIntegra = codLinhaIntegra;
    d->codEstIntegra = codEstIntegra;
}

/// @brief      Seta o nome da Estação
/// @param d    Ponteiro do objeto trem
/// @param nome Nome da Estação
void dados_set_nomeEstacao(dados *d, char *nome) {
    if (!d) return;
    if (d->nomeEstacao) free(d->nomeEstacao);
    
    if (nome && strlen(nome) > 0) {
        d->tamNomeEstacao = strlen(nome);
        d->nomeEstacao = malloc((d->tamNomeEstacao + 1) * sizeof(char));
        strcpy(d->nomeEstacao, nome);
    } else {
        d->tamNomeEstacao = 0;
        d->nomeEstacao = NULL;
    }
}

/// @brief      Seta o noe da linha
/// @param d    Ponteiro para o objeto trem
/// @param nome Nome da linha
void dados_set_nomeLinha(dados *d, char *nome) {
    if (!d) return;
    if (d->nomeLinha) free(d->nomeLinha);
    
    if (nome && strlen(nome) > 0) {
        d->tamNomeLinha = strlen(nome);
        d->nomeLinha = malloc((d->tamNomeLinha + 1) * sizeof(char));
        strcpy(d->nomeLinha, nome);
    } else {
        d->tamNomeLinha = 0;
        d->nomeLinha = NULL;
    }
}

/// @brief          Grava os registros no arquivo de dados
/// @param d        Ponteiro do objeto trem
/// @param binFile  Ponteiro para o arquivo que será gravado
void dados_grava_binario(dados *d, FILE *binFile) {
    if (!d || !binFile) return;

    fwrite(&d->removido, sizeof(char), 1, binFile); 
    fwrite(&d->proxRemovido, sizeof(int), 1, binFile);
    fwrite(&d->codEstacao, sizeof(int), 1, binFile);
    fwrite(&d->codLinha, sizeof(int), 1, binFile);
    fwrite(&d->codProxEstacao, sizeof(int), 1, binFile);
    fwrite(&d->distProxEstacao, sizeof(int), 1, binFile);
    fwrite(&d->codLinhaIntegra, sizeof(int), 1, binFile);
    fwrite(&d->codEstIntegra, sizeof(int), 1, binFile);
    
    fwrite(&d->tamNomeEstacao, sizeof(int), 1, binFile);
    if(d->tamNomeEstacao > 0) {
        fwrite(d->nomeEstacao, sizeof(char), d->tamNomeEstacao, binFile);
    }
    
    fwrite(&d->tamNomeLinha, sizeof(int), 1, binFile);
    if(d->tamNomeLinha > 0) {
        fwrite(d->nomeLinha, sizeof(char), d->tamNomeLinha, binFile);
    }

    // Completa o registro com lixo $ até dar 80 bytes
    int bytesEscritos = 1 + 4 + (4 * 6) + 4 + d->tamNomeEstacao + 4 + d->tamNomeLinha;
    char lixo = '$';
    for (int k = 0; k < (80 - bytesEscritos); k++) {
        fwrite(&lixo, sizeof(char), 1, binFile);
    }
}

/// @brief          Lê o arquivo de dados
/// @param binFile  Ponteiro para o arquivo de dados
/// @return         O Ponteiro da struct contendo os dados lidos
dados *dados_le_binario(FILE *binFile) {
    if (!binFile) return NULL;

    dados *d = dados_cria();
    if (!d) return NULL;

    // Se falhar ao ler o status de removido, chegou ao fim do arquivo
    if (fread(&d->removido, sizeof(char), 1, binFile) != 1) {
        dados_apaga(d);
        return NULL;
    }

    fread(&d->proxRemovido, sizeof(int), 1, binFile);
    fread(&d->codEstacao, sizeof(int), 1, binFile);
    fread(&d->codLinha, sizeof(int), 1, binFile);
    fread(&d->codProxEstacao, sizeof(int), 1, binFile);
    fread(&d->distProxEstacao, sizeof(int), 1, binFile);
    fread(&d->codLinhaIntegra, sizeof(int), 1, binFile);
    fread(&d->codEstIntegra, sizeof(int), 1, binFile);
    
    fread(&d->tamNomeEstacao, sizeof(int), 1, binFile);
    if (d->tamNomeEstacao > 0 && d->tamNomeEstacao <= 80) {
        d->nomeEstacao = malloc(d->tamNomeEstacao + 1);
        fread(d->nomeEstacao, sizeof(char), d->tamNomeEstacao, binFile);
        d->nomeEstacao[d->tamNomeEstacao] = '\0';
    }

    fread(&d->tamNomeLinha, sizeof(int), 1, binFile);
    if (d->tamNomeLinha > 0 && d->tamNomeLinha <= 80) {
        d->nomeLinha = malloc(d->tamNomeLinha + 1);
        fread(d->nomeLinha, sizeof(char), d->tamNomeLinha, binFile);
        d->nomeLinha[d->tamNomeLinha] = '\0';
    }

    // Pula o lixo restante para posicionar o ponteiro no início do próximo registro
    int bytesLidos = 1 + 4 + (4 * 6) + 4 + d->tamNomeEstacao + 4 + d->tamNomeLinha;
    fseek(binFile, 80 - bytesLidos, SEEK_CUR);

    return d;
}

/// @brief                      Atualiza os campos do objeto
/// @param d                    Ponteiro para o objeto
/// @param p_campos             Número de campos que serão atualizados
/// @param nomesCamposAtualiza  Nomes dos campos a serem atualizados
/// @param valoresAtualiza      Valores a serem atualizados
void dados_atualiza_campos(dados *d, int p_campos, char nomesCamposAtualiza[][50], char valoresAtualiza[][100]) {
    if (!d) return;

    for (int k = 0; k < p_campos; k++) {
        int valorInt_Novo = (strcmp(valoresAtualiza[k], "NULO") == 0) ? -1 : atoi(valoresAtualiza[k]);

        if (strcmp(nomesCamposAtualiza[k], "codEstacao") == 0)
            d->codEstacao = valorInt_Novo;
        else if (strcmp(nomesCamposAtualiza[k], "codLinha") == 0)
            d->codLinha = valorInt_Novo;
        else if (strcmp(nomesCamposAtualiza[k], "codProxEstacao") == 0)
            d->codProxEstacao = valorInt_Novo;
        else if (strcmp(nomesCamposAtualiza[k], "distProxEstacao") == 0)
            d->distProxEstacao = valorInt_Novo;
        else if (strcmp(nomesCamposAtualiza[k], "codLinhaIntegra") == 0)
            d->codLinhaIntegra = valorInt_Novo;
        else if (strcmp(nomesCamposAtualiza[k], "codEstIntegra") == 0)
            d->codEstIntegra = valorInt_Novo;
        else if (strcmp(nomesCamposAtualiza[k], "nomeEstacao") == 0) {
            dados_set_nomeEstacao(d, valoresAtualiza[k]);
        }
        else if (strcmp(nomesCamposAtualiza[k], "nomeLinha") == 0) {
            dados_set_nomeLinha(d, valoresAtualiza[k]);
        }
    }
}

/// @brief              Verifica se deu match do registro buscado
/// @param d            Ponteiro para o objeto trem
/// @param m_campos     M campos a serem validados
/// @param nomesCampos  Nome dos campos
/// @param valoresBusca Valores de busca 
/// @return             Retorna 0 se não deu Match e 1 se deu Match
int valida_registro(dados *d, int m_campos, char nomesCampos[][50], char valoresBusca[][100]) {
    for (int j = 0; j < m_campos; j++) {
        int valorInt_Buscado = (strcmp(valoresBusca[j], "NULO") == 0) ? -1 : atoi(valoresBusca[j]);

        if (strcmp(nomesCampos[j], "codEstacao") == 0) {
            if (dados_get_codEstacao(d) != valorInt_Buscado) return 0;
        }
        else if (strcmp(nomesCampos[j], "codLinha") == 0) {
            if (dados_get_codLinha(d) != valorInt_Buscado) return 0;
        }
        else if (strcmp(nomesCampos[j], "codProxEstacao") == 0) {
            if (dados_get_codProxEstacao(d) != valorInt_Buscado) return 0;
        }
        else if (strcmp(nomesCampos[j], "distProxEstacao") == 0) {
            if (dados_get_distProxEstacao(d) != valorInt_Buscado) return 0;
        }
        else if (strcmp(nomesCampos[j], "codLinhaIntegra") == 0) {
            if (dados_get_codLinhaIntegra(d) != valorInt_Buscado) return 0;
        }
        else if (strcmp(nomesCampos[j], "codEstIntegra") == 0) {
            if (dados_get_codEstIntegra(d) != valorInt_Buscado) return 0;
        }
        else if (strcmp(nomesCampos[j], "nomeEstacao") == 0) {
            if (strcmp(valoresBusca[j], "") == 0) {
                if (d->tamNomeEstacao > 0) return 0;
            } else {
                if (d->nomeEstacao == NULL || strcmp(d->nomeEstacao, valoresBusca[j]) != 0) return 0;
            }
        } 
        else if (strcmp(nomesCampos[j], "nomeLinha") == 0) {
            if (strcmp(valoresBusca[j], "") == 0) {
                if (d->tamNomeLinha > 0) return 0;
            } else { 
                if (d->nomeLinha == NULL || strcmp(d->nomeLinha, valoresBusca[j]) != 0) return 0;
            }
        }
    }
    return 1;
}