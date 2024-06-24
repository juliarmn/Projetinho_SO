#ifndef RR_H
#define RR_H
//#include "../Interface/interface.h"
#include "../processo/processo.h"
#include "../semaforo/semaforo.h"
#include <locale.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <pthread.h>

// Define do Quantum time padrao
#define QUANTUM 5000

// ====== STRUCTS =======

// Struct da lista do escalonador na CPU
typedef struct lista
{
    Processo *processo;
    struct lista *prox;
} Round_robin;

// ==================================

// ====== FUNCOES =======
void inserir_round_robin(Round_robin **cabeca, Processo *elemento);
int remover(Round_robin **cabeca, Processo *elemento, Vetor_tabela_pag *memoria, Segmento **cabeca_seg, Segmento **remover_seg);
Round_robin *atender_instrucao(Round_robin *atendido, int quantum, Semaforo **cabeca_sem);
Round_robin *atender_semaforo(Round_robin *atendido, Semaforo **cabeca_sem, char tipo);
void robin_robin_atende(Round_robin **cabeca, Semaforo **cabeca_sem, Vetor_tabela_pag *memoria, Segmento **cabeca_seg, Segmento **remover_seg);
void print_lista_robin(Round_robin *cabeca);

#endif
