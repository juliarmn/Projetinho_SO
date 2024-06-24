#ifndef MEMORIA_H
#define MEMORIA_H

#include "../processo/processo.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Pagina de tam fixo
// Segmento que tem o tamanho do processo

// Defines de tamanhos
#define KILOBYTE 1024
#define GIGABYTE (1024 * KILOBYTE * KILOBYTE)
#define TAMANHO (1 * GIGABYTE)
#define TAMANHO_PAGINA (8 * KILOBYTE)
#define NUM_TOTAL_PAG 130

// ====== STRUCTS =======

// Struct de pagina
typedef struct pag
{
    int id;
    int ocupada; // verifica se tem 1 processo nela
} Pagina;

// Vetor de paginas que é a memoria
typedef struct vetor
{
    Pagina tabela_pag[NUM_TOTAL_PAG];
    int qtd_pag_livre;

} Vetor_tabela_pag;

// Struct de segmento
typedef struct Segment
{
    int id;
    int index_vet_pag[130];
    int dirty_bit;
    int modificado;
    int bit_presenca;
    // int pag_count;
    Processo *processo;
    int quantidade_pag;
    struct Segment *prox;
} Segmento;

// ======= FUNCOES =======:

// Função para inicializar a memoria
int buscar_seg(Processo *processo, Segmento *cabeca_seg);
int achar_pag_livre(Vetor_tabela_pag *tabela_pagina);
Segmento *inserir_processo_memoria(Processo *processo, Vetor_tabela_pag *tabela_paginas, Segmento **cabeca_segmento);
void inicializar_pag(Vetor_tabela_pag *tabela_paginas);
Segmento *criar_segmento(Processo *processo);
void inserir_segmento_lista(Segmento **cabeca_segmento, Segmento *novo_segmento);
void remover_segmento_da_lista(Segmento **cabeca_segmento, Segmento *segmento_a_remover);
void remover_processo(int pid, Segmento **cabeca_segmento, Vetor_tabela_pag *paginas, Segmento **remover_seg);
void swap_out(Segmento *segmento, Segmento **cabeca_segmento, FILE *fp, Vetor_tabela_pag *paginas, Segmento **remover_seg);
void segunda_chance(Segmento **cabeca_segmento, Vetor_tabela_pag *paginas, Segmento **remover, FILE *fp);
int finalizar_carregamento_memoria(Processo *processo, Segmento **cabeca, Vetor_tabela_pag *paginas);
void imprimir_segmento(Segmento *cabeca_seg, Vetor_tabela_pag *paginas);

#endif
