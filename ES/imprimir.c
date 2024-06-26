#include "imprimir.h"
extern int relogio;

/**
 * @param Print_request ** cabeca da lista de request
 * @param int tempo de impressao
 * @param Processo * processo da impressao
 * 
 * Insere na lista de request (fila prioritaria pelo tempo - menor primeiro)
 * 
 * @return void
 * */
void inserir_lista(Print_request **cabeca_print, int tempo, Processo *processo)
{
    Print_request *novo = malloc(sizeof(Print_request));
    novo->processo = processo;
    novo->tempo = tempo;

    if (!(*cabeca_print))
    {
        *cabeca_print = novo;
        novo->prox = NULL;
        return;
    }

    if (tempo < (*cabeca_print)->tempo)
    {
        novo->prox = *cabeca_print;
        *cabeca_print = novo;
        return;
    }

    Print_request *aux = *cabeca_print;
    Print_request *aux_ant = aux;

     while (aux->prox && aux->prox->tempo < tempo)
    {
        aux = aux->prox;
    }

    novo->prox = aux->prox;
    aux->prox = novo;
}

/**
 * @param Print_request ** cabeca da lista de request
 * 
 * Atende o prioritário - primeiro
 * 
 * @return elemento atendido
*/
Print_request *atender_lista(Print_request **cabeca_print)
{
    Print_request *aux = *cabeca_print;
    (*cabeca_print) = (*cabeca_print)->prox;

    return aux;
}

/**
 * @param Print_request ** cabeca da lista de request
 * @param int tempo de impressao
 * @param Processo * processo para entrar na lista
 * 
 * Se não tiver ninguem imprime, caso contrario coloca na lista
 * 
 * @return void
*/
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
    print_finish(cabeca_print, processo);
}

// Pensar em como interromper
/**
 * @param Print_request ** cabeca da lista de request
 * @param Processo * processo para entrar na lista
 * 
 * Enquanto tiver request de impressao atende
 * 
 * @return void
*/
void print_finish(Print_request **cabeca_print, Processo *processo)
{
    Print_request *aux;
    while ((*cabeca_print))
    {
        aux = atender_lista(cabeca_print);
        printf("Processo %s e PID %d impresso em %d unidade de tempo.\n", aux->processo->nome, aux->processo->pid, aux->tempo);
        free(aux);
    }
}