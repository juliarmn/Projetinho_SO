#include "disco.h"
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>
extern int imprime_robin;
extern sem_t ler;

extern int relogio;
int direcao;
int flag_disco = 1;

typedef struct arg_disk
{
    Trilhas **atual;
    Disco **HD;
} Thread_disco;

void *disk_thread(void *arg)
{
    Thread_disco *args = (Thread_disco *)arg;
    Disco **HD = args->HD;
    Trilhas **atual = args->atual;

    while (((*HD)->cabeca_trilhas) == NULL)
        ;
    while (1)
    {
        while (flag_disco)
        {
            elevador(HD, atual);
        }
    }

    return NULL;
}

void iniciar_disco(Disco **HD, Trilhas **atual)
{
    pthread_t disco_thread;
    pthread_attr_t disco_thread_atributo;
    pthread_attr_init(&disco_thread_atributo);
    pthread_attr_setscope(&disco_thread_atributo, PTHREAD_SCOPE_SYSTEM);

    Thread_disco *disco = malloc(sizeof(Thread_disco));

    disco->HD = HD;
    disco->atual = atual;

    if (pthread_create(&disco_thread, &disco_thread_atributo, disk_thread, disco) != 0)
    {
        printf("\033[38;5;196m");
        printf("\n\t\t\033[6;1mNÃO CRIOU A THREAD PARA DISCO\033[0m\n");
        sleep(1);
        system("clear");
        return;
    }
}

/**
 * @param Processo * processo que chamou
 */
void disk_finish(Processo *processo)
{
    sem_wait(&ler);
    if (imprime_robin == 1)
    {
        printf("\033[38;5;206m");
        printf("\t\tFim da operacao de E/S para processo %s\n", processo->nome);
    }
    sem_post(&ler);

    processo->status = 1;
}

/**
 * @param char op 'r' ou 'w'
 * @param Disco
 * @param int num da trilha
 * @param Processo processo da request
 * @param Trilhas ** atual do elevador
 *
 * Calcula prioridade e chama para colocar na fila
 *
 * @return void
 */
void disk_request(char op, Disco *HD, int num_trilha, Processo *processo, Trilhas **atual)
{
    processo->status = 0;
    int prioridade;

    if (!(*atual))
    {
        if (op == 'w')
        {
            flag_disco = 0;
            inserir_trilha(num_trilha, &HD, processo);
            flag_disco = 1;
            *atual = HD->cabeca_trilhas;
            sem_wait(&ler);
            if (imprime_robin == 1)
            {
                printf("\033[38;5;206m");
                printf("\t\tFim da operacao de E/S para processo %s\n", processo->nome);
            }
            sem_post(&ler);

            processo->status = 1;
        }
        else
        {
            sem_wait(&ler);
            if (imprime_robin == 1)
            {
                printf("Não pode read\n");
            }
            sem_post(&ler);

            processo->status = 1;
            return;
        }
    }

    if ((*atual)->num_trilha < num_trilha && direcao == 1)
        prioridade = num_trilha - (*atual)->num_trilha;
    else if ((*atual)->num_trilha < num_trilha && direcao == 1)
        prioridade = (HD->ultima_trilha - (*atual)->num_trilha) + (HD->ultima_trilha - num_trilha);
    else if ((*atual)->num_trilha < num_trilha && direcao == 2)
        prioridade = (HD->ultima_trilha - (*atual)->num_trilha) + (HD->ultima_trilha - num_trilha);
    else if ((*atual)->num_trilha > num_trilha && direcao == 2)
        prioridade = (-1) * (num_trilha - (*atual)->num_trilha);

    inserir_fila_espera_disco(&HD, processo, num_trilha, prioridade, op);
}

/**
 * @param Disco * disco que verificamos
 * @param int numero da trilha
 *
 * Verifica se a trilha já existe, para não sobrescrever
 *
 * @return int 1 para sim 0 para nao
 */
int trilha_existe(Disco *HD, int num_trilha)
{
    if (!HD->cabeca_trilhas)
    {
        return 0;
    }

    Trilhas *aux = HD->cabeca_trilhas;

    while (aux)
    {
        if (aux->num_trilha == num_trilha)
        {
            return 1;
        }
        aux = aux->prox;
    }
    return 0;
}

/**
 * @param int numero da trilha que vai escrever
 * @param Disco
 * @param Processo * que vai ser escrito
 *
 * Insere na trilha a escrita
 *
 * @return void
 */
void inserir_trilha(int num_trilha, Disco **HD, Processo *processo)
{
    if (trilha_existe((*HD), num_trilha))
    {
        sem_wait(&ler);
        if (imprime_robin == 1)
        {
            printf("\t\tTrilha ocupada\n");
        }
        sem_post(&ler);
        return;
    }

    Trilhas *novo = malloc(sizeof(Trilhas));
    novo->num_trilha = num_trilha;
    novo->processo = processo;
    novo->ant = novo->prox = NULL;

    if (!(*HD)->cabeca_trilhas)
    {
        (*HD)->cabeca_trilhas = novo;
        (*HD)->ultima_trilha = novo->num_trilha;
        return;
    }

    if ((*HD)->cabeca_trilhas->num_trilha > num_trilha)
    {
        novo->prox = (*HD)->cabeca_trilhas;
        (*HD)->cabeca_trilhas->ant = novo;
        (*HD)->cabeca_trilhas = novo;
        return;
    }

    Trilhas *aux = (*HD)->cabeca_trilhas;
    while (aux->prox && aux->prox->num_trilha < num_trilha)
    {
        aux = aux->prox;
    }

    novo->prox = aux->prox;
    novo->ant = aux;

    if (aux->prox)
    {
        aux->prox->ant = novo;
    }
    aux->prox = novo;

    if (novo->prox == NULL)
    {
        (*HD)->ultima_trilha = novo->num_trilha;
    }
}

