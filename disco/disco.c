#include "disco.h"

extern int relogio;

// Inserir a trilha na lista - nao tem apagar nem att ent ok - pft
// Ler trilha
// Inserir na fila de espera -ok
// Atender fila -ok

// funções print e arq
// Elevador

int buscar_trilha(Disco *HD, int num_trilha)
{
    // Testando tem trilhas
    if (!HD->cabeca_trilhas)
    {
        return 0;
    }

    Trilhas *aux = HD->cabeca_trilhas->prox;

    // Testando se ser a cabeca
    if (HD->cabeca_trilhas->num_trilha == num_trilha)
    {
        return 1;
    }

    // andando na lista e verificando se temos o elemento na trilha
    while (HD->cabeca_trilhas != aux)
    {

        if (aux->num_trilha == num_trilha)
        {
            return 1;
        }
    }
    return 0;
}

void inserir_trilha(Processo *processo, int num_trilha, Disco **HD)
{

    if (buscar_trilha((*HD), num_trilha))
    {
        printf("Trilha ocupada\n");
        return;
    }

    Trilhas *novo = malloc(sizeof(Trilhas));
    novo->num_trilha = num_trilha;
    novo->processo->cabeca_instrucao = processo;

    if (!(*HD)->cabeca_trilhas)
    {
        novo->ant = novo->prox = NULL;
        (*HD)->cabeca_trilhas = novo;
        return;
    }

    // So tem 1 elemento
    if (!(*HD)->cabeca_trilhas->prox)
    {
        (*HD)->cabeca_trilhas->prox = (*HD)->cabeca_trilhas->ant = novo;
        novo->ant = novo->prox = (*HD)->cabeca_trilhas;
    }

    // Se for menor que a cabeca
    if ((*HD)->cabeca_trilhas->num_trilha > num_trilha)
    {
        novo->ant = (*HD)->cabeca_trilhas->ant;
        novo->prox = (*HD)->cabeca_trilhas;
        (*HD)->cabeca_trilhas->ant->prox = novo;
        (*HD)->cabeca_trilhas->ant = novo;
        (*HD)->cabeca_trilhas = novo;
    }

    Trilhas *aux = (*HD)->cabeca_trilhas->prox;
    // Se nao procura a posicao dele na lista
    while (aux != (*HD)->cabeca_trilhas)
    {
        if (aux->num_trilha > novo->num_trilha)
        {
            novo->prox = aux;
            novo->ant = aux->ant;
            aux->ant->prox = novo;
            aux->ant = novo;
            return;
        }
        aux = aux->prox;
    }
}

void inserir_fila_espera(Disco **HD, Processo *processo, int num_trilha)
{
    Fila_Request *novo = malloc(sizeof(Fila_Request));
    novo->num_trilha = num_trilha;
    novo->processo = processo;

    if (!(*HD)->fila)
    {
        novo->prox = NULL;
        (*HD)->fila = novo;
        (*HD)->fila->tam++;
        return;
    }

    Fila_Request *aux = (*HD)->fila;

    while (aux->prox)
    {
        aux = aux->prox;
    }

    aux->prox = novo;
    (*HD)->fila->tam++;
    novo->prox = NULL;
}

Fila_Request *atender_fila(Disco **HD)
{
    Fila_Request *aux = (*HD)->fila;
    (*HD)->fila = (*HD)->fila->prox;
    (*HD)->fila->tam--;

    return aux;
}

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

int print_request(int num_trilha, Disco *HD)
{
    int funcionou;
    Trilhas *imprimir;
    imprimir = buscar_trilha(num_trilha, HD);
    int retorno;

    if (imprimir)
    {
        printf("\033[38;5;206m");
        printf("/----------------------/\nPrint da trilha %d\n", num_trilha);
        printf("\033[38;5;21m");
        printf("Nome processo:\033[0m %s\n", HD->cabeca_trilhas->processo->nome);
        printf("\033[38;5;21m");
        printf("Tamanho:\033[0m %d\n", HD->cabeca_trilhas->processo->tam);
        printf("\033[38;5;21m");
        printf("Prioridade processo:\033[0m %d\n", HD->cabeca_trilhas->processo->prioridade);
        printf("\033[38;5;206m");
        printf("/----------------------/\n");
    }
    retorno = printf_request_finish(imprimir);
    return retorno;
}

int printf_request_finish(Trilhas *imprimir)
{
    if (imprimir)
    {
        printf("\033[38;5;93m");
        printf("Fim da impressão\n");
        return 1;
    }
    else
    {
        printf("\033[38;5;196m");
        printf("Erro ao imprimir\n");
        return 0;
    }
}