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
};                    // Tamanho total: 53 Bytes

/// @brief Inicializa uma página em memória com valores padrão
/// @param no Ponteiro para a página a ser inicializada
void inicializa_no_vazio(pagina *no) {
    no->removido = '0';
    no->proximo = -1;
    no->tipoNo = -1;
    no->nroChaves = 0;
    for (int i = 0; i < 3; i++) {
        no->C[i] = -1;
        no->Pr[i] = -1;
    }
    for (int i = 0; i < 4; i++) {
        no->P[i] = -1;
    }
}

/// @brief Lê uma página do arquivo de índices a partir do RRN informado
/// @param arq_indice Ponteiro para o arquivo de índices
/// @param rrn RRN da página a ser lida
/// @param no Ponteiro para a página a ser lida
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

/// @brief Escreve/Atualiza uma página no arquivo de índices
/// @param arq_indice Ponteiro para o arquivo de índices
/// @param rrn RRN da página a ser escrita
/// @param no Ponteiro para a página a ser escrita
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

/// @brief Função de Busca na Árvore B
/// @param arq_indice Ponteiro para o arquivo de índices
/// @param rrn_atual RRN da página atual
/// @param chave_busca Chave a ser buscada
/// @return Byte offset no arquivo de dados ou -1 se não encontrada
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

/// @brief Função auxiliar recursiva para inserção com tratamento de split
/// @param arq_indice Ponteiro para o arquivo de índices
/// @param cab Ponteiro para o cabeçalho da árvore B
/// @param rrn_atual RRN da página atual
/// @param chave Chave a ser inserida
/// @param byte_offset Byte offset no arquivo de dados
/// @param chave_promovida Ponteiro para a chave promovida
/// @param pr_promovido Ponteiro para o ponteiro para a página promovida
/// @param filho_dir_promovido Ponteiro para o filho da direita promovido
/// @return 1 se houve promoção, 0 caso contrário
int insere_recursivo(FILE *arq_indice, cabecalho_arvb *cab, int rrn_atual, int chave, long byte_offset, 
                     int *chave_promovida, int *pr_promovido, int *filho_dir_promovido) {
    
    // Para se chegar abaixo de um nó folha
    if (rrn_atual == -1) {
        *chave_promovida = chave;
        *pr_promovido = (int)byte_offset;
        *filho_dir_promovido = -1;
        return 1; // Inserção nó folha pai
    }

    pagina no;
    le_no(arq_indice, rrn_atual, &no);

    // busca a posição correta dentro do nó atual
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

    // Se houve promoção do filho, tratamos a inserção no nivel atual
    if (no.nroChaves < 3) {
        // Caso 1: tem espaço livre no nó atual. Desloca elementos e insere ordenado
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
        return 0;
    } else {
        // Caso 2: Sem espaço no nó, faz o split
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

        // Insere o elemento que não cabia na posição correta do vetor auxiliar
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

        // Aloca uma nova página a direita 
        int novo_rrn = pega_rrn_disponivel_arvb(arq_indice, cab);
        setNroNosArvb(cab, getNroNosArvb(cab) + 1);

        pagina novo_no;
        inicializa_no_vazio(&novo_no);
        
        // Ajuste do tipo de nó
        if (no.tipoNo == -1) {
            novo_no.tipoNo = -1; // Se a esquerda era folha, o da direita também é
        } else {
            novo_no.tipoNo = 1;  // Se era interno ou raiz antiga, o da direita vira interno
        }

        // Caso empare, promove o elemento de índice 2 (primeiro do segundo nó)
        *chave_promovida = aux_C[2];
        *pr_promovido = aux_Pr[2];
        *filho_dir_promovido = novo_rrn;

        // Nó esquerdo atual fica mais cheio (guarda os índices 0 e 1)
        no.nroChaves = 2;
        no.C[0] = aux_C[0];  no.Pr[0] = aux_Pr[0];
        no.C[1] = aux_C[1];  no.Pr[1] = aux_Pr[1];
        no.P[0] = aux_P[0];  no.P[1] = aux_P[1];  no.P[2] = aux_P[2];
        
        // Limpa a posição antiga que sobrou no nó esquerdo
        no.C[2] = -1;        no.Pr[2] = -1;
        no.P[3] = -1;

        // Nó direito novo assume as chaves restantes (índice 3)
        novo_no.nroChaves = 1;
        novo_no.C[0] = aux_C[3];  novo_no.Pr[0] = aux_Pr[3];
        novo_no.P[0] = aux_P[3];  novo_no.P[1] = aux_P[4];

        // Escreve no disco
        escreve_no(arq_indice, rrn_atual, &no);
        escreve_no(arq_indice, novo_rrn, &novo_no);

        return 1;
    }
}

