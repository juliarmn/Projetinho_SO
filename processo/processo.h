#ifndef PROCESSO_H
#define PROCESSO_H
#include <locale.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ======= ENUM =======

// enum do estado do processo
typedef enum
{
    block,
    pronto,
    correndo,
    terminado,
    espera
} estadoProcesso;

// enum do tipo de instrucao
typedef enum
{
    EXEC,
    READ,
    WRITE,
    P,
    V,
    PRINT
} tipo_instrucao;

// =========================================

// ======= STRUCTS ========

// Struct de instrucao
typedef struct instrucao
{
    tipo_instrucao tipo;
    int num;
    char semaforo;
    struct instrucao *prox;
} Instrucao;

/*
typedef struct log
{
    char mensagem[1000];
    struct log *prox;
}log_processo;
*/

// Struct do estado do processo
typedef struct BCP
{
    estadoProcesso status;
    char nome[256];
    int pid;
    int prioridade;
    int criacao;
    char lista_sem[256];
    double tam;
    Instrucao *lista_instrucao;
    Instrucao *cabeca_instrucao;
    //log_processo *cabeca_log;
    int tam_lista_inst;
    int duracao;
    
} Processo;

// =========================================

// ======= FUNCOES =======

FILE *abrir_arquivo(char *nome);
Instrucao *inserir_lista_instrucoes(Instrucao **cabeca_inst);
Processo *criar_processo(FILE *fp, Processo *processo);
void imprimir_processo(Processo *processo);
//void adicionar_log(Processo *processo, char * mensagem);

#endif
