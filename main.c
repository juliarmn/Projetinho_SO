// #include "Interface/interface.h"
#include "escalonador/rr.h"
#include "memoria/memoria.h"
#include "processo/processo.h"
#include "semaforo/semaforo.h"
#include "disco/disco.h"
#include "ES/imprimir.h"
#include <semaphore.h>
int relogio = 0;
pthread_cond_t condicao_interrupcao = PTHREAD_COND_INITIALIZER;
sem_t ler;
int flag_interrupcao = 0;
int imprime_robin = 0;

void imprime_logo()
{
    system("clear");
    printf("\033[38;5;206m");
    printf("\n\n");
    printf("\t\t\t\t\t /\\_/\\  \n");
    printf("\t\t\t\t\t( o.o ) \n");
    printf("\t\t\t\t\t > ^ < \n");
    printf("\033[38;5;21m");
    printf("\t\t ______                               _____     _____ \n");
    printf("\t\t/ _____|     _     _______   _   _   / ___ \\   / ____|\n");
    printf("\t\t| |  __     / \\   |__   __| | | | | | |   | | ( (____ \n");
    printf("\t\t| | |_ |   / _ \\     | |    | | | | | |   | |  \\____ \\\n");
    printf("\t\t| |__| |  /  _  \\    | |    | |_| | | |___| |   ____) )\n");
    printf("\t\t\\______| /__/ \\__\\   |_|    \\_____/  \\_____/   |_____/\n");
    printf("\n\n");
    printf("\033[38;5;206m");
    printf("\t\t======================================================\n\n");
}

/**
 * Funcao de menu
 *
 * @return int opcao escolhida
 */
int menu()
{
    int op;

    imprime_logo();
    printf("\033[38;5;21m");
    printf("\t\t1)\033[0m Criar um novo processo\n");
    printf("\033[38;5;21m");
    printf("\t\t2)\033[0m Ver processos em execução\n");
    printf("\033[38;5;21m");
    printf("\t\t3)\033[0m Ver estado da memória\n");
    printf("\033[38;5;21m");
    printf("\t\t4)\033[0m Sair\n");
    printf("\t\tDigite a opcao: ");
    scanf("%d", &op);

    return op;
}

typedef struct
{
    Round_robin **cabeca_robin;
    Semaforo **cabeca_sem;
    Segmento **cabeca_seg;
    Segmento **remover;
    Vetor_tabela_pag *memoria;
    Disco **HD;
    Trilhas **atual;
    Print_request **cabeca_print;
} ThreadArgs;

void *round_robin_thread(void *arg)
{
    ThreadArgs *args = (ThreadArgs *)arg;
    Round_robin **cabeca_robin = args->cabeca_robin;
    Semaforo **cabeca_sem = args->cabeca_sem;
    Segmento **cabeca_segmento = args->cabeca_seg;
    Segmento **remover_seg = args->cabeca_seg;
    Vetor_tabela_pag *memoria = args->memoria;
    Disco **HD = args->HD;
    Trilhas **atual = args->atual;
    Print_request **cabeca_print = args->cabeca_print;
    while (!(*cabeca_robin) && !(*cabeca_sem))
        ;
    while (1)
    {
        while (!flag_interrupcao && cabeca_robin && cabeca_sem)
        {
            robin_robin_atende(cabeca_robin, cabeca_sem, memoria, cabeca_segmento, remover_seg, HD, atual, cabeca_print);
        }
    }

    return NULL;
}