/// @brief Insere uma chave na árvore B
/// @param arq_indice Ponteiro para o arquivo de índices 
/// @param cab Ponteiro para o cabeçalho da árvore B
/// @param chave Chave a ser inserida
/// @param byte_offset Offset do byte no arquivo
void insere_arvore_b(FILE *arq_indice, cabecalho_arvb *cab, int chave, long byte_offset) {
    int raiz_atual = getNoRaizArvb(cab);

    // Árvore vazia, cria nó raiz/foha
    if (raiz_atual == -1) {
        int novo_rrn = pega_rrn_disponivel_arvb(arq_indice, cab);
        setNroNosArvb(cab, getNroNosArvb(cab) + 1);
        setNoRaizArvb(cab, novo_rrn);

        pagina raiz;
        inicializa_no_vazio(&raiz);
        raiz.tipoNo = -1; 
        raiz.nroChaves = 1;
        raiz.C[0] = chave;
        raiz.Pr[0] = (int)byte_offset;

        escreve_no(arq_indice, novo_rrn, &raiz);
        escreveCabecalhoArvb(arq_indice, cab);
        return;
    }

    int ch_prom, pr_prom, f_dir_prom;
    int transbordou = insere_recursivo(arq_indice, cab, raiz_atual, chave, byte_offset, &ch_prom, &pr_prom, &f_dir_prom);

    // Se não cabe mais, cria uma nova raiz e promove elemento 
    if (transbordou) {
        // Atualiza a raiz antiga que vira ou folha ou nó interno
        pagina raiz_antiga;
        le_no(arq_indice, raiz_atual, &raiz_antiga);
        raiz_antiga.tipoNo = (raiz_antiga.P[0] == -1) ? -1 : 1;
        escreve_no(arq_indice, raiz_atual, &raiz_antiga);

        // Aloca o novo nó do
        int nova_raiz_rrn = pega_rrn_disponivel_arvb(arq_indice, cab);
        setNroNosArvb(cab, getNroNosArvb(cab) + 1);
        setNoRaizArvb(cab, nova_raiz_rrn);

        pagina nova_raiz;
        inicializa_no_vazio(&nova_raiz);
        nova_raiz.tipoNo = 0;
        nova_raiz.nroChaves = 1;
        nova_raiz.C[0] = ch_prom;
        nova_raiz.Pr[0] = pr_prom;
        nova_raiz.P[0] = raiz_atual;   // Aponta para a antiga esquerda
        nova_raiz.P[1] = f_dir_prom;   // Aponta para a divisão à direita

        escreve_no(arq_indice, nova_raiz_rrn, &nova_raiz);
        escreveCabecalhoArvb(arq_indice, cab);
    }
}



void destroi_pagina_arvore_b(FILE *file_arvb, cabecalho_arvb *cab, int rrn_destruido) {
    pagina no_destruido;
    le_no(file_arvb, rrn_destruido, &no_destruido);
    
    no_destruido.removido = '1';
    no_destruido.proximo = getTopoArvb(cab); 
    
    setTopoArvb(cab, rrn_destruido);
    setNroNosArvb(cab, getNroNosArvb(cab) - 1);
    
    escreve_no(file_arvb, rrn_destruido, &no_destruido);
}

