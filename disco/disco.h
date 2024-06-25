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
    int tam;
    Processo *processo;
    int num_trilha;
    struct f *prox;
} Fila_Request;

typedef struct d {
    int em_uso;
    Trilhas *cabeca_trilhas;
    Fila_Request *fila;
} Disco;

int buscar_trilha(Disco *HD, int num_trilha);
void inserir_trilha(Processo *processo, int num_trilha, Disco **HD);

#endif  