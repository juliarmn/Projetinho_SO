#include "rr.h"
#include <semaphore.h>
extern int relogio;
extern pthread_mutex_t mutex_interrupcao;
extern int flag_interrupcao;
extern int imprime_robin;
extern sem_t ler;

// TENHO QUE ARRUMAR DURACAO ANTES DE RODAR
// Arrumar a atende

/**
 * @param Round_robin cabeca da lista de round robin
 * @param Processo elemento a inserir na lista
 *
 * Funcao para inserir processos por ordem de proridade na lista robin
 *
 * @return nada
 */
void inserir_round_robin(Round_robin **cabeca, Processo *elemento)
{
    Round_robin *novo = malloc(sizeof(Round_robin));
    novo->processo = malloc(sizeof(Processo));
    novo->processo->lista_instrucao = malloc(sizeof(Instrucao));
    Round_robin *aux, *aux_ant;

    // Atribuição
    strcpy(novo->processo->lista_sem, elemento->lista_sem);
    novo->processo->prioridade = elemento->prioridade;
    novo->processo->status = elemento->status;
    strcpy(novo->processo->nome, elemento->nome);
    novo->processo->tam = elemento->tam;
    novo->processo->pid = elemento->pid;
    novo->processo->criacao = elemento->criacao;
    novo->processo->cabeca_instrucao = elemento->cabeca_instrucao;

    elemento->lista_instrucao = elemento->cabeca_instrucao;
    while (elemento->lista_instrucao)
    {
        novo->processo->lista_instrucao->num = elemento->lista_instrucao->num;
        novo->processo->lista_instrucao->prox = elemento->lista_instrucao->prox;
        novo->processo->lista_instrucao->semaforo = elemento->lista_instrucao->semaforo;
        novo->processo->lista_instrucao->tipo = elemento->lista_instrucao->tipo;

        elemento->lista_instrucao = elemento->lista_instrucao->prox;
    }

    novo->processo->lista_instrucao = novo->processo->cabeca_instrucao;

    // Lista está vazia,
    if (!(*cabeca))
    {
        novo->prox = NULL;
        // Atualizando primeiro elemento da lista
        (*cabeca) = novo;

        return;
    }

    // Prioridade menor que o primeiro elemento da lista
    if ((*cabeca)->processo->prioridade >= elemento->prioridade)
    {
        novo->prox = (*cabeca);
        (*cabeca) = novo;
        return;
    }

    aux = (*cabeca);

    while (aux && aux->processo->prioridade <= elemento->prioridade)
    {
        aux_ant = aux;
        aux = aux->prox;
    }

    aux_ant->prox = novo;
    novo->prox = aux;
}

/**
 * @param Round_robin cabeca para o primeiro elemento da lista.
 * @param Processo elemento a ser removido da lista
 *
 *  Funcao para remover elementos da lista do escalonador
 *
 * @return 1 se a lista está vazia e 0 se ainda tem elementos
 */
int remover(Round_robin **cabeca, Processo *elemento)
{

    Round_robin *aux = (*cabeca);
    Round_robin *aux_ant = NULL;
    // O elemento é o único da lista, pois a cabeca não tem prox
    if (!(*cabeca)->prox)
    {
        (*cabeca) = NULL;
        printf("\033[1;33m\t\tO Processo %s foi atendido.\n", aux->processo->nome);

        free(aux->processo);
        free(aux);

        return 1;
    }

    // o processo a ser removido é o primeiro da lista
    if ((*cabeca)->processo->pid == elemento->pid)
    {
        (*cabeca) = (*cabeca)->prox;
    }
    else
    {

        while (aux->processo->pid != elemento->pid)
        {
            aux_ant = aux;
            aux = aux->prox;
        }
        aux_ant->prox = aux->prox;
    }
    printf("\033[1;33m\t\tO Processo %s foi atendido.\n", aux->processo->nome);
    aux->processo->status = terminado;

    free(aux->processo);
    free(aux);

    return 0;
}

// sE CHAMAR PROCESS CREATE TRAVA ela e chama a inserir de novo (interface)

/**
 * @param Round_robin ponteiro do auxiliar que está sendo enviado para atender semaforo - membro da lista robin
 * @param Semaforo cabeca da lista de semáforos
 * @param char tipo P ou tipo V
 *
 * Muda o status do processo, que estará em execução, contabilida o relogio e verifica qual semáforo chamar e tratar.
 * A resposta  indica se o semáforo foi travado ou não.
 *
 * @return Round_robin elemento da lista atualizado
 */
