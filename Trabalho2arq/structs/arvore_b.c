#include "arvore_b.h"
#include <stdlib.h>
#include <string.h>

struct _no {
    char removido;    // 1 Byte
    int proximo;      // 4 Bytes
    int tipoNo;       // 4 Bytes (0=raiz, 1=interno, -1=folha)
    int nroChaves;    // 4 Bytes
    int C[3];         // 12 Bytes (Chaves de busca: codEstacao)
    int Pr[3];        // 12 Bytes (Offsets correspondentes no arquivo de dados)
    int P[4];         // 16 Bytes (Ponteiros RRN para subárvores)
}; // Tamanho total exato: 53 Bytes

// Inicializa uma página em memória com valores padrão vazios (-1)
void inicializa_no_vazio(pagina *no) {
    no->removido = '0';
    no->proximo = -1;
    no->tipoNo = -1; // Folha por padrão
    no->nroChaves = 0;
    for (int i = 0; i < 3; i++) {
        no->C[i] = -1;
        no->Pr[i] = -1;
    }
    for (int i = 0; i < 4; i++) {
        no->P[i] = -1;
    }
}

// Lê uma página do arquivo de índices a partir do RRN informado
void le_no(FILE *arq_indice, int rrn, pagina *no) {
    long offset_pagina = 17 + ((long)rrn * 53);
    fseek(arq_indice, offset_pagina, SEEK_SET);
    fread(&no->removido, sizeof(char), 1, arq_indice);
    fread(&no->proximo, sizeof(int), 1, arq_indice);
    fread(&no->tipoNo, sizeof(int), 1, arq_indice);
    fread(&no->nroChaves, sizeof(int), 1, arq_indice);
    for (int i = 0; i < 3; i++) {
        fread(&no->C[i], sizeof(int), 1, arq_indice);
        fread(&no->Pr[i], sizeof(int), 1, arq_indice);
    }
    fread(no->P, sizeof(int), 4, arq_indice);
}

// Escreve/Atualiza uma página no arquivo de índices
void escreve_no(FILE *arq_indice, int rrn, pagina *no) {
    long offset_pagina = 17 + ((long)rrn * 53);
    fseek(arq_indice, offset_pagina, SEEK_SET);
    fwrite(&no->removido, sizeof(char), 1, arq_indice);
    fwrite(&no->proximo, sizeof(int), 1, arq_indice);
    fwrite(&no->tipoNo, sizeof(int), 1, arq_indice);
    fwrite(&no->nroChaves, sizeof(int), 1, arq_indice);
    for (int i = 0; i < 3; i++) {
        fwrite(&no->C[i], sizeof(int), 1, arq_indice);
        fwrite(&no->Pr[i], sizeof(int), 1, arq_indice);
    }
    fwrite(no->P, sizeof(int), 4, arq_indice);
}

// Função de Busca conforme o teu protótipo original
long busca_arvore_b(FILE *arq_indice, int rrn_atual, int chave_busca) {
    if (rrn_atual == -1) {
        return -1; // Chave não encontrada
    }

    pagina no;
    le_no(arq_indice, rrn_atual, &no);

    int i = 0;
    while (i < no.nroChaves && chave_busca > no.C[i]) {
        i++;
    }

    if (i < no.nroChaves && chave_busca == no.C[i]) {
        return (long)no.Pr[i]; // Retorna o byte offset no arquivo de dados
    }

    // Se for folha e não achou, a chave não existe
    if (no.tipoNo == -1) {
        return -1;
    }

    // Caso contrário, desce recursivamente na subárvore correta
    return busca_arvore_b(arq_indice, no.P[i], chave_busca);
}

