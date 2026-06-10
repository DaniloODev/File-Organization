#include "calcula.h"
#include "../structs/cabecalho.h"
#include "../structs/dados.h"
#include <string.h>
#include <stdlib.h>

/// @brief                  Calcula o número de estações únicas
/// @param d                O array de ponteiros para as structs de dados
/// @param totalRegistros   Passa o total de registros ativos
/// @return                 Retorna um inteiro que representa o número de estações
int calc_nroEstacoes(dados **d, int totalRegistros)
{
    int cont = 0;
    for (int i = 0; i < totalRegistros; i++)
    {
        char *nomeI = dados_get_nomeEstacao(d[i]);
        
        // Se a estação não tem nome válido ou está vazia, não conta
        if (nomeI == NULL || strlen(nomeI) == 0) continue;

        int repetido = 0;
        for (int j = 0; j < i; j++) {   
            char *nomeJ = dados_get_nomeEstacao(d[j]); 
            
            if (nomeJ != NULL && strcmp(nomeI, nomeJ) == 0) {
                repetido = 1;
                break;
            }
        }
        if (!repetido) cont++;
    }
    return cont;
}

/// @brief                  Conta pares únicos (codEstacao, codProxEstacao)
/// @param d                O array de ponteiros para as structs de dados
/// @param totalRegistros   Passa o total de registros ativos
/// @return                 Retorna um inteiro que representa o número de pares de estações
int calc_nroPares(dados **d, int totalRegistros)
{
    int cont = 0;
    for (int i = 0; i < totalRegistros; i++)
    {
        int codEstI = dados_get_codEstacao(d[i]);
        int codProxI = dados_get_codProxEstacao(d[i]);

        // Se o código da estação ou da próxima for inválido (-1), não forma par
        if (codEstI == -1 || codProxI == -1) continue;

        int repetido = 0;
        for (int j = 0; j < i; j++)
        {
            if (codEstI == dados_get_codEstacao(d[j]) && 
                codProxI == dados_get_codProxEstacao(d[j])) {
                repetido = 1;
                break;
            }
        }
        if (!repetido) cont++;
    }
    return cont;
}

/// @brief          Recalcula o número de estações ativas, número de pares de estações
///                 e por fim grava as alterações no arquivo
/// @param binFile  O arquivo a ser manipulado
/// @param topo     Topo da pilha de exclusão (se = -1, insere no RRN)
/// @param proxRRN  Próximo RRN
void recalcula_e_grava_cabecalho(FILE *binFile, int topo, int proxRRN)
{   
    // Move o ponteiro para o início dos dados dos registros (pula os 17 bytes)
    fseek(binFile, 17, SEEK_SET);
    
    dados **array_temp = NULL;
    int total_ativos = 0;
    dados *reg_atual = NULL;

    // Lê o arquivo do início ao fim usando as permissões do TAD
    while ((reg_atual = dados_le_binario(binFile)) != NULL) 
    {
        if (dados_get_removido(reg_atual) == '0') {
            array_temp = realloc(array_temp, (total_ativos + 1) * sizeof(dados *));
            array_temp[total_ativos] = reg_atual;
            total_ativos++;
        } else {
            dados_apaga(reg_atual);
        }
    }

    int nroEstacoes = 0;
    int nroParesEstacao = 0;

    if (array_temp != NULL) {
        nroEstacoes = calc_nroEstacoes(array_temp, total_ativos);   
        nroParesEstacao = calc_nroPares(array_temp, total_ativos);  
        
        for (int i = 0; i < total_ativos; i++) {    
            dados_apaga(array_temp[i]); 
        }
        free(array_temp); 
    }

    // Aloca e monta o cabeçalho final atualizado
    cabecalho *cab = inicializaCabecalho();
    atualizaCabecalho(cab, '1', topo, proxRRN, nroEstacoes, nroParesEstacao);
    
    gravaCabecalho(binFile, cab);
    finalizaCabecalho(cab);
}