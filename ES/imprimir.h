#ifndef IMPRIMIR_H
#define IMPRIMIR_H

#include "../processo/processo.h"

//======= STRUCTS =========
typedef struct impressao_lista {
   Processo *processo;
   int tempo;
   struct impressao_lista *prox;
} Print_request;

// =====================================================================
//======= FUNCOES =========
void inserir_lista(Print_request **cabeca_print, int tempo, Processo *processo);
Print_request *atender_lista(Print_request **cabeca_print);
void print_request(Print_request **cabeca_print, int tempo, Processo *processo);
void print_finish(Print_request **cabeca_print);
void iniciar_impressao(Print_request **cabeca_print);
void *ES_thread(void *arg);

#endif