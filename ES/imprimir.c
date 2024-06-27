#include "imprimir.h"
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>
extern int imprime_robin;
extern sem_t ler;
extern int relogio;
int flag_ES = 1;
int flag_inicio_thread = 0;

typedef struct arg_disk
{
    Print_request **cabeca_print;
} Thread_ES;

void *ES_thread(void *arg)
{
    Thread_ES *args = (Thread_ES *)arg;
    Print_request **cabeca_print = args->cabeca_print;
    while (!flag_inicio_thread)
        ;
    while (1)
    {
        while (flag_ES)
        {
            print_finish(cabeca_print);
        }
    }

    return NULL;
}

void iniciar_impressao(Print_request **cabeca_print)
{
    pthread_t es_thread;
    pthread_attr_t es_thread_atributo;
    pthread_attr_init(&es_thread_atributo);
    pthread_attr_setscope(&es_thread_atributo, PTHREAD_SCOPE_SYSTEM);

    Thread_ES *es = malloc(sizeof(Thread_ES));

    es->cabeca_print = cabeca_print;

    if (pthread_create(&es_thread, &es_thread_atributo, ES_thread, es) != 0)
    {
        printf("\033[38;5;196m");
        printf("\n\t\t\033[6;1mNÃO CRIOU A THREAD PARA DISCO\033[0m\n");
        sleep(1);
        system("clear");
        return;
    }
}

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
        flag_inicio_thread = 1;
        return;
    }

    if (tempo < (*cabeca_print)->tempo)
    {
        novo->prox = *cabeca_print;
        *cabeca_print = novo;
        flag_inicio_thread = 1;
        return;
    }

    Print_request *aux = *cabeca_print;

    while (aux->prox && aux->prox->tempo < tempo)
    {
        aux = aux->prox;
    }

    novo->prox = aux->prox;
    aux->prox = novo;
    flag_inicio_thread = 1;
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
    flag_ES = 0;
    inserir_lista(cabeca_print, tempo, processo);
    flag_ES = 1;
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
void print_finish(Print_request **cabeca_print)
{
    Print_request *aux;
    while ((*cabeca_print) && flag_ES)
    {
        aux = atender_lista(cabeca_print);
        aux->processo->status = 1;
        sem_wait(&ler);
        if (imprime_robin == 1)
        {
            printf("\033[38;5;206m");
            printf("\t\tProcesso %s e PID %d impresso em %d unidade de tempo.\n", aux->processo->nome, aux->processo->pid, aux->tempo);
        }
        sem_post(&ler);
        // free(aux);
    }
}