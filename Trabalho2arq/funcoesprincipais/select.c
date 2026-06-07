#include "auxiliares.h"
#include "fornecidas.h"
#include "dados.h"
#include "select.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/// @brief      Recebe a estação e printa ela.
/// @param trem Estação que foi encontrada e será printada.
static void printa_estacao(dados trem)
{
    printf("%d %s ", trem.codEstacao, trem.nomeEstacao);
    if (trem.codLinha != -1) printf("%d ", trem.codLinha); else printf("NULO ");
    if (trem.tamNomeLinha > 0 && trem.nomeLinha != NULL) printf("%s ", trem.nomeLinha); else printf("NULO ");
    if (trem.codProxEstacao != -1) printf("%d ", trem.codProxEstacao); else printf("NULO ");
    if (trem.distProxEstacao != -1) printf("%d ", trem.distProxEstacao); else printf("NULO ");
    if (trem.codLinhaIntegra != -1) printf("%d ", trem.codLinhaIntegra); else printf("NULO ");
    if (trem.codEstIntegra != -1) printf("%d\n", trem.codEstIntegra); else printf("NULO\n");
}

/// @brief              Abre o arquivo, percorre, se achou os registros correspondentes, printa os registros,
///                     se não, printa que não foi encontrado.
/// @param nomeArqBin   Nome do arquivo passado para manipulação.
void SELECT_FROM(char *nomeArqBin)
{
    FILE *file = abre_verifica_rb(nomeArqBin);

    fseek(file, 17, SEEK_SET);  // Pula o cabeçalho (17 bytes) para chegar nos registros de dados

    dados trem;
    int encontrou = 0;

    // Enquanto conseguir ler o campo removido, que é o primeiro de qualquer registro:
    while (fread(&trem.removido, sizeof(char), 1, file))
    {
        fread(&trem.proxRemovido, sizeof(int), 1, file);
        fread(&trem.codEstacao, sizeof(int), 1, file);
        fread(&trem.codLinha, sizeof(int), 1, file);
        fread(&trem.codProxEstacao, sizeof(int), 1, file);
        fread(&trem.distProxEstacao, sizeof(int), 1, file);
        fread(&trem.codLinhaIntegra, sizeof(int), 1, file);
        fread(&trem.codEstIntegra, sizeof(int), 1, file);
        
        // Leitura do nome da estação
        fread(&trem.tamNomeEstacao, sizeof(int), 1, file);
        trem.nomeEstacao = malloc((trem.tamNomeEstacao + 1) * sizeof(char));
        fread(trem.nomeEstacao, sizeof(char), trem.tamNomeEstacao, file);
        trem.nomeEstacao[trem.tamNomeEstacao] = '\0';

        // Leitura do nome da linha
        fread(&trem.tamNomeLinha, sizeof(int), 1, file);
        if (trem.tamNomeLinha > 0) {
            trem.nomeLinha = malloc((trem.tamNomeLinha + 1) * sizeof(char));
            fread(trem.nomeLinha, sizeof(char), trem.tamNomeLinha, file);
            trem.nomeLinha[trem.tamNomeLinha] = '\0';
        } else trem.nomeLinha = NULL;

        // Pula o lixo $ até completar os 80 bytes do registro (37 bytes fixos + tamanhos das strings)
        int bytesLidos = 37 + trem.tamNomeEstacao + trem.tamNomeLinha;
        fseek(file, 80 - bytesLidos, SEEK_CUR);

        if (trem.removido == '1') {   // Se estiver removido, limpa a memória e pula para o próximo
            if (trem.nomeEstacao != NULL) free(trem.nomeEstacao);
            if (trem.nomeLinha != NULL) free(trem.nomeLinha);
            continue;
        }

        encontrou = 1;
        printa_estacao(trem);   // Mostrando as estações encontradas

        // Desalocando a memória préviamente alocada
        if (trem.nomeEstacao != NULL) free(trem.nomeEstacao);
        if (trem.nomeLinha != NULL) free(trem.nomeLinha);
    }

    if (!encontrou)
        printf("Registro inexistente.\n");

    fclose(file);
}