void underflow_routine(FILE *file_arvb, cabecalho_arvb *cab, pagina *pai, int pos_filho, int rrn_pai) {
    int rrn_filho = pai->P[pos_filho];
    pagina filho;
    le_no(file_arvb, rrn_filho, &filho);

    // 1. Empréstimo da Direita
    if (pos_filho < pai->nroChaves) {
        int rrn_dir = pai->P[pos_filho + 1];
        pagina dir;
        le_no(file_arvb, rrn_dir, &dir);
        
        if (dir.nroChaves > 1) { 
            int total_chaves = 1 + dir.nroChaves; 
            int comb_C[5], comb_Pr[5], comb_P[6];
            
            comb_C[0] = pai->C[pos_filho];
            comb_Pr[0] = pai->Pr[pos_filho];
            comb_P[0] = filho.P[0];
            
            for (int i = 0; i < dir.nroChaves; i++) {
                comb_C[i + 1] = dir.C[i];
                comb_Pr[i + 1] = dir.Pr[i];
                comb_P[i + 1] = dir.P[i];
            }
            comb_P[dir.nroChaves + 1] = dir.P[dir.nroChaves];

            int num_esq = total_chaves / 2; 
            filho.nroChaves = num_esq;
            for (int i = 0; i < num_esq; i++) {
                filho.C[i] = comb_C[i];
                filho.Pr[i] = comb_Pr[i];
                filho.P[i] = comb_P[i];
            }
            filho.P[num_esq] = comb_P[num_esq];
            
            for (int i = num_esq; i < 3; i++) { filho.C[i] = -1; filho.Pr[i] = -1; }
            for (int i = num_esq + 1; i < 4; i++) { filho.P[i] = -1; }

            pai->C[pos_filho] = comb_C[num_esq];
            pai->Pr[pos_filho] = comb_Pr[num_esq];

            int num_dir = total_chaves - num_esq - 1;
            dir.nroChaves = num_dir;
            for (int i = 0; i < num_dir; i++) {
                dir.C[i] = comb_C[num_esq + 1 + i];
                dir.Pr[i] = comb_Pr[num_esq + 1 + i];
                dir.P[i] = comb_P[num_esq + 1 + i];
            }
            dir.P[num_dir] = comb_P[total_chaves];
            
            for (int i = num_dir; i < 3; i++) { dir.C[i] = -1; dir.Pr[i] = -1; }
            for (int i = num_dir + 1; i < 4; i++) { dir.P[i] = -1; }

            escreve_no(file_arvb, rrn_filho, &filho);
            escreve_no(file_arvb, rrn_dir, &dir);
            escreve_no(file_arvb, rrn_pai, pai);
            return;
        }
    }

    // 2. Empréstimo da Esquerda
    if (pos_filho > 0) {
        int rrn_esq = pai->P[pos_filho - 1];
        pagina esq;
        le_no(file_arvb, rrn_esq, &esq);
        
        if (esq.nroChaves > 1) { 
            int total_chaves = esq.nroChaves + 1; 
            int comb_C[5], comb_Pr[5], comb_P[6];
            
            for (int i = 0; i < esq.nroChaves; i++) {
                comb_C[i] = esq.C[i];
                comb_Pr[i] = esq.Pr[i];
                comb_P[i] = esq.P[i];
            }
            comb_P[esq.nroChaves] = esq.P[esq.nroChaves];
            comb_C[esq.nroChaves] = pai->C[pos_filho - 1];
            comb_Pr[esq.nroChaves] = pai->Pr[pos_filho - 1];
            comb_P[esq.nroChaves + 1] = filho.P[0];

            int num_esq = total_chaves / 2;
            esq.nroChaves = num_esq;
            for (int i = 0; i < num_esq; i++) {
                esq.C[i] = comb_C[i];
                esq.Pr[i] = comb_Pr[i];
                esq.P[i] = comb_P[i];
            }
            esq.P[num_esq] = comb_P[num_esq];
            for (int i = num_esq; i < 3; i++) { esq.C[i] = -1; esq.Pr[i] = -1; }
            for (int i = num_esq + 1; i < 4; i++) { esq.P[i] = -1; }

            pai->C[pos_filho - 1] = comb_C[num_esq];
            pai->Pr[pos_filho - 1] = comb_Pr[num_esq];

            int num_dir = total_chaves - num_esq - 1;
            filho.nroChaves = num_dir;
            for (int i = 0; i < num_dir; i++) {
                filho.C[i] = comb_C[num_esq + 1 + i];
                filho.Pr[i] = comb_Pr[num_esq + 1 + i];
                filho.P[i] = comb_P[num_esq + 1 + i];
            }
            filho.P[num_dir] = comb_P[total_chaves];
            for (int i = num_dir; i < 3; i++) { filho.C[i] = -1; filho.Pr[i] = -1; }
            for (int i = num_dir + 1; i < 4; i++) { filho.P[i] = -1; }

            escreve_no(file_arvb, rrn_filho, &filho);
            escreve_no(file_arvb, rrn_esq, &esq);
            escreve_no(file_arvb, rrn_pai, pai);
            return;
        }
    }

    // 3. Concatenação à Esquerda
    if (pos_filho > 0) {
        int rrn_esq = pai->P[pos_filho - 1];
        pagina esq;
        le_no(file_arvb, rrn_esq, &esq);

        esq.C[esq.nroChaves] = pai->C[pos_filho - 1];
        esq.Pr[esq.nroChaves] = pai->Pr[pos_filho - 1];
        esq.P[esq.nroChaves + 1] = filho.P[0];
        esq.nroChaves++;

        for (int i = pos_filho - 1; i < pai->nroChaves - 1; i++) {
            pai->C[i] = pai->C[i + 1];
            pai->Pr[i] = pai->Pr[i + 1];
            pai->P[i + 1] = pai->P[i + 2];
        }
        pai->C[pai->nroChaves - 1] = -1;
        pai->Pr[pai->nroChaves - 1] = -1;
        pai->P[pai->nroChaves] = -1;
        pai->nroChaves--;

        escreve_no(file_arvb, rrn_esq, &esq);
        escreve_no(file_arvb, rrn_pai, pai);
        destroi_pagina_arvore_b(file_arvb, cab, rrn_filho);
        return;
    }

    // 4. Concatenação à Direita
    if (pos_filho < pai->nroChaves) {
        int rrn_dir = pai->P[pos_filho + 1];
        pagina dir;
        le_no(file_arvb, rrn_dir, &dir);

        filho.C[0] = pai->C[pos_filho];
        filho.Pr[0] = pai->Pr[pos_filho];
        filho.P[1] = dir.P[0];
        for (int i = 0; i < dir.nroChaves; i++) {
            filho.C[1 + i] = dir.C[i];
            filho.Pr[1 + i] = dir.Pr[i];
            filho.P[2 + i] = dir.P[1 + i];
        }
        filho.nroChaves = 1 + dir.nroChaves;

        for (int i = pos_filho; i < pai->nroChaves - 1; i++) {
            pai->C[i] = pai->C[i + 1];
            pai->Pr[i] = pai->Pr[i + 1];
            pai->P[i + 1] = pai->P[i + 2];
        }
        pai->C[pai->nroChaves - 1] = -1;
        pai->Pr[pai->nroChaves - 1] = -1;
        pai->P[pai->nroChaves] = -1;
        pai->nroChaves--;

        escreve_no(file_arvb, rrn_filho, &filho);
        escreve_no(file_arvb, rrn_pai, pai);
        destroi_pagina_arvore_b(file_arvb, cab, rrn_dir); 
        return;
    }
}

