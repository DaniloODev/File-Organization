/**
   @author Marcelo da Silva Zanata - 16831070 
   @author Danilo de Oliveira Lopes - 17076598 
*/
#include <stdio.h>
#include "./structs/dados.h"
#include "./funcoes_principais/select.h"
#include "./funcoes_principais/create_table.h"
#include "./structs/cabecalho.h"
#include "./funcoes_principais/update.h"
#include "./funcoes_principais/delete.h"
#include "./funcoes_principais/insert_into.h"
#include "./structs/arvore_b.h"
#include "./funcoes_principais/create_index.h"
#include "./funcoes_principais/sort.h"

int main()
{
    int funcionalidade, n;
    char arqCsv[30], arqBin[30], arqBtree[30];

    // Novas variáveis para armazenar os parâmetros das funcionalidades 11 e 12
    char nomeCampo1[30], arqBin2[30], nomeCampo2[30], arqIndice[30];

    if (scanf("%d", &funcionalidade) != 1) return 0;

    switch (funcionalidade)
    {
        case 1:
            scanf("%s %s", arqCsv, arqBin);
            CREATE_TABLE(arqCsv, arqBin);
            break;
        case 2:
            scanf("%s", arqBin);
            SELECT_FROM(arqBin);
            break;
        case 3:
            scanf("%s", arqBin);
            scanf("%d", &n);
            SELECT_WHERE(arqBin, NULL, n);
            break;
        case 4:
            scanf("%s", arqBin);
            scanf("%d", &n);
            DELETE_WHERE(arqBin, NULL, n);
            break;
        case 5:
            scanf("%s", arqBin);
            scanf("%d", &n);
            INSERT_INTO(arqBin, NULL, n);
            break;
        case 6:
            scanf("%s", arqBin);
            scanf("%d", &n);
            UPDATE(arqBin, n);
            break;
        case 7:
            scanf("%s", arqBin);
            scanf("%s", arqBtree);
            criar_indice_arvore_b(arqBin, arqBtree);
            break;
        case 8:
            scanf("%s %s %d", arqBin, arqBtree, &n);
            SELECT_WHERE(arqBin, arqBtree, n);
            break;
        case 9:
            scanf("%s %s %d", arqBin, arqBtree, &n);
            INSERT_INTO(arqBin, arqBtree, n);
            break;
        case 10: 
            scanf("%s %s %d", arqBin, arqBtree, &n);
            DELETE_WHERE(arqBin, arqBtree, n);
            break;
        case 11:
            scanf("%s %s %s %s", arqBin, nomeCampo1, arqBin2, nomeCampo2);
            SELECT_ANINHADO(arqBin, nomeCampo1, arqBin2, nomeCampo2);
            break;
        case 12:
            scanf("%s %s %s %s %s", arqBin, nomeCampo1, arqBin2, nomeCampo2, arqIndice);
            SELECT_LOOP_UNICO(arqBin, nomeCampo1, arqBin2, nomeCampo2, arqIndice);
            break;
        case 13:
            char arqEntrada[30], campoOrd[30], arqSaida[30];
            scanf("%s %s %s", arqEntrada, campoOrd, arqSaida);
            ordena_arquivo_ram(arqEntrada, campoOrd, arqSaida);
            break;
        case 14:
            char arqEntrada1[30], campo1[30], arqEntrada2[30], campo2[30];
            scanf("%s %s %s %s", arqEntrada1, campo1, arqEntrada2, campo2);
            SELECT_SORT_MERGE(arqEntrada1, campo1, arqEntrada2, campo2);
            break;
        default:
            break;
    }

    return 0;
}