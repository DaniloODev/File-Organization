#include "printaEstacao.h"
#include "../estrutura_verifica/dados.h"

/// @brief      Recebe a estação e printa ela.
/// @param trem Estação que foi encontrada e será printada.
void printa_estacao(dados trem)
{
    printf("%d %s ", trem.codEstacao, trem.nomeEstacao);
    if (trem.codLinha != -1) printf("%d ", trem.codLinha); else printf("NULO ");
    if (trem.tamNomeLinha > 0 && trem.nomeLinha != NULL) printf("%s ", trem.nomeLinha); else printf("NULO ");
    if (trem.codProxEstacao != -1) printf("%d ", trem.codProxEstacao); else printf("NULO ");
    if (trem.distProxEstacao != -1) printf("%d ", trem.distProxEstacao); else printf("NULO ");
    if (trem.codLinhaIntegra != -1) printf("%d ", trem.codLinhaIntegra); else printf("NULO ");
    if (trem.codEstIntegra != -1) printf("%d\n", trem.codEstIntegra); else printf("NULO\n");
}