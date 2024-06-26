#include "processo.h"
extern int relogio; // contagem de tempo de criacao do processo

/**
 * @param char* do arquivo
 *
 * Faz a abertura do arquivo de processo para leitura e escrita.
 *
 * @return arquivo aberto
 */
FILE *abrir_arquivo(char *nome)
{
    FILE *fp = fopen(nome, "rw");

    if (!fp)
    {
        return NULL;
    }

    return fp;
}

/**
 * @param Instrucao cabeca da lista de instrucao do processo
 *
 * Cria uma nova instrucao (malloca) e verifica pprimeiro se é vazia a lista, caso seja, atualiza a cabeca
 * Caso nao, roda um loop até achar o final da lista
 * Insere na lista
 *
 * @return processo criado
 */
Instrucao *inserir_lista_instrucoes(Instrucao **cabeca_inst)
{
    // Aloca espaco para instrucao
    Instrucao *novo = malloc(sizeof(Instrucao));
    Instrucao *aux;
    if (!(*cabeca_inst))
    {
        novo->prox = NULL;
        (*cabeca_inst) = novo;
        return (*cabeca_inst);
    }

    /// cria nova instrucao com malloc
    /// adiciona no final da lista
    aux = (*cabeca_inst);
    while (aux->prox)
        aux = aux->prox;

    aux->prox = novo;
    novo->prox = NULL;
    return novo;
}

/**
 * @param FILE arquivo do processo
 * @param Processo processo que será modificado
 *
 * Malloca um espaço para o processo e coloca o primeiro da lista de instrução como NULL
 * Faz a leitura de cada campo com cuidado especial para a lista de instrucoes, inserindo na lista ao chamar outra
 * funcao.
 *
 * @return processo alocado aqui
 */
Processo *criar_processo(FILE *fp, Processo *processo)
{
    processo = malloc(sizeof(Processo));
    processo->cabeca_instrucao = NULL;
    // processo->cabeca_log = NULL;
    Instrucao *aux = malloc(sizeof(Instrucao));

    int i = 0;
    char string_aux[10];
    fscanf(fp, "%s", processo->nome);
    fscanf(fp, "%d", &processo->pid);
    fscanf(fp, "%d", &processo->prioridade);
    fscanf(fp, "%lf", &processo->tam);
    processo->duracao = 0;

    char c = fgetc(fp);
    c = fgetc(fp);

    while (c != '\n')
    {
        processo->lista_sem[i] = c;
        fgetc(fp);
        c = fgetc(fp);
        i++;
    }

    while (!feof(fp))
    {
        fscanf(fp, "%s", string_aux);

        aux = inserir_lista_instrucoes(&processo->cabeca_instrucao);
        /// se começa com P ou V
        if (string_aux[0] == 'P' || string_aux[0] == 'V')
        {
            /// P(CHAR), o nome do semaforo está (c = fgetc(fp))na 2 posicao
            char sem = string_aux[2];
            aux->semaforo = sem;
            if (string_aux[0] == 'P')
                aux->tipo = 3;
            else
                aux->tipo = 4;
            aux->num = -1;
        }
        else
        {
            fgetc(fp);

            if (!strcmp(string_aux, "exec"))
                aux->tipo = 0;
            else if (!strcmp(string_aux, "write"))
                aux->tipo = 2;
            else if (!strcmp(string_aux, "read"))
                aux->tipo = 1;
            else if (!strcmp(string_aux, "print"))
                aux->tipo = 5;

            fscanf(fp, "%d", &aux->num);
            aux->semaforo = '\0';
        }

        processo->lista_instrucao = processo->cabeca_instrucao;
    }

    processo->criacao = relogio++;
    return processo;
}

/**
 * @param Processo processo
 *
 * Imprime o processo
 *
 * @return void
 */
void imprimir_processo(Processo *processo)
{
    char tecla;
    int qtd_inst = 0;

    if (!processo)
    {
        printf("\033[38;5;196m");
        printf("\n\t\t\033[6;1mSEM PROCESSO\033[0m\n");
        sleep(2);
        system("clear");
        return;
    }
    printf("\033[38;5;206m");
    printf("\t\tPROCESSO: \n");
    printf("\033[38;5;206m");
    printf("\t\tNome: \033[0m%s\n", processo->nome);
    printf("\033[38;5;206m");
    printf("\t\tPID: \033[0m%d\n", processo->pid);
    printf("\033[38;5;206m");
    printf("\t\tPrioridade: \033[0m%d\n", processo->prioridade);
    printf("\033[38;5;206m");
    printf("\t\tTamanho:\033[0m %.0lf\n", processo->tam);
    printf("\033[38;5;206m");
    if (processo->status == 0)
        printf("\t\tStatus: \033[0mblock\n");
    else if (processo->status == 1)
        printf("\t\tStatus: \033[0mpronto\n");
    else if (processo->status == 2)
        printf("\t\tStatus: \033[0mcorrendo\n");
    else if (processo->status == 3)
        printf("\t\tStatus: \033[0mterminado\n");
    else
        printf("\t\tStatus: \033[0mespera\n");
    printf("\033[38;5;206m");
    printf("\t\tCriação: \033[0m%d\n", processo->criacao);
    printf("\033[38;5;206m");
    printf("\t\tSemáforos: \033[0m");
    for (int i = 0; processo->lista_sem[i] != '\0'; i++)
    {
        printf("%c ", processo->lista_sem[i]);
    }
    printf("\033[38;5;93m");
    printf("\n\t\t/====================/\n");
    processo->lista_instrucao = processo->cabeca_instrucao;
    while (processo->lista_instrucao)
    {
        printf("\033[38;5;21m");

        printf("\t\tINSTRUÇÃO:\n");
        printf("\033[38;5;21m");
        if (processo->lista_instrucao->tipo == 0)
            printf("\t\tTipo:\033[0m exec %d\n", processo->lista_instrucao->num);
        if (processo->lista_instrucao->tipo == 1)
            printf("\t\tTipo:\033[0m read %d\n", processo->lista_instrucao->num);
        if (processo->lista_instrucao->tipo == 2)
            printf("\t\tTipo:\033[0m write %d\n", processo->lista_instrucao->num);
        if (processo->lista_instrucao->tipo == 3)
            printf("\t\tTipo:\033[0m P(%c)\n", processo->lista_instrucao->semaforo);
        if (processo->lista_instrucao->tipo == 4)
            printf("\t\tTipo:\033[0m V(%c)\n", processo->lista_instrucao->semaforo);
        if (processo->lista_instrucao->tipo == 5)
            printf("\t\tTipo:\033[0m print %d\n", processo->lista_instrucao->num);
        processo->lista_instrucao = processo->lista_instrucao->prox;
        printf("\033[38;5;93m");
        printf("\t\t=============\n");
        qtd_inst++;
    }
    printf("\033[38;5;206m");
    printf("\n\t\tQTD INST:\033[0m %d", qtd_inst);
    printf("\033[1;33m");
    processo->lista_instrucao = processo->cabeca_instrucao;
    printf("\033[0m");
    printf("\n\t\tPRESSIONE ENTER PARA VOLTAR\033[0m\n");
    while ((tecla = getchar()) != '\n')
        ;
}