/// @brief                  Abre o arquivo, percorre, lista todas as N buscas com todos os resultados
///                         correspondentes, se não encontrar, printa que não existe. 
/// @param nomeArqBin       Nome do arquivo passado para manipulação.
/// @param numero_buscas    Número de buscas, onde poderão ser filtrados por campos diferentes.
void SELECT_WHERE(char *nomeArqBin, int numero_buscas)
{
    FILE *file = abre_verifica_rb(nomeArqBin);

    for (int b = 0; b < numero_buscas; b++)
    {
        int m_campos;
        scanf("%d", &m_campos); // Lê quantos campos essa busca vai ter

        char nomesCampos[m_campos][50];
        char valoresBusca[m_campos][100];

        // Lê os pares nomeCampo valorCampo
        for (int j = 0; j < m_campos; j++)
        {
            scanf("%s", nomesCampos[j]);
            
            //Strings usam ScanQuoteString, números e nulo usam scanf normal 
            if (strcmp(nomesCampos[j], "nomeEstacao") == 0 || strcmp(nomesCampos[j], "nomeLinha") == 0)
                ScanQuoteString(valoresBusca[j]);
            
            else scanf("%s", valoresBusca[j]); 
        }

        fseek(file, 17, SEEK_SET); // Volta pro início dos dados a cada nova busca
        dados trem;
        int encontrouAoMenosUm = 0;

        while (fread(&trem.removido, sizeof(char), 1, file))
        {
            // Leitura fixa
            fread(&trem.proxRemovido, sizeof(int), 1, file);
            fread(&trem.codEstacao, sizeof(int), 1, file);
            fread(&trem.codLinha, sizeof(int), 1, file);
            fread(&trem.codProxEstacao, sizeof(int), 1, file);
            fread(&trem.distProxEstacao, sizeof(int), 1, file);
            fread(&trem.codLinhaIntegra, sizeof(int), 1, file);
            fread(&trem.codEstIntegra, sizeof(int), 1, file);
            
            // Leitura variável
            fread(&trem.tamNomeEstacao, sizeof(int), 1, file);
            trem.nomeEstacao = malloc(trem.tamNomeEstacao + 1);
            fread(trem.nomeEstacao, sizeof(char), trem.tamNomeEstacao, file);
            trem.nomeEstacao[trem.tamNomeEstacao] = '\0';

            fread(&trem.tamNomeLinha, sizeof(int), 1, file);
            if (trem.tamNomeLinha > 0) {
                trem.nomeLinha = malloc(trem.tamNomeLinha + 1);
                fread(trem.nomeLinha, sizeof(char), trem.tamNomeLinha, file);
                trem.nomeLinha[trem.tamNomeLinha] = '\0';
            } else trem.nomeLinha = NULL;

            // Pula o lixo $ até completar os 80 bytes do registro (37 bytes fixos + tamanhos das strings)
            int bytesLidos = 37 + trem.tamNomeEstacao + trem.tamNomeLinha;
            fseek(file, 80 - bytesLidos, SEEK_CUR);

            if (trem.removido == '1') {
                free(trem.nomeEstacao);
                if (trem.nomeLinha) free(trem.nomeLinha);
                continue;
            }

            int match = valida_registro(&trem, m_campos, nomesCampos, valoresBusca); // Assume que o registro serve, a menos que falhe em um dos critérios.
            if (match)
            {
                encontrouAoMenosUm = 1;
                printa_estacao(trem);   // Mostrando as estações encontradas

            }
            free(trem.nomeEstacao);
            if (trem.nomeLinha)
                free(trem.nomeLinha);
        }        
        if (!encontrouAoMenosUm)
            printf("Registro inexistente.\n");

        printf("\n");
    }
    fclose(file);
}