int DELETE_arvb(FILE *file_arvb, cabecalho_arvb *cab, int rrn_atual, int chave_procurada) {
    if (rrn_atual == -1) return 0; 

    pagina no_atual;
    le_no(file_arvb, rrn_atual, &no_atual);

    int pos = 0;
    int encontrou = 0;
    for (pos = 0; pos < no_atual.nroChaves; pos++) {
        if (no_atual.C[pos] == chave_procurada) {
            encontrou = 1;
            break;
        }
        if (no_atual.C[pos] > chave_procurada) break;
    }

    if (encontrou) {
        if (no_atual.tipoNo == -1) { 
            for (int i = pos; i < no_atual.nroChaves - 1; i++) {
                no_atual.C[i] = no_atual.C[i + 1];
                no_atual.Pr[i] = no_atual.Pr[i + 1];
                no_atual.P[i + 1] = no_atual.P[i + 2];
            }
            no_atual.C[no_atual.nroChaves - 1] = -1;
            no_atual.Pr[no_atual.nroChaves - 1] = -1;
            no_atual.P[no_atual.nroChaves] = -1;
            no_atual.nroChaves--;
            
            escreve_no(file_arvb, rrn_atual, &no_atual);
            return (no_atual.nroChaves < 1); 
        } else { 
            int rrn_sucessor = no_atual.P[pos + 1]; 
            pagina succ_node;
            le_no(file_arvb, rrn_sucessor, &succ_node);
            
            while (succ_node.tipoNo != -1) { 
                rrn_sucessor = succ_node.P[0];
                le_no(file_arvb, rrn_sucessor, &succ_node);
            }
            
            int succ_chave = succ_node.C[0];
            int succ_pr = succ_node.Pr[0];

            no_atual.C[pos] = succ_chave;
            no_atual.Pr[pos] = succ_pr;
            escreve_no(file_arvb, rrn_atual, &no_atual);

            int underflow = DELETE_arvb(file_arvb, cab, no_atual.P[pos + 1], succ_chave);
            
            if (underflow) {
                le_no(file_arvb, rrn_atual, &no_atual);
                underflow_routine(file_arvb, cab, &no_atual, pos + 1, rrn_atual);
            }
            
            le_no(file_arvb, rrn_atual, &no_atual);
            return (no_atual.nroChaves < 1);
        }
    } else {
        int underflow = DELETE_arvb(file_arvb, cab, no_atual.P[pos], chave_procurada);
        
        if (underflow) {
            le_no(file_arvb, rrn_atual, &no_atual);
            underflow_routine(file_arvb, cab, &no_atual, pos, rrn_atual);
        }
        
        le_no(file_arvb, rrn_atual, &no_atual);
        return (no_atual.nroChaves < 1);
    }
}

