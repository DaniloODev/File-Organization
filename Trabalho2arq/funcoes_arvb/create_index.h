#ifndef CREATE_INDEX_H
#define CREATE_INDEX_H

// Cria um arquivo de indexação do arquivo contendo os registros utilizando uma
// árvore-B como estrutura de dados, para maior eficiencia de busca através
// de uma primary key.
void criar_indice_arvore_b(const char *nome_arq_dados, const char *nome_arq_indice);

#endif