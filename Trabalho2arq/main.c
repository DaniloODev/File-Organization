/**
   @author Marcelo da Silva Zanata - 16831070 
   @author Danilo de Oliveira Lopes - 17076598 
*/
#include <stdio.h>
#include "dados.h"
#include "select.h"
#include "create_table.h"
#include "cabecalho.h"
#include "update.h"
#include "delete.h"
#include "insert_into.h"

int main()
{
    int funcionalidade;
    char arqCsv[30], arqBin[30];

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
            int n0;
            scanf("%s", arqBin);
            scanf("%d", &n0);
            SELECT_WHERE(arqBin, n0);
            break;
        case 4: 
            char arqBin[100];
            int n1;
            scanf("%s", arqBin);
            scanf("%d", &n1);
            DELETE_WHERE(arqBin, n1);
            break;
        case 5:
            int n2;
            scanf("%s", arqBin);
            scanf("%d", &n2);
            INSERT_INTO(arqBin, n2);
            break;
        case 6:
            int n3;
            scanf("%s", arqBin);
            scanf("%d", &n3);
            UPDATE(arqBin, n3);
            break;
        default:
            break;
    }

    return 0;
}