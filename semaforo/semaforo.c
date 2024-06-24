#include "semaforo.h"
#include <semaphore.h>
/**
 * @param char caracteres do semaforo
 *
 * Apenas inicializa os dados de semaforo
 *
 * @return Semaforo atualizado
 */
Semaforo *inicializa_semaforo(char c)
{
    Semaforo *semaforo = malloc(sizeof(Semaforo));
    semaforo->semaforo_id = c;
    semaforo->ocupado = 0;
    semaforo->fila_espera = NULL;
    semaforo->cabeca_fila = NULL;
    semaforo->tamanho_fila_espera = 0;
    return semaforo;
}

/**
 * @param Semaforo que vai ser inserido algo na fila de espera
 * @param Processo que vai ser inserido na fila
 *
 * Procura o final da fila e insere
 *
 * @return void
 */
void insere_fila_espera(Semaforo **semaforo, Processo *processo)
{
    Fila_espera *novo = malloc(sizeof(Fila_espera));
    (*semaforo)->fila_espera = (*semaforo)->cabeca_fila;
    novo->fila_espera = processo;
    novo->prox = NULL;
    // Fila vazia
    if (!(*semaforo)->cabeca_fila)
    {
        (*semaforo)->cabeca_fila = novo;
        (*semaforo)->fila_espera = (*semaforo)->cabeca_fila;
        return;
    }

    while ((*semaforo)->fila_espera->prox)
        (*semaforo)->fila_espera = (*semaforo)->fila_espera->prox;
    (*semaforo)->fila_espera->prox = novo;
}

/**
 * @param Semaforo semaforo a ser removido o primeiro da fila
 *
 * Remove o elemento da fila de espera quando liberar o semaforo é chamada.
 *
 * @return Processo removido
 */
Processo *remover_fila_espera(Semaforo **semaforo)
{
    Fila_espera *aux = (*semaforo)->cabeca_fila;
    // Verificar se é o unico da fila
    if (!(*semaforo)->cabeca_fila)
    {
        return NULL;
    }

    (*semaforo)->cabeca_fila = (*semaforo)->cabeca_fila->prox;
    return aux->fila_espera;
}

/**
 * @param Semaforo cabeca da lista de semaforos
 * @param char id do semaforo
 * @param Processo processo que vai adentrar o semaforo
 *
 * Verifica se o semaforo esta em uso, se estiver coloca o processo na lista de espera
 *  e se nao estiver insere o processo no semaforo
 *
 * @return int indica se houve ou nao ocupacao do semaforo
 */
int semaforo_P(Semaforo **semaforo_cabeca, char id, Processo *processo)
{
    Semaforo *aux = buscar_semaforo(id, *semaforo_cabeca);
    sem_t P;
    sem_init(&P, 1, 1);

    if (!aux->ocupado)
    {
        sem_wait(&P);
        aux->id_processo = processo->pid;
        aux->ocupado = 1;
        sem_post(&P);
        return 1;
    }

    // semaforo está ocupado
    insere_fila_espera(&aux, processo);
    aux->tamanho_fila_espera++;
    return 0;
}

/**
 * @param Semaforo cabeca da lista de semaforo
 * @param char id do semaforo
 *
 * Verifica se o semaforo está ocupado, se estiver o libera
 *
 * @return int 1 se estava ocupado e 0 se já estava desocupado
 */
int semaforo_V(Semaforo **semaforo_cabeca, char id)
{
    Semaforo *aux = buscar_semaforo(id, *semaforo_cabeca);
    sem_t V;
    sem_init(&V, 1, 1);

    if (aux->ocupado)
    {
        sem_wait(&V);
        aux->ocupado = 0;
        sem_post(&V);
        return 1;
    }

    return 0;
}

/**
 * @param char id do semaforo
 * @param Semaforo cabeca da lista de semaforo
 *
 * Percore a lista e devolve o semaforo com o id informado
 *
 * @return Semaforo encontrado
 */
Semaforo *buscar_semaforo(char semaforo_id, Semaforo *cabeca)
{
    while (cabeca)
    {
        if (semaforo_id == cabeca->semaforo_id)
            return cabeca;

        cabeca = cabeca->prox;
    }

    return NULL;
}

/**
 * @param Semaforo cabeca da lista de semaforo
 * @param Semaforo rabo da lista de semaforo
 * @param Processo que contem os semaforos a serem incluidos
 *
 * Funcao que verifica a lista de semaforos presentes nos processos e inicializa eles, verificando se tem repeticao
 * Insere sempre no final da lista
 *
 * @return void
 */
void inserir_semaforo(Semaforo **cabeca, Semaforo **rabo, Processo *processo)
{
    int i = 0;
    Semaforo *aux;
    while (processo->lista_sem[i] != '\0')
    {
        aux = buscar_semaforo(processo->lista_sem[i], *cabeca);
        if (aux)
        {
            printf("O semaforo já está na lista\n");
        }
        else
        {
            if (!(*cabeca))
            {
                (*cabeca) = (*rabo) = inicializa_semaforo(processo->lista_sem[i]);
                (*cabeca)->prox = NULL;
            }
            else
            {
                aux = inicializa_semaforo(processo->lista_sem[i]);
                aux->prox = NULL;
                (*rabo)->prox = aux;
                (*rabo) = aux;
            }
        }
        i++;
    }
}

/**
 * @param char id para fazer a busca pela funcao de busca
 * @param Semaforo cabeca da lista de semaforo
 * @param Semaforo rabo da lista de semaforo
 *
 * Remove o semaforo quando necessario e verifica se pode ou nao remover (se tem algo na lista ou se o semaforo existe)
 * Retira da lista
 *
 * @return void
 */

/*
void remover_semaforo(char id, Semaforo **cabeca, Semaforo **rabo)
{

    Semaforo *aux = buscar_semaforo(id, (*cabeca));
    Semaforo *temp;
    if (aux)
    {
        if ((*cabeca)->semaforo_id == (*rabo)->semaforo_id)
        {
            (*cabeca) = (*rabo) = NULL;
            free(aux);
            return;
        }
        if ((*cabeca)->semaforo_id == aux->semaforo_id)
        {
            (*cabeca) = (*cabeca)->prox;
            free(aux);
            return;
        }
        temp = (*cabeca);

        while (temp->prox && temp->semaforo_id != id)
        {
            aux = temp;
            temp = temp->prox;
        }

        if (!temp->prox)
        {
            aux->prox = NULL;
            (*rabo) = aux;
            free(temp);
            return;
        }

        aux->prox = temp->prox;
        free(temp);
    }
    else
        printf("Semaforo nao existe!\n");
    return;
}*/
