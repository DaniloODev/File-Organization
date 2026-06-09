#include <stdio.h>
#include "dados.h"
#ifndef BUSCA_H
#define BUSCA_H

// funcao: 0 para selectFrom, 1 para selectWhere, 2 para update, 3 para delete
void busca (dados trem, FILE *file, int funcao, char **valoresBusca, char **nomesCampos, int m_campos, char **nomesCamposAtualiza, char **valoresAtualiza, int p_campos); 

#endif