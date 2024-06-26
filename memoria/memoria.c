#include "memoria.h"
int id_seg = 0; // conta sequencialmente o id do segmento
int id_pag = 0; // conta sequencialmente o id do segmento

/**
 * @param nenhum
 * Inicializa a tabela de páginas e já coloca um id
 * Aloca espaço para tabela de paginas (memoria)
 *
 * @return tabela_pagina
 */
void inicializar_pag(Vetor_tabela_pag *tabela_paginas)
{
    tabela_paginas->qtd_pag_livre = NUM_TOTAL_PAG;
    for (int i = 0; i < NUM_TOTAL_PAG; i++)
    {
        tabela_paginas->tabela_pag[i].id = id_pag;
        id_pag++;
        tabela_paginas->tabela_pag[i].ocupada = 0;
    }
}

/**
 * @param Vetor_tabela_pag memoria (paginas)
 *
 * Encontra a página livre para inserir o processo
 *
 * @return o inteiro da posicao
 */
int achar_pag_livre(Vetor_tabela_pag *tabela_pagina)
{
    for (int i = 0; i < NUM_TOTAL_PAG; i++)
    {
        if (!tabela_pagina->tabela_pag[i].ocupada)
        {
            return i;
        }
    }
    return -1;
}

/**
 * @param Processo ponteiro do processo que vai entrar no segmento
 *
 * Copia o processo no segmento
 *
 * @return novo segmento criado
 */
Segmento *criar_segmento(Processo *processo)
{
    Segmento *novo_segmento = malloc(sizeof(Segmento));
    novo_segmento->processo = malloc(sizeof(Processo));
    novo_segmento->processo->lista_instrucao = malloc(sizeof(Instrucao));
    processo->lista_instrucao = processo->cabeca_instrucao;
    novo_segmento->id = id_seg;
    id_seg++;
    int i;

    novo_segmento->processo->criacao = processo->criacao;
    novo_segmento->processo->cabeca_instrucao = processo->cabeca_instrucao;

    strcpy(novo_segmento->processo->lista_sem, processo->lista_sem);
    strcpy(novo_segmento->processo->nome, processo->nome);
    novo_segmento->processo->pid = processo->pid;
    novo_segmento->processo->prioridade = processo->prioridade;
    novo_segmento->processo->status = processo->status;
    novo_segmento->processo->tam = processo->tam;
    novo_segmento->processo->duracao = processo->duracao;

    // Inicializando lista de indices livres
    for (i = 0; i < NUM_TOTAL_PAG; i++)
        novo_segmento->index_vet_pag[i] = -1;

    novo_segmento->dirty_bit = 0;
    novo_segmento->modificado = 0;

    novo_segmento->prox = NULL;
    novo_segmento->bit_presenca = 0;

    return novo_segmento;
}

/**
 * @param Segmento cabeca da lista
 * @param Segmento novo segmento
 *
 * Sempre insere o novo segmento no final da lista e cria a cabeca
 *
 * @return nada
 */
void inserir_segmento_lista(Segmento **cabeca_segmento, Segmento *novo_segmento)
{
    // Lista vazia, é o primeiro elemento a ser inserido
    if (!(*cabeca_segmento))
    {
        (*cabeca_segmento) = novo_segmento;
        (*cabeca_segmento)->prox = NULL;
        return;
    }

    Segmento *aux = (*cabeca_segmento);
    // Insere sempre no final
    while (aux->prox)
    {
        aux = aux->prox;
    }

    aux->prox = novo_segmento;
}

/**
 * @param Processo processo a ser inserido
 * @param Vetor_tabela_pag memoria
 * @param Segmento cabeca da lista de segmentos
 *
 * Função que verifica se tem espaço na memória, caso tenha cria um segmento para o processo e o insere nas paginas.
 *
 * @return segmento criado
 */
Segmento *inserir_processo_memoria(Processo *processo, Vetor_tabela_pag *tabela_paginas, Segmento **cabeca_segmento)
{
    // Primeiro verificar se tem paginas livre e o processo cabe na memória
    if (!tabela_paginas->qtd_pag_livre || (tabela_paginas->qtd_pag_livre * TAMANHO_PAGINA / KILOBYTE) < processo->tam)
    {
        return NULL;
    }

    int i = 0, qtd_pag = 0;
    int pag_livre;

    Segmento *novo_segmento = criar_segmento(processo);

    inserir_segmento_lista(cabeca_segmento, novo_segmento);

    int tamanho_seg = processo->tam;

    // Laco para verificar se o processo ocuparia mais de uma pagina e marcar elas como ocupadas
    while (tamanho_seg)
    {
        pag_livre = achar_pag_livre(tabela_paginas); // acha pagina livre (posicao no vetor) para inserir
        if (pag_livre != -1)
        {
            tabela_paginas->tabela_pag[pag_livre].ocupada = 1;
            if (tamanho_seg < TAMANHO_PAGINA / KILOBYTE) // Processo e menor que a pagina
            {
                tamanho_seg = 0;
                tabela_paginas->qtd_pag_livre--;
                qtd_pag++;
            }
            else // Maior ou igual a pagina
            {
                tamanho_seg -= TAMANHO_PAGINA / KILOBYTE;
                tabela_paginas->qtd_pag_livre--;

                qtd_pag++;
            }
            novo_segmento->index_vet_pag[i] = pag_livre;
            novo_segmento->quantidade_pag = qtd_pag;

            i++;
        }
    }
    return novo_segmento;
}