// Função auxiliar recursiva para inserção com tratamento de split
int insere_recursivo(FILE *arq_indice, cabecalho_arvb *cab, int rrn_atual, int chave, long byte_offset, 
                     int *chave_promovida, int *pr_promovido, int *filho_dir_promovido) {
    
    // Condição de paragem: Chegámos ao fim da descida (abaixo de uma folha)
    if (rrn_atual == -1) {
        *chave_promovida = chave;
        *pr_promovido = (int)byte_offset;
        *filho_dir_promovido = -1;
        return 1; // Indica que uma inserção precisa de ser feita no nó folha pai
    }

    pagina no;
    le_no(arq_indice, rrn_atual, &no);

    // Procura a posição correta dentro do nó atual
    int pos = 0;
    while (pos < no.nroChaves && chave > no.C[pos]) {
        pos++;
    }

    // Evita duplicados
    if (pos < no.nroChaves && chave == no.C[pos]) {
        return 0; 
    }

    int ch_aux, pr_aux, f_dir_aux;
    int houve_promocao = insere_recursivo(arq_indice, cab, no.P[pos], chave, byte_offset, &ch_aux, &pr_aux, &f_dir_aux);
    
    if (!houve_promocao) return 0;

    // Se houve promoção do filho, tratamos a inserção no nível atual
    if (no.nroChaves < 3) {
        // Caso 1: Há espaço livre no nó atual. Desloca elementos e insere de forma ordenada.
        for (int i = no.nroChaves; i > pos; i--) {
            no.C[i] = no.C[i - 1];
            no.Pr[i] = no.Pr[i - 1];
            no.P[i + 1] = no.P[i];
        }
        no.C[pos] = ch_aux;
        no.Pr[pos] = pr_aux;
        no.P[pos + 1] = f_dir_aux;
        no.nroChaves++;
        
        escreve_no(arq_indice, rrn_atual, &no);
        return 0; // Inserção concluída com sucesso neste nível (sem propagar split)
    } else {
        // Caso 2: Nó Cheio! É necessário efetuar o Split.
        int aux_C[4];
        int aux_Pr[4];
        int aux_P[5];

        for (int i = 0; i < 3; i++) {
            aux_C[i] = no.C[i];
            aux_Pr[i] = no.Pr[i];
        }
        for (int i = 0; i < 4; i++) {
            aux_P[i] = no.P[i];
        }

        // Insere o elemento transbordado na posição ordenada correta da estrutura auxiliar
        int k = 3;
        while (k > pos) {
            aux_C[k] = aux_C[k - 1];
            aux_Pr[k] = aux_Pr[k - 1];
            aux_P[k + 1] = aux_P[k];
            k--;
        }
        aux_C[pos] = ch_aux;
        aux_Pr[pos] = pr_aux;
        aux_P[pos + 1] = f_dir_aux;

        // Aloca uma nova página à direita 
        int novo_rrn = getProxRRNArvb(cab);
        setProxRRNArvb(cab, novo_rrn + 1);
        setNroNosArvb(cab, getNroNosArvb(cab) + 1);

        pagina novo_no;
        inicializa_no_vazio(&novo_no);
        
        // CORREÇÃO DOS TIPOS: O novo nó da direita nunca pode herdar o tipo "Raiz (0)"
        if (no.tipoNo == -1) {
            novo_no.tipoNo = -1; // Se a esquerda era folha, o da direita também é
        } else {
            novo_no.tipoNo = 1;  // Se era interno ou raiz antiga, o da direita vira interno
        }

        // CRITÉRIO DE EMPATE: Promove o elemento de índice 2 (primeiro do 2º nó)
        *chave_promovida = aux_C[2];
        *pr_promovido = aux_Pr[2];
        *filho_dir_promovido = novo_rrn;

        // Nó esquerdo (atual) fica mais cheio (guarda os índices 0 e 1)
        no.nroChaves = 2;
        no.C[0] = aux_C[0];  no.Pr[0] = aux_Pr[0];
        no.C[1] = aux_C[1];  no.Pr[1] = aux_Pr[1];
        no.P[0] = aux_P[0];  no.P[1] = aux_P[1];  no.P[2] = aux_P[2];
        
        // Limpa a posição antiga que sobrou no nó esquerdo
        no.C[2] = -1;        no.Pr[2] = -1;
        no.P[3] = -1;

        // Nó direito (novo) assume a chave restante superior (índice 3)
        novo_no.nroChaves = 1;
        novo_no.C[0] = aux_C[3];  novo_no.Pr[0] = aux_Pr[3];
        novo_no.P[0] = aux_P[3];  novo_no.P[1] = aux_P[4];

        // Persiste as duas alterações em disco
        escreve_no(arq_indice, rrn_atual, &no);
        escreve_no(arq_indice, novo_rrn, &novo_no);

        return 1;
    }
}

void insere_arvore_b(FILE *arq_indice, cabecalho_arvb *cab, int chave, long byte_offset) {
    int raiz_atual = getNoRaizArvb(cab);

    // Árvore totalmente vazia: Criação do nó Raiz inicial
    if (raiz_atual == -1) {
        int novo_rrn = getProxRRNArvb(cab);
        setProxRRNArvb(cab, novo_rrn + 1);
        setNroNosArvb(cab, getNroNosArvb(cab) + 1);
        setNoRaizArvb(cab, novo_rrn);

        pagina raiz;
        inicializa_no_vazio(&raiz);
        raiz.tipoNo = -1; // -1 = Raiz e Folha simultaneamente no início
        raiz.nroChaves = 1;
        raiz.C[0] = chave;
        raiz.Pr[0] = (int)byte_offset;

        escreve_no(arq_indice, novo_rrn, &raiz);
        escreveCabecalhoArvb(arq_indice, cab); // CORREÇÃO: Sincroniza o cabeçalho no arquivo
        return;
    }

    int ch_prom, pr_prom, f_dir_prom;
    int transbordou = insere_recursivo(arq_indice, cab, raiz_atual, chave, byte_offset, &ch_prom, &pr_prom, &f_dir_prom);

    // Se o transbordo alcançou o topo absoluto, cria-se uma nova camada (Criação de nova Raiz)
    if (transbordou) {
        // Atualiza a raiz antiga para deixar de se intitular Raiz
        pagina raiz_antiga;
        le_no(arq_indice, raiz_atual, &raiz_antiga);
        raiz_antiga.tipoNo = (raiz_antiga.P[0] == -1) ? -1 : 1;
        escreve_no(arq_indice, raiz_atual, &raiz_antiga);

        // Aloca o novo nó do topo absoluto
        int nova_raiz_rrn = getProxRRNArvb(cab);
        setProxRRNArvb(cab, nova_raiz_rrn + 1);
        setNroNosArvb(cab, getNroNosArvb(cab) + 1);
        setNoRaizArvb(cab, nova_raiz_rrn);

        pagina nova_raiz;
        inicializa_no_vazio(&nova_raiz);
        nova_raiz.tipoNo = 0; // 0 = Raiz absoluta com filhos
        nova_raiz.nroChaves = 1;
        nova_raiz.C[0] = ch_prom;
        nova_raiz.Pr[0] = pr_prom;
        nova_raiz.P[0] = raiz_atual;   // Aponta para a antiga esquerda
        nova_raiz.P[1] = f_dir_prom;   // Aponta para a divisão à direita

        escreve_no(arq_indice, nova_raiz_rrn, &nova_raiz);
        escreveCabecalhoArvb(arq_indice, cab); // CORREÇÃO: Sincroniza o cabeçalho no arquivo
    }
}