// Essa função encapsula a remoção e cuida da raiz se ela esvaziar!
void remover_arvore_b(FILE *arq_indice, cabecalho_arvb *cab, int chave_procurada) {
    int raiz_atual = getNoRaizArvb(cab); // ou cab->noRaiz, dependendo de como você pega
    if (raiz_atual == -1) return;

    DELETE_arvb(arq_indice, cab, raiz_atual, chave_procurada);

    // Verifica se a raiz esvaziou após a remoção
    pagina raiz;
    le_no(arq_indice, raiz_atual, &raiz);
    
    if (raiz.nroChaves == 0) {
        if (raiz.tipoNo != -1) {
            int nova_raiz_rrn = raiz.P[0];
            setNoRaizArvb(cab, nova_raiz_rrn); 
            destroi_pagina_arvore_b(arq_indice, cab, raiz_atual);
            
            pagina nova_raiz;
            le_no(arq_indice, nova_raiz_rrn, &nova_raiz);
            if (nova_raiz.tipoNo == 1) { // se era nó intermediário
                nova_raiz.tipoNo = 0;    // atualiza para ser a nova raiz
                escreve_no(arq_indice, nova_raiz_rrn, &nova_raiz);
            } 
        } else {
            // A árvore ficou inteira vazia
            setNoRaizArvb(cab, -1); // ou cab->noRaiz = -1;
            destroi_pagina_arvore_b(arq_indice, cab, raiz_atual);
        }
    }
}

int pega_rrn_disponivel_arvb(FILE *arq_indice, cabecalho_arvb *cab) {
    int topo_arvb = getTopoArvb(cab); // Pega o campo 'topo' do cabeçalho da árvore
    
    if (topo_arvb != -1) {
        // REAPROVEITAMENTO: Temos uma página removida na árvore!
        pagina no_removido;
        le_no(arq_indice, topo_arvb, &no_removido);
        
        // O topo da árvore agora avança para o 'proximo' da pilha encadeada
        setTopoArvb(cab, no_removido.proximo);
        
        // Retorna o RRN recuperado sem mexer no proxRRN
        return topo_arvb;
    } else {
        // Pilha vazia: aloca no fim do arquivo e incrementa
        int novo_rrn = getProxRRNArvb(cab);
        setProxRRNArvb(cab, novo_rrn + 1);
        return novo_rrn;
    }
}