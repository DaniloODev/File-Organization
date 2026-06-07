#include "fornecidas.h"
#include "auxiliares.h"
#include "dados.h"
#include "cabecalho.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/// @brief              Realiza a validação verificando se existe ou não o que está sendo buscado
/// @param trem         Array de struct de dados
/// @param m_campos     Números de campos
/// @param nomesCampos  Tipo do campo
/// @param valoresBusca Conteúdo do campo
/// @return             Após a cascara de if, se passar em tudo ele retorna 1 = achou, retorna 0 se não achar
int valida_registro(dados *trem, int m_campos, char nomesCampos[][50], char valoresBusca[][100])
{
    for (int j = 0; j < m_campos; j++)
    {
        int valorInt_Buscado = (strcmp(valoresBusca[j], "NULO") == 0) ? -1 : atoi(valoresBusca[j]);

        if (strcmp(nomesCampos[j], "codEstacao") == 0){
            if (trem->codEstacao != valorInt_Buscado) return 0;}
        
        else if (strcmp(nomesCampos[j], "codLinha") == 0){
            if (trem->codLinha != valorInt_Buscado) return 0;}
        
        else if (strcmp(nomesCampos[j], "codProxEstacao") == 0){
            if (trem->codProxEstacao != valorInt_Buscado) return 0;}
        
        else if (strcmp(nomesCampos[j], "distProxEstacao") == 0){
            if (trem->distProxEstacao != valorInt_Buscado) return 0;}
        
        else if (strcmp(nomesCampos[j], "codLinhaIntegra") == 0){
            if (trem->codLinhaIntegra != valorInt_Buscado) return 0;}
        
        else if (strcmp(nomesCampos[j], "codEstIntegra") == 0){
            if (trem->codEstIntegra != valorInt_Buscado) return 0;}
        
        else if (strcmp(nomesCampos[j], "nomeEstacao") == 0)
        {
            if (strcmp(valoresBusca[j], "") == 0){
                if (trem->tamNomeEstacao > 0) return 0;}
            
            else{
                if (trem->nomeEstacao == NULL || strcmp(trem->nomeEstacao, valoresBusca[j]) != 0) return 0;}
            
        } 
        else if (strcmp(nomesCampos[j], "nomeLinha") == 0)
        {
            if (strcmp(valoresBusca[j], "") == 0){
                if (trem->tamNomeLinha > 0) return 0;}
            else{ 
                if (trem->nomeLinha == NULL || strcmp(trem->nomeLinha, valoresBusca[j]) != 0) return 0;}
        }
    }
    return 1; // Se passou por todos os IFs sem retornar 0, é porque deu match em tudo!
}