/**
 * @param Segmento cabeca da lista de segmentos
 * @param Segmento que sera removido
 *
 * Funcao que atualiza a lista de segmentos
 *
 * @return nada
 */
void remover_segmento_da_lista(Segmento **cabeca_segmento, Segmento *segmento_a_remover)
{
    Segmento *aux = (*cabeca_segmento), *aux_ant;

    // removendo o primeiro ou unico elemento da lista
    if ((*cabeca_segmento)->id == segmento_a_remover->id)
    {
        (*cabeca_segmento) = (*cabeca_segmento)->prox;
        free(aux);
        return;
    }
    // Andando na lista procurando o segmento a remover
    while (aux->prox && aux->id != segmento_a_remover->id)
    {
        aux_ant = aux;
        aux = aux->prox;
    }
    // O segmento a remover é o ultimo da lista
    if (!aux->prox)
    {
        aux_ant->prox = NULL;
        free(aux);
        return;
    }
    // O elemento está no meio da lista
    aux_ant->prox = aux->prox;
    free(aux);
}

/**
 * @param int PID do processo a ser removido
 * @param Segmento cabeca da lista de segmentos
 * @param Vetor_tabela_pag memoria (paginas)
 *
 * Função que procura as paginas do segmento para apagar elas e depois apagar o segmento, liberando espaco
 *
 * @return nada
 */
void remover_processo(int pid, Segmento **cabeca_segmento, Vetor_tabela_pag *paginas, Segmento **remover_seg)
{
    int i;
    Segmento *aux = (*cabeca_segmento), *aux_ant;

    while (aux)
    {
        // testando se o segmento é do processo a remover.
        if (pid == aux->processo->pid)
        {
            for (i = 0; aux->index_vet_pag[i] != -1; i++)
            {
                paginas->tabela_pag[i].ocupada = 0; // desocupa a pagina
                paginas->qtd_pag_livre++;
            }
            aux_ant = aux;
            if (aux->prox) {
                (*remover_seg) = aux;
            } else {
                remover_seg = cabeca_segmento;
            }
            remover_segmento_da_lista(cabeca_segmento, aux_ant);

            break;
        }
        aux = aux->prox;
    }
}

void swap_out(Segmento *segmento, Segmento **cabeca_segmento, FILE *fp, Vetor_tabela_pag *paginas, Segmento **remover_seg)
{
    // Se foi modificado em memória, copia no arquivo
    if (segmento->modificado)
    {
        fseek(fp, 0, SEEK_SET);
        fprintf(fp, "%s\n", segmento->processo->nome);
        fprintf(fp, "%d\n", segmento->processo->pid);
        fprintf(fp, "%d\n", segmento->processo->prioridade);
        fprintf(fp, "%.0f\n", segmento->processo->tam);
        for (int i = 0; segmento->processo->lista_sem[i] != '\0'; i++)
            fprintf(fp, "%c ", segmento->processo->lista_sem[i]);
        fprintf(fp, "\n");
        fprintf(fp, "\n");

        segmento->processo->lista_instrucao = segmento->processo->cabeca_instrucao;
        while (segmento->processo->lista_instrucao)
        {
            if (segmento->processo->lista_instrucao->tipo == 0)
                fprintf(fp, "exec %d\n", segmento->processo->lista_instrucao->num);
            else if (segmento->processo->lista_instrucao->tipo == 1)
                fprintf(fp, "read %d\n", segmento->processo->lista_instrucao->num);
            else if (segmento->processo->lista_instrucao->tipo == 2)
                fprintf(fp, "write %d\n", segmento->processo->lista_instrucao->num);
            else if (segmento->processo->lista_instrucao->tipo == 5)
                fprintf(fp, "print %d\n", segmento->processo->lista_instrucao->num);
            else if (segmento->processo->lista_instrucao->tipo == 3)
                fprintf(fp, "P(%c)\n", segmento->processo->lista_instrucao->semaforo);
            else
                fprintf(fp, "V(%c)\n", segmento->processo->lista_instrucao->semaforo);
            segmento->processo->lista_instrucao = segmento->processo->lista_instrucao->prox;
        }
    }
    remover_processo(segmento->processo->pid, cabeca_segmento, paginas, remover_seg);
}

