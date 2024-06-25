#ifndef DISCO_H
#define DISCO_H

#include "../processo/processo.h"

typedef struct lista_trilhas {
    int num_trilha;
    Processo *processo;
    struct lista_trilhas *prox;
    struct lista_trilhas *ant;
} Trilhas;

typedef struct f{
    Processo *processo;
    int num_trilha;
    char op; 
    int prioridade;
    struct f *prox;
} Fila_Request;

typedef struct d {
    int em_uso;
    int tam_fila;
    int ultima_trilha;
    Trilhas *cabeca_trilhas;
    Fila_Request *fila;
} Disco;

int trilha_existe(Disco *HD, int num_trilha);
void inserir_trilha(int num_trilha, Disco **HD);
void inserir_fila_espera(Disco **HD, Processo *processo, int num_trilha);
Fila_Request *atender_fila(Disco **HD, int num_trilha);
Trilhas *buscar_trilha(int num_trilha, Disco *HD);
void ler_processo(Disco *HD, int num_trilha);

#endif  