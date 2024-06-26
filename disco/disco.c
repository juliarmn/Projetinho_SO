#include "disco.h"

extern int relogio;
 int direcao;


void disk_finish(Processo *processo)
{
    printf("\033[38;5;206m");
    printf("Fim da operacao de E/S para processo %s\n", processo->nome);
    processo->status = 1;
}

void disk_request(char op, Disco *HD, int num_trilha, Processo *processo, Trilhas **atual)
{
    processo->status = 0;
    int prioridade;

    if (!(*atual)) {
        if (op == 'w') {
            inserir_trilha(num_trilha, &HD);
            *atual = HD->cabeca_trilhas;
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
    
    inserir_fila_espera(&HD, processo, num_trilha, prioridade, op);
}

int trilha_existe(Disco *HD, int num_trilha)
{
    // Testando tem trilhas
    if (!HD->cabeca_trilhas)
    {
        return 0;
    }

    Trilhas *aux = HD->cabeca_trilhas;

    // andando na lista e verificando se temos o elemento na trilha
    while (aux)
    {
        if (aux->num_trilha == num_trilha)
        {
            return 1;
        }
    }
    return 0;
}

void inserir_trilha(int num_trilha, Disco **HD)
{

    if (trilha_existe((*HD), num_trilha))
    {
        printf("Trilha ocupada\n");
        return;
    }

    Trilhas *novo = malloc(sizeof(Trilhas));
    novo->num_trilha = num_trilha;
    novo->processo->cabeca_instrucao = (*HD)->fila->processo;

    if (!(*HD)->cabeca_trilhas)
    {
        novo->ant = novo->prox = NULL;
        (*HD)->cabeca_trilhas = novo;
        (*HD)->ultima_trilha = novo->num_trilha;
        return;
    }

    // Se for menor que a cabeca
    if ((*HD)->cabeca_trilhas->num_trilha > num_trilha)
    {
        novo->ant = NULL;
        novo->prox = (*HD)->cabeca_trilhas;
        (*HD)->cabeca_trilhas->ant = novo;
        (*HD)->cabeca_trilhas = novo;
        return;
    }

    Trilhas *aux = (*HD)->cabeca_trilhas;
    // Se nao procura a posicao dele na lista
    while (aux->prox)
    {
        if (aux->num_trilha > novo->num_trilha)
        {
            novo->ant = aux->ant;
            novo->prox = aux;
            aux->ant->prox = novo;
            aux->ant = novo;
            return;
        }
        aux = aux->prox;
    }

    aux->prox = novo;
    novo->ant = aux;
    (*HD)->ultima_trilha = novo->num_trilha;
    novo->prox = NULL;
}

void inserir_fila_espera(Disco **HD, Processo *processo, int num_trilha, int prioridade, char op)
{
    Fila_Request *novo = malloc(sizeof(Fila_Request));
    novo->num_trilha = num_trilha;
    novo->processo = processo;
    novo->op = op;
    novo->prioridade = prioridade;
    Fila_Request *aux, *aux_ant;

    if (!(*HD)->fila) {
        (*HD)->fila = novo;
        (*HD)->fila->prox = NULL;
        return;
    } 

    if (prioridade < (*HD)->fila->prioridade) {
        novo->prox = (*HD)->fila;
        (*HD)->fila = novo;
        return;
    }

    aux = (*HD)->fila;
    aux_ant = aux;

    while (aux->prox)
    {
        if (prioridade < aux->prioridade) {
            aux_ant->prox = novo;
            novo->prox = aux;
            return;
        }

        aux_ant = aux;
        aux = aux->prox;
    }

    aux_ant->prox = novo;
    novo->prox = NULL;
    
}

Fila_Request *atender_fila(Disco **HD, int num_trilha)
{
    Fila_Request *aux = (*HD)->fila;

    if (aux->op == 'w')
    {
        inserir_trilha(num_trilha, (*HD));
    }
    else
    {
        ler_processo((*HD), num_trilha);
    }

    disk_finish((*HD)->fila->processo);
    (*HD)->fila = (*HD)->fila->prox;
    (*HD)->tam_fila--;

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

void ler_processo(Disco *HD, int num_trilha)
{
    Processo *processo;
    Trilhas *trilha = buscar_trilha(num_trilha, HD);
    printf("\033[38;5;206m");

    printf("Início da leitura\n");
    sleep(1);
    if (!trilha)
    {
        printf("\033[6;1mNada na trilha\n");
        return;
    }

    processo = trilha->processo;

    sleep(2);
    printf("\033[38;5;206m");
    printf("Operação de leitura do processo %s realizada\n", processo->nome);
}

void elevador(Disco **HD, Trilhas **atual)
{
    direcao = 1;// 1 tá indo e 2 voltando
    while (1)
    {
        sleep(1);
        if ((*atual)->num_trilha == (*HD)->fila->num_trilha)
        {
            atender_fila(HD, (*atual)->num_trilha);
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