/**
 * @param Segmento cabeca_segmento da lista de segmentos
 * @param Vetor_tabela_pag memoria (lista de paginas)
 * @param Segmento ponteiro do elemento a ser removido (proximo)
 * @param FILE fp - arquivo
 *
 * Verifica o dirty bit, se foi modificado, dá uma segunda chance.
 * Caso não, ele é removido
 * Quando remover sai da funcao (swap out)
 * Salva o ponteiro do proximo a ser removido
 *
 * @return nada
 */
void segunda_chance(Segmento **cabeca_segmento, Vetor_tabela_pag *paginas, Segmento **remover, FILE *fp)
{
    Segmento *aux = (*remover);

    while (1)
    {
        if (aux->dirty_bit == 1) // se tiver sido mexido em memória
        {
            aux->dirty_bit = 0;  // Dá outra chance para ele continuar na memoria
            aux->modificado = 1; // marca como modificado para copiar no arquivo
        }
        else // Nao foi modificao (remove ele)
        {
            swap_out(aux, cabeca_segmento, fp, paginas, remover);
            break;
        }

        if (aux->prox == NULL)
        {
            aux = (*cabeca_segmento);
        }
        else
        {
            aux = aux->prox;
        }
    }
}

/**
 * @param Processo processo a ser buscado na memoria
 * @param Segmento cabeca da lista de segmentos
 *
 * Busca o procesos na lista de segmento já existente na memória
 *
 * @return int existe ou nao
 */
int buscar_seg(Processo *processo, Segmento *cabeca_seg)
{
    if (!cabeca_seg)
        return 0;
    while (cabeca_seg)
    {
        if (processo->pid == cabeca_seg->processo->pid)
            return 1;
        cabeca_seg = cabeca_seg->prox;
    }
    return 0;
}

/**
 * @param Processo processo a ser inserido
 * @param Segmento cabeca da lista de segmentos
 * @param Vetor_tabela_pag paginas (memoria)
 *
 * Chama a funcao de inserçao na memoria
 * Arruma o status processo
 *
 * @return nada
 */
int finalizar_carregamento_memoria(Processo *processo, Segmento **cabeca, Vetor_tabela_pag *paginas)
{
    processo->status = 1;

    Segmento *segmento = inserir_processo_memoria(processo, paginas, cabeca);
    if (!segmento)
    {
        printf("\033[38;5;196m");
        printf("\n\t\t\033[6;1mO PROCESSO NÃO PODE SER INSERIDO - SEM ESPAÇO\033[0m\n");
        sleep(1);
        system("clear");
        return 0;
    }
    segmento->processo->status = 1;

    segmento->bit_presenca = 1;
    return 1;
}

/**
 * @param Segmento cabeca da lista de segmento
 * @param Vetor_tabela_pag paginas da memória
 *
 * Imprime dados do segmento
 */
void imprimir_segmento(Segmento *cabeca_seg, Vetor_tabela_pag *paginas)
{
    char tecla;
    if (!cabeca_seg)
    {
        printf("\033[38;5;196m");
        printf("\n\t\t\033[6;1mSEM SEGMENTO\033[0m\n");
        sleep(1);
        system("clear");
        return;
    }
    int qtd_seg = 0;
    while (cabeca_seg)
    {
        printf("\033[38;5;206m");
        printf("\t\tSEGMENTO: \n");
        printf("\033[38;5;206m");
        printf("\t\tID: \033[0m%d\n", cabeca_seg->id);
        printf("\033[38;5;206m");
        printf("\t\tPID processo: \033[0m%d\n", cabeca_seg->processo->pid);
        printf("\033[38;5;206m");
        printf("\t\tNome processo: \033[0m%s\n", cabeca_seg->processo->nome);
        printf("\033[38;5;206m");
        printf("\t\tTamanho:\033[0m %lf\n", cabeca_seg->processo->tam);
        printf("\033[38;5;206m");
        printf("\t\tQuantidade páginas:\033[0m %d\n", cabeca_seg->quantidade_pag);
        printf("\033[38;5;93m");
        printf("\t\t=======================\n");
        qtd_seg++;
        cabeca_seg = cabeca_seg->prox;
    }
    printf("\033[38;5;21m");
    printf("\t\tMemória tem %d páginas ocupadas e %d processos\n", (NUM_TOTAL_PAG - paginas->qtd_pag_livre), qtd_seg);
    printf("\033[38;5;21m");
    printf("\t\tMemória tem ocupação de %d KB\n", TAMANHO_PAGINA * (NUM_TOTAL_PAG - paginas->qtd_pag_livre));
    printf("\033[38;5;21m");
    printf("\t\tMemória tem %d pag livres\n", paginas->qtd_pag_livre);
    printf("\033[0m");
    printf("\n\t\tPRESSIONE ENTER PARA VOLTAR\033[0m\n");
    while ((tecla = getchar()) != '\n')
        ;
 
    system("clear");
}
