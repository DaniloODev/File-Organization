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
#include "./funcoes_arvb/create_index.h"

int main()
{
    int funcionalidade, n;
    char arqCsv[30], arqBin[30], arqBtree[30];

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
        default:
            break;
    }

    return 0;
}