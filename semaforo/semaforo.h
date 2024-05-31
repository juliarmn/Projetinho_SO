#ifndef SEMAFORO_H
#define SEMAFORO_H
#include "../memoria/memoria.h"
#include "../processo/processo.h"

// ======= STRUCTS ========

// Fila de espera dos processos a executar
typedef struct fila_espera
{
    Processo *fila_espera;
    struct fila_espera *prox;
} Fila_espera;

// Lista dos semaforos
typedef struct Lista_sem
{
    char semaforo_id;
    int id_processo;
    int ocupado;
    Fila_espera *fila_espera;
    Fila_espera *cabeca_fila;
    int tamanho_fila_espera;
    struct Lista_sem *prox;
} Semaforo;

// =========================================

// ======= FUNCOES =======

Semaforo *inicializa_semaforo(char c);
void insere_fila_espera(Semaforo **semaforo, Processo *processo);
Processo *remover_fila_espera(Semaforo **semaforo);
int semaforo_P(Semaforo **semaforo_cabeca, char id, Processo *processo);
int semaforo_V(Semaforo **semaforo_cabeca, char id);
Semaforo *buscar_semaforo(char semaforo_id, Semaforo *cabeca);
void inserir_semaforo(Semaforo **cabeca, Semaforo **rabo, Processo *processo);
void remover_semaforo(char id, Semaforo **cabeca, Semaforo **rabo);

#endif
