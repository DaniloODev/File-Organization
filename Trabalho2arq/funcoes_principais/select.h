#ifndef SELECT_H
#define SELECT_H

void SELECT_FROM(char *nomeArqBin);     // Printa todos os registros marcados como ativos
// Printa registros ativos a partir de filtros dados pelo usuário, é capaz de
// operar sem um arquivo de indexação, porem de forma menos eficiente, fazendo uma
// busca sequencial, e não pela estrutura arvorte B que utilizamos para indexação
void SELECT_WHERE(char *nomeArqBin, char *nomeArqArvoreB, int numero_buscas);

#endif