int main()
{
    sem_init(&ler, 1, 1);
    pthread_t thread;
    pthread_attr_t thread_atributo;
    pthread_attr_init(&thread_atributo);
    pthread_attr_setscope(&thread_atributo, PTHREAD_SCOPE_SYSTEM);

    char tecla;

    FILE *fp;
    char nome_arq[200];

    ThreadArgs thread_argumento;

    Segmento *cabeca_segmento = NULL, *remover = NULL;
    Vetor_tabela_pag *memoria = malloc(NUM_TOTAL_PAG * sizeof(Vetor_tabela_pag));
    inicializar_pag(memoria);

    Semaforo *cabeca_semaforo = NULL, *rabo_semaforo = NULL;

    Processo *processo = NULL;

    Round_robin *cabeca_robin = NULL;

    Disco *HD = malloc(sizeof(Disco));
    Trilhas *atual = NULL;
    HD->cabeca_trilhas = NULL;
    HD->fila = NULL;
    HD->tam_fila = 0;
    HD->ultima_trilha = 0;

    Print_request *cabeca_print = NULL;

    int op, carregou_memoria;
    int existe_memoria;

    thread_argumento.cabeca_robin = &cabeca_robin;
    thread_argumento.cabeca_sem = &cabeca_semaforo;
    thread_argumento.cabeca_seg = &cabeca_segmento;
    thread_argumento.remover = &remover;
    thread_argumento.memoria = memoria;
    thread_argumento.HD = &HD;
    thread_argumento.cabeca_print = &cabeca_print;
    thread_argumento.atual = &atual;

    if (pthread_create(&thread, &thread_atributo, round_robin_thread, &thread_argumento) != 0)
    {
        printf("\033[38;5;196m");
        printf("\n\t\t\033[6;1mNÃO CRIOU A THREAD\033[0m\n");
        sleep(1);
        system("clear");
        return 1;
    }

    iniciar_disco(&HD, &atual);
    do
    {
        op = menu();
        if (op > 4 || op < 1)
        {
            printf("\033[38;5;196m");
            printf("\n\t\t\033[6;1mDIGITE UMA OPÇÃO VÁLIDA\033[0m\n");
            sleep(1);
            system("clear");
        }

        switch (op)
        {
        case 1:
        {
            if (cabeca_robin != NULL)
                flag_interrupcao = 1;
            imprime_logo();
            printf("\t\t\033[38;5;93mDigite o nome do arquivo: \033[0m");
            scanf("%s", nome_arq);
            tecla = getchar();
            fp = abrir_arquivo(nome_arq);

            if (fp)
            {
                processo = criar_processo(fp, processo);
                existe_memoria = buscar_seg(processo, cabeca_segmento);
                if (existe_memoria)
                {
                    printf("\033[38;5;196m");
                    printf("\n\t\t\033[6;1mO PROCESSO JÁ EXISTE NA MEMÓRIA (PIDS IGUAIS)\033[0m\n");
                    sleep(1);
                    system("clear");
                    break;
                }
                if (((NUM_TOTAL_PAG * TAMANHO_PAGINA) / KILOBYTE) < processo->tam)
                {
                    printf("\033[38;5;196m");
                    printf("\n\t\t\033[6;1mSEGMENTO MAIOR QUE A MEMÓRIA\033[0m\n");
                    sleep(1);
                    system("clear");
                    break;
                }
                carregou_memoria = finalizar_carregamento_memoria(processo, &cabeca_segmento, memoria);
                if (!cabeca_segmento->prox)
                    remover = cabeca_segmento;
                while (!carregou_memoria && cabeca_segmento)
                {
                    segunda_chance(&cabeca_segmento, memoria, &remover, fp);
                    carregou_memoria = finalizar_carregamento_memoria(processo, &cabeca_segmento, memoria);
                }
                inserir_semaforo(&cabeca_semaforo, &rabo_semaforo, processo);
                inserir_round_robin(&cabeca_robin, processo);
                imprime_logo();
                imprimir_processo(processo);
                flag_interrupcao = 0;
            }
            else
            {
                printf("\033[38;5;196m");
                printf("\n\t\t\033[6;1mNOME INVÁLIDO\033[0m\n");
                sleep(1);
                system("clear");
            }
            break;
        }
        case 2:
        {
            tecla = getchar();

            imprime_logo();
            printf("\033[0m");
            printf("\n\t\tPRESSIONE ENTER PARA VOLTAR\033[0m\n");
            sem_wait(&ler);
            imprime_robin = 1;
            sem_post(&ler);
            tecla = getchar();
            if (tecla == '\n')
            {
                sem_wait(&ler);
                imprime_robin = 0;
                sem_post(&ler);
            }
            break;
        }
        case 3:
        {
            tecla = getchar();
            imprime_logo();
            imprimir_segmento(cabeca_segmento, memoria);
            break;
        }
        case 4:
        {
            system("clear");
            printf("\033[38;5;206m");

            printf("\t\t                   ####        ####                \n");
            printf("\t\t                   ######    ######                \n");
            printf("\t\t                   ######    ######                \n");
            printf("\t\t                   ######    ######                \n");
            printf("\t\t                     ##        ##                  \n");
            printf("\t\t           ######                    ######        \n");
            printf("\t\t           ########                ########        \n");
            printf("\t\t           ######      ########    ########        \n");
            printf("\t\t           ######    ############    ######        \n");
            printf("\t\t                     ############                  \n");
            printf("\t\t                   ################                \n");
            printf("\t\t                 ####################              \n");
            printf("\t\t               ########################            \n");
            printf("\t\t               ########################            \n");
            printf("\t\t               ########################            \n");
            printf("\t\t                 ####################              \n");

            printf("\n\t\t\t\t\t\033[900;1mADEUS\033[0m\n");
            return 0;
        }
        default:
        {
            break;
        }
        }

    } while (op != 4);
    pthread_join(thread, NULL);
    pthread_exit(NULL);
    return 0;
}