Round_robin *atender_semaforo(Round_robin *aux, Semaforo **cabeca_sem, char tipo)
{
    int semaforo_resp;
    Processo *processo;
    Semaforo *aux_sem = buscar_semaforo(aux->processo->lista_instrucao->semaforo, *cabeca_sem);
    aux->processo->status = 2;

    if (tipo == 'P')
    {
        semaforo_resp = semaforo_P(cabeca_sem, aux->processo->lista_instrucao->semaforo, aux->processo);

        if (semaforo_resp)
        {
            relogio += 200;
            aux->processo->status = 4;
            if (aux->processo->lista_instrucao->prox)
                aux->processo->lista_instrucao = aux->processo->lista_instrucao->prox;
        }
        else
        {
            aux->processo->status = 0;
        }

        return aux;
    }

    semaforo_resp = semaforo_V(cabeca_sem, aux->processo->lista_instrucao->semaforo);

    if (semaforo_resp)
    {
        relogio += 200;

        if (aux->processo->lista_instrucao->prox)
            aux->processo->lista_instrucao = aux->processo->lista_instrucao->prox;
        aux->processo->status = 4;
        if (aux_sem->cabeca_fila)
        {
            processo = remover_fila_espera(&aux_sem);
            while (aux->prox && aux->processo->pid != processo->pid)
                aux = aux->prox;
            return aux;
        }
    }
    else
    {
        aux->processo->status = 0;
    }

    return aux;
}

/**
 * @param Round_robin elemento da lista robin atendido
 * @param int quantum time do processo
 * @param Semaforo cabeca do semaforo
 * @param Fila_espera cabeca da fila
 * @param Fila_espera rabbo da fila
 * @param char tipo
 *
 * Verifica o quanto o processo usou do quantum time, se usou tudo ou ainda falta para passar para proxima instrucao,
 * sempre fazendo tratamento de semaforo.
 *
 * @return Round_robin elemento atendido
 */
Round_robin *atender_instrucao(Round_robin *atendido, int quantum, Semaforo **cabeca_sem)
{

    int tempo_restante = 0;
    atendido->processo->status = 2;

    if (atendido->processo->duracao <= 0)
        atendido->processo->duracao = atendido->processo->lista_instrucao->num;

    atendido->processo->duracao -= quantum;

    if (atendido->processo->duracao == 0)
    {
        relogio += quantum;
        atendido->processo->status = 4;
        if (atendido->processo->lista_instrucao->prox)
            atendido->processo->lista_instrucao = atendido->processo->lista_instrucao->prox;
    }
    else if (atendido->processo->duracao < 0)
    {
        if (flag_interrupcao == 1)
            return NULL;

        tempo_restante = -atendido->processo->duracao;

        atendido->processo->status = 4;
        if (atendido->processo->lista_instrucao->prox)
        {
            atendido->processo->lista_instrucao = atendido->processo->lista_instrucao->prox;

            atendido->processo->duracao = atendido->processo->lista_instrucao->num;
        }
        while (tempo_restante > 0)
        {
            if (flag_interrupcao == 1)
                return NULL;

            if (atendido->processo->lista_instrucao->tipo == 3)
            {
                atendido = atender_semaforo(atendido, cabeca_sem, 'P');
                if (flag_interrupcao == 1)
                    return NULL;
                tempo_restante -= 200;

                if (atendido->processo->status == 0)
                    return atendido;
            }

            else if (atendido->processo->lista_instrucao->tipo == 4)
            {
                atendido = atender_semaforo(atendido, cabeca_sem, 'V');
                if (flag_interrupcao == 1)
                    return NULL;
                tempo_restante -= 200;

                if (atendido->processo->status == 0)
                    return atendido;
            }

            else if (tempo_restante - atendido->processo->duracao < 0)
            {
                if (flag_interrupcao == 1)
                    return NULL;
                atendido->processo->duracao = atendido->processo->duracao - tempo_restante;
                atendido->processo->status = 4;
                relogio += tempo_restante;
                tempo_restante = 0;
                break;
            }
            else
            {
                if (flag_interrupcao == 1)
                    return NULL;
                atendido->processo->status = 4;
                if (atendido->processo->duracao != -1)
                    tempo_restante -= atendido->processo->duracao;

                relogio += atendido->processo->duracao;
                if (atendido->processo->lista_instrucao->prox)
                    atendido->processo->lista_instrucao = atendido->processo->lista_instrucao->prox;
                else
                {
                    return atendido;
                }
            }
            atendido->processo->duracao = atendido->processo->lista_instrucao->num;
        }
    }
    else
    {
        tempo_restante = 0;
        relogio += quantum;
    }
    atendido->processo->status = 4;
    //sleep(2);
    return atendido;
}

