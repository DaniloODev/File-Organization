#include "calcula.h"
#include "cabecalho.h"
#include <string.h>
#include <stdlib.h>

/// @brief                  Calcula o número de estações
/// @param trem             O array de structs para que seja possivel contar 
/// @param totalRegistros   Passa o total de registros ativos
/// @return                 Retorna um intero que representa o número de estações
int calc_nroEstacoes(dados *trem, int totalRegistros)
{
    int cont = 0;
    for (int i = 0; i < totalRegistros; i++)
    {
        int repetido = 0;
        for (int j = 0; j < i; j++) {   // Olha para tras para ver se o nome ja apareceu
            if (trem[i].nomeEstacao != NULL && trem[j].nomeEstacao != NULL) {
                if (strcmp(trem[i].nomeEstacao, trem[j].nomeEstacao) == 0) {
                    repetido = 1;
                    break;
                }
            }
        }
        if (!repetido) cont++;
    }
    return cont;
}

/// @brief                  Conta pares únicos (codEstacao, codProxEstacao)
/// @param trem             O array de structs para que seja possivel contar
/// @param totalRegistros   Passa o total de registros ativos
/// @return                 Retorna um intero que representa o número de pares de estações
int calc_nroPares(dados *trem, int totalRegistros)
{
    int cont = 0;
    for (int i = 0; i < totalRegistros; i++)
    {
        // Se a estação não tem próxima (final da linha), não forma par
        if (trem[i].codProxEstacao == -1) continue;

        int repetido = 0;
        for (int j = 0; j < i; j++)
        {
            if (trem[i].codEstacao == trem[j].codEstacao && trem[i].codProxEstacao == trem[j].codProxEstacao){
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
/// @param proxRRN  Próximo RRN (próximo espaço em branco para inserção de regitro) 
void recalcula_e_grava_cabecalho(FILE *binFile, int topo, int proxRRN)
{
    fseek(binFile, 17, SEEK_SET);
    dados *array_temp = NULL;
    int total_ativos = 0;
    dados trem_temp;

    // Lê o arquivo
    while (fread(&trem_temp.removido, sizeof(char), 1, binFile)) {
        fread(&trem_temp.proxRemovido, sizeof(int), 1, binFile);
        fread(&trem_temp.codEstacao, sizeof(int), 1, binFile);
        fread(&trem_temp.codLinha, sizeof(int), 1, binFile);
        fread(&trem_temp.codProxEstacao, sizeof(int), 1, binFile);
        fread(&trem_temp.distProxEstacao, sizeof(int), 1, binFile);
        fread(&trem_temp.codLinhaIntegra, sizeof(int), 1, binFile);
        fread(&trem_temp.codEstIntegra, sizeof(int), 1, binFile);
        
        fread(&trem_temp.tamNomeEstacao, sizeof(int), 1, binFile);
        if (trem_temp.tamNomeEstacao > 0 && trem_temp.tamNomeEstacao <= 80) {
            trem_temp.nomeEstacao = malloc(trem_temp.tamNomeEstacao + 1);
            fread(trem_temp.nomeEstacao, sizeof(char), trem_temp.tamNomeEstacao, binFile);
            trem_temp.nomeEstacao[trem_temp.tamNomeEstacao] = '\0';
        } else { trem_temp.nomeEstacao = NULL; }

        fread(&trem_temp.tamNomeLinha, sizeof(int), 1, binFile);
        if (trem_temp.tamNomeLinha > 0 && trem_temp.tamNomeLinha <= 80) {
            trem_temp.nomeLinha = malloc(trem_temp.tamNomeLinha + 1);
            fread(trem_temp.nomeLinha, sizeof(char), trem_temp.tamNomeLinha, binFile);
            trem_temp.nomeLinha[trem_temp.tamNomeLinha] = '\0';
        } else { trem_temp.nomeLinha = NULL; }

        // Pulando lixo $$$
        int bytesLidos = 37 + trem_temp.tamNomeEstacao + trem_temp.tamNomeLinha;
        fseek(binFile, 80 - bytesLidos, SEEK_CUR);

        if (trem_temp.removido == '0') {
            array_temp = realloc(array_temp, (total_ativos + 1) * sizeof(dados));
            array_temp[total_ativos] = trem_temp;
            total_ativos++;
        } else {
            if (trem_temp.nomeEstacao) free(trem_temp.nomeEstacao);
            if (trem_temp.nomeLinha) free(trem_temp.nomeLinha);
        }
    }

    int nroEstacoes = 0;
    int nroParesEstacao = 0;

    // Recalculando
    if (array_temp != NULL) {
        nroEstacoes = calc_nroEstacoes(array_temp, total_ativos);   // Recalcula o número de estações
        nroParesEstacao = calc_nroPares(array_temp, total_ativos);  // Recalcula o número de pares de estações
        
        for (int i = 0; i < total_ativos; i++) {    // Desalocando memória préviamente alocada
            if (array_temp[i].nomeEstacao) free(array_temp[i].nomeEstacao);
            if (array_temp[i].nomeLinha) free(array_temp[i].nomeLinha);
        }
        free(array_temp);
    }

    // Grava o cabeçalho limpo e atualizado
    cabecalho *cab = inicializaCabecalho();
    atualizaCabecalho(cab, '1', topo, proxRRN, nroEstacoes, nroParesEstacao);
    gravaCabecalho(binFile, cab);
    finalizaCabecalho(cab);
}