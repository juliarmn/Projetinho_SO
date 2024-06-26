#include "imprimir.h"
extern int relogio;

void inserir_lista(Print_request **cabeca_print, int tempo, Processo *processo)
{
    Print_request *novo = malloc(sizeof(Print_request));
    novo->processo = processo;
    novo->tempo = tempo;

    if (!(*cabeca_print))
    {
        *cabeca_print = novo;
        novo->prox = NULL;
    }

    if (tempo < (*cabeca_print)->tempo)
    {
        novo->prox = *cabeca_print;
        *cabeca_print = novo;
        return;
    }

    Print_request *aux = *cabeca_print;
    Print_request *aux_ant = aux;

    while (aux->prox)
    {
        if (aux->tempo >= tempo)
        {
            novo->prox = aux;
            aux_ant->prox = novo;
            return;
        }
        aux_ant = aux;
        aux = aux->prox;
    }

    aux->prox = novo;
}

Print_request *atender_lista(Print_request **cabeca_print)
{
    Print_request *aux = *cabeca_print;
    (*cabeca_print) = (*cabeca_print)->prox;

    return aux;
}

void print_request(Print_request **cabeca_print, int tempo, Processo *processo)
{
    int print = tempo / 1000;
    if (!(*cabeca_print))
    {
        sleep(print);
        printf("Processo %s e PID %d impresso em %d unidade de tempo.\n", processo->nome, processo->pid, tempo);
        return;
    }

    inserir_lista(cabeca_print, tempo, processo);
}

// Pensar em como interromper
void print_finish(Print_request **cabeca_print, int tempo, Processo *processo)
{
    Print_request *aux;
    while ((*cabeca_print))
    {
        aux = atender_lista(cabeca_print);
        printf("Processo %s e PID %d impresso em %d unidade de tempo.\n", aux->processo->nome, aux->processo->pid, tempo);
    }
}