/**
 * @param Round_robin cabeca do round robin
 * @param Semaforo cabeca do semaforo
 * @param Fila_espera cabeca da fila de espera
 * @param Fila_espera rabo da fila de espera
 *
 * Calcula o quantum time e verifica o tipo de instrucao a ser atendida, se acabarem as instrucoes do processo, ele é
 * retirado do round_robin
 *
 * @return void
 */
void robin_robin_atende(Round_robin **cabeca, Semaforo **cabeca_sem)
{
    if (!(*cabeca) && !(*cabeca_sem))
        return;
    Round_robin *aux = malloc(sizeof(Round_robin));
    aux->prox = (*cabeca)->prox;
    aux->processo = (*cabeca)->processo;
    Round_robin *aux_prox_pos_remocao;
    int lista_vazia = 0;
    int quantum_processo = 0;

    while (!lista_vazia)
    {
        sleep(1);
        sem_wait(&ler);
        if (imprime_robin == 1)
        {
            print_lista_robin(aux);
        }
        sem_post(&ler);

        if (quantum_processo <= 0)
            quantum_processo = QUANTUM / aux->processo->prioridade;
        if (aux->processo->lista_instrucao->tipo == 0 && aux->processo->status != 0)
        {
            aux = atender_instrucao(aux, quantum_processo, cabeca_sem);
            if (flag_interrupcao == 1)
                return;
            if (aux->processo->status == 0)
            {
                quantum_processo = QUANTUM / aux->processo->prioridade;
                aux->processo->status = 4;
            }
            else
                quantum_processo = 0;
        }
        else if ((aux->processo->lista_instrucao->tipo == 1 || aux->processo->lista_instrucao->tipo == 2 ||
                  aux->processo->lista_instrucao->tipo == 5) &&
                 aux->processo->status != 0)
        {
            aux = atender_instrucao(aux, quantum_processo, cabeca_sem);
            if (flag_interrupcao == 1)
                return;
            if (aux->processo->status == 0)
            {
                quantum_processo = QUANTUM / aux->processo->prioridade;
                aux->processo->status = 4;
            }
            else
                quantum_processo = 0;
        }
        else if (aux->processo->lista_instrucao->tipo == 3 && aux->processo->status != 0)
        {
            aux = atender_semaforo(aux, cabeca_sem, 'P');
            if (flag_interrupcao == 1)
                return;
            if (quantum_processo >= 200 && aux->processo->status != 0)
                quantum_processo -= 200;
            else
                quantum_processo = 0;
        }
        else if (aux->processo->lista_instrucao->tipo == 4 && aux->processo->status != 0)
        {
            aux = atender_semaforo(aux, cabeca_sem, 'V');
            if (flag_interrupcao == 1)
                return;
            if (quantum_processo >= 200 && aux->processo->status != 0)
                quantum_processo -= 200;
            else
                quantum_processo = 0;
        }
        if (!aux->processo->lista_instrucao->prox)
        {
            if (aux->prox)
                aux_prox_pos_remocao = aux->prox;
            else
                aux_prox_pos_remocao = (*cabeca);
            lista_vazia = remover(cabeca, aux->processo);
            aux = aux_prox_pos_remocao;

            if (flag_interrupcao == 1)
                return;
        }
        else
        {
            if (aux->prox && (quantum_processo <= 0 || aux->processo->status == 0))
                aux = aux->prox;
            else if (!aux->prox && (quantum_processo <= 0 || aux->processo->status == 0))
                aux = *cabeca;
            if (flag_interrupcao == 1)
                return;
        }
    }

    //pthread_mutex_lock(&mutex_interrupcao);
    flag_interrupcao = 1;
    //pthread_mutex_unlock(&mutex_interrupcao);
}

void print_lista_robin(Round_robin *robin)
{
    if (robin->processo->status == 0) {
        printf("\t\t\033[1;33mprocesso %s blocked\n", robin->processo->nome);
        return;
    }
    if (!robin->processo)
    {
        printf("\033[38;5;196m");
        printf("\n\t\t\033[6;1mSEM PROCESSO\033[0m\n");
        sleep(1);
        system("clear");
        return;
    }
    printf("\033[38;5;206m");
    printf("\t\tPROCESSO EM EXECUÇÃO: \n");
    printf("\033[38;5;206m");
    printf("\t\tNome: \033[0m%s\n", robin->processo->nome);
    printf("\033[38;5;206m");
    printf("\t\tPID: \033[0m%d\n", robin->processo->pid);
    printf("\033[38;5;206m");
    printf("\t\tPrioridade: \033[0m%d\n", robin->processo->prioridade);
    printf("\033[38;5;93m");
    printf("\n\t\t/====================/\n");
}
