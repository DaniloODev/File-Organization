#ifndef INSERT_INTO_H
#define INSERT_INTO_H

// Insere registros na tabela, com ou sem arquivo de indexação como um parametro
// realizando a inserção através da árvore B, ou de forma sequencial, analizando
// se existe um topo ou se insere no fim.
void INSERT_INTO(char *nomeArqBin, char *nomeArqIndice ,int n_insercoes);

#endif