/**
 * @param Disco **
 * @param Processo * processo na fila
 * @param int num_trilha para operacao
 * @param int prioridade para entrar na fila - contrario, menor num, mais na frente fica
 * @param char op 'r' para read e 'w' para write
 *
 * Insere na fila de request do disco por prioridade
 *
 * @return void
 */
void inserir_fila_espera_disco(Disco **HD, Processo *processo, int num_trilha, int prioridade, char op)
{
    Fila_Request *novo = malloc(sizeof(Fila_Request));
    novo->num_trilha = num_trilha;
    novo->processo = processo;
    novo->op = op;
    novo->prioridade = prioridade;
    novo->prox = NULL;

    Fila_Request *aux, *aux_ant;

    if (!(*HD)->fila)
    {
        (*HD)->fila = novo;
        return;
    }

    if (prioridade < (*HD)->fila->prioridade)
    {
        novo->prox = (*HD)->fila;
        (*HD)->fila = novo;
        return;
    }

    aux = (*HD)->fila;
    aux_ant = NULL;

    while (aux != NULL && prioridade >= aux->prioridade)
    {
        aux_ant = aux;
        aux = aux->prox;
    }

    if (aux_ant != NULL)
    {
        aux_ant->prox = novo;
    }

    novo->prox = aux;
}

/**
 * @param Disco **
 *
 * Atende o primeiro
 *
 * @return Elemento atendido
 */
Fila_Request *atender_fila(Disco **HD, Trilhas **atual)
{
    Fila_Request *aux = (*HD)->fila;
    if (aux->op == 'w')
    {
        flag_disco = 0;
        inserir_trilha((*HD)->fila->num_trilha, HD, (*HD)->fila->processo);
        flag_disco = 1;
    }
    else
    {
        ler_processo((*HD), (*HD)->fila->num_trilha);
    }

    disk_finish((*HD)->fila->processo);
    (*HD)->fila = (*HD)->fila->prox;
    (*HD)->tam_fila--;

    return aux;
}

/**
 * @param int numero da trilha
 * @param Disco *HD
 *
 * Busca a trilha no disco
 *
 * @return *Trilha achada ou NULL se nao
 */
Trilhas *buscar_trilha(int num_trilha, Disco *HD)
{
    if (!(*HD).cabeca_trilhas)
    {
        return NULL;
    }

    while ((*HD).cabeca_trilhas)
    {
        if ((*HD).cabeca_trilhas->num_trilha == num_trilha)
        {
            return (*HD).cabeca_trilhas;
        }
        (*HD).cabeca_trilhas = (*HD).cabeca_trilhas->prox;
    }

    return NULL;
}

/**
 * @param Disco HD
 * @param int numero da trilha
 *
 * Le o processo no disco
 *
 * @return void
 */
void ler_processo(Disco *HD, int num_trilha)
{
    Processo *processo;
    Trilhas *trilha = buscar_trilha(num_trilha, HD);
    sem_wait(&ler);
    if (imprime_robin == 1)
    {
        printf("\033[38;5;206m");
        printf("\t\tInício da leitura\n");
    }
    sem_post(&ler);

    sleep(1);
    if (!trilha)
    {
        sem_wait(&ler);
        if (imprime_robin == 1)
        {
            printf("\t\t\033[6;1mNada na trilha\n");
        }
        sem_post(&ler);

        return;
    }

    processo = trilha->processo;

    sleep(2);
    sem_wait(&ler);
    if (imprime_robin == 1)
    {
        printf("\033[38;5;206m");
        printf("\t\tOperação de leitura do processo %s realizada\n", processo->nome);
    }
    sem_post(&ler);
}

/**
 * @param Disco**
 * @param Trilhas ** ponteiro atual
 *
 * Modifica o ponteiro atual e atende quando chega na requisicao
 *
 * @return void
 */
void elevador(Disco **HD, Trilhas **atual)
{
    direcao = 1; // 1 tá indo e 2 voltando
    while (1)
    {
        if (flag_disco == 0)
            return;
        sleep(1);

        if ((*HD)->fila && (*atual)->num_trilha == (*HD)->fila->num_trilha)
        {
            atender_fila(HD, atual);
        }

        if (!(*atual)->prox && !(*atual)->ant)
            continue;

        if (!(*atual)->prox)
        {
            direcao = 2;
        }

        if (!(*atual)->ant)
        {
            direcao = 1;
        }

        if (direcao == 1)
        {
            (*atual) = (*atual)->prox;
        }
        else
        {
            (*atual) = (*atual)->ant;
        }
    }
}
