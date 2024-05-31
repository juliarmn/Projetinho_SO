#include "memoria.c"
#include "process.c"
#include <ncurses.h>
#include <string.h>

WINDOW *criar_win(int row, int col);
void draw_cat(int cursor_position);
void print_large_title();
void print_process(Processo *processo, WINDOW *win, int j, int flag);

void menu()
{
    char titulo[] = "GatuOS";
    int len = strlen(titulo);

    initscr();

    start_color();

    curs_set(0);

    init_color(3, 15 * 1000 / 255, 18 * 1000 / 255, 24 * 1000 / 255);
    init_color(4, 179 * 1000 / 255, 149 * 1000 / 255, 191 * 1000 / 255);

    init_pair(2, 4, 3);
    bkgd(COLOR_PAIR(2));

    int row, col;
    getmaxyx(stdscr, row, col);
    int cursor_position = (col / 2) - (len / 2);
    move(1, cursor_position);

    print_large_title();
    /*attron(COLOR_PAIR(2) | A_BOLD);
    printw("%s\n", titulo);
    attroff(COLOR_PAIR(2) | A_BOLD);*/

    move(3, cursor_position);
    draw_cat(cursor_position);
    refresh();

    WINDOW *menuwin = criar_win(row, col);

    /*
    WINDOW *menuwin = newwin(row/2, col/2, row/4, col/4);
    box(menuwin, 0, 0);

    wbkgd(menuwin, COLOR_PAIR(2));
    refresh();
    wrefresh(menuwin);
    */

    mvwprintw(menuwin, 0, 3, "Escolha uma opcao:");

    keypad(menuwin, true);
    char opcoes[4][50] = {"1 - Submeter novo processo.", "2 - Acompanhar dados dos processos em execucao.",
                          "3 - Acompanhar estado de ocupacao da memoria.", "4 - Sair."};

    int op;
    int highlight = 0;

    while (1)
    {
        curs_set(0);
        for (int i = 0; i < 4; i++)
        {
            if (i == highlight)
                wattron(menuwin, A_REVERSE);
            mvwprintw(menuwin, i + 1, 1, opcoes[i]);
            wattroff(menuwin, A_REVERSE);
        }
        op = wgetch(menuwin);

        switch (op)
        {
        case KEY_UP:
            highlight--;
            if (highlight == -1)
                highlight = 0;
            break;
        case KEY_DOWN:
            highlight++;
            if (highlight == 4)
                highlight = 3;
            break;
        case 10: // enter
            switch (highlight)
            {
            case 0:
                clear();
                delwin(menuwin);

                print_large_title();
                draw_cat(cursor_position);

                WINDOW *procwin = criar_win(row, col);
                mvwprintw(procwin, 0, 3, "Preencha os dados do processo:");
                mvwprintw(procwin, 26, 60, "Pressione ESC para voltar");
                mvwprintw(procwin, 1, 1, "Caminho do arquivo do processo: ");
                wrefresh(procwin);

                curs_set(1);
                noecho();

                char arq[200];
                int ch;
                int i = 0;
                int j = 2;

                while (1)
                {
                    ch = wgetch(procwin);
                    if (ch == 27)
                    { // ESC
                        break;
                    }
                    else if (ch == 10)
                    { // enter
                        curs_set(0);
                        arq[i] = '\0';

                        FILE *fp = abrir_arquivo(arq);
                        Processo *processo = NULL;

                        if (fp)
                        {
                            mvwprintw(procwin, j + 2, 1, "Arquivo aberto com sucesso!");
                            processo = criar_processo(fp, processo);
                            print_process(processo, procwin, j);
                            /*
                            mvwprintw(procwin, j+3, 1, "===== PROCESSO =====");
                            mvwprintw(procwin, j+4, 2, "Nome: %s", processo->nome);
                            mvwprintw(procwin, j+5, 2, "PID: %d", processo->pid);
                            mvwprintw(procwin, j+6, 2, "Prioridade: %d",
                            processo->prioridade); mvwprintw(procwin, j+7, 2, "Tamanho: %lf",
                            processo->tam); mvwprintw(procwin, j+8, 2, "Status: %d",
                            processo->prioridade); mvwprintw(procwin, j+9, 2, "Criação: %d",
                            processo->criacao); mvwprintw(procwin, j+10, 2, "Semáforos: ");

                            for (int i = 0; processo->lista_sem[i] != '\0'; i++)
                            {
                                mvwprintw(procwin, j+11, 3+i, "%c", processo->lista_sem[i]);
                                //mvwprintw(procwin, j+11, 4+i, " ");
                            }
                            mvwprintw(procwin, j+12, 1, "/===================/");

                            processo->lista_instrucao = processo->cabeca_instrucao;
                            while (processo->lista_instrucao)
                            {
                                mvwprintw(procwin, j+13, 7, "INSTRUCAO:");
                                mvwprintw(procwin, j+14, 2, "Tipo: %d",
                            processo->lista_instrucao->tipo); mvwprintw(procwin, j+15, 2,
                            "Número: %d", processo->lista_instrucao->num); mvwprintw(procwin,
                            j+16, 2, "Semaforo: %d", processo->lista_instrucao->semaforo);

                                processo->lista_instrucao = processo->lista_instrucao->prox;
                                mvwprintw(procwin, j+17, 1, "====================");
                                qtd_inst ++;
                            }
                            mvwprintw(procwin, j+18, 2, "QTD INST: %d", qtd_inst);
                            processo->lista_instrucao = processo->cabeca_instrucao;
                            */
                        }
                        else
                        {
                            mvwprintw(procwin, j + 2, 1, "Erro ao abrir arquivo.");
                        }

                        // mvwprintw(procwin, 3, 1, "Nome: %s", arq);
                        mvwprintw(procwin, 22, 1, "Pressione ENTER para concluir.");

                        wrefresh(procwin);
                        getch();
                        break;
                    }
                    else if (i < sizeof(arq) - 1)
                    {
                        arq[i++] = ch;
                        waddch(procwin, ch);
                        wrefresh(procwin);
                    }
                }

                echo();
                /*
                wgetnstr(procwin, arq, sizeof(arq) - 1);
                curs_set(0);

                mvwprintw(procwin, 2, 1, "Nome: %s", arq);
                wrefresh(procwin);
                getch();
                */

                delwin(procwin);

                print_large_title();
                draw_cat(cursor_position);

                menuwin = criar_win(row, col);
                mvwprintw(menuwin, 0, 3, "Escolha uma opcao:");
                keypad(menuwin, true);

                refresh();
                break;
            case 1:
                clear();
                delwin(menuwin);

                print_large_title();
                draw_cat(cursor_position);

                WINDOW *execwin = criar_win(row, col);
                mvwprintw(execwin, 0, 3, "Dados dos processos em execucao:");
                mvwprintw(execwin, 26, 58, "Pressione ENTER para voltar");
                wrefresh(execwin);

                Segmento *lista = NULL; // receber ponteiro pra cabeca da lista
                int z = 1;

                if (!lista)
                {
                    mvwprintw(execwin, 1, 1, "==== PROCESSO ====");
                    mvwprintw(execwin, 2, 2, "vazio.");
                }
                else
                {
                    while (lista)
                    {
                        if (lista->processo->status == 2)
                        { // se o processo estiver em execucao, printa
                            print_process(lista->processo, execwin, z, 0);
                        }
                    }
                }

                wrefresh(execwin);
                getch();

                delwin(execwin);
                menuwin = criar_win(row, col);
                mvwprintw(menuwin, 0, 3, "Escolha uma opcao:");
                keypad(menuwin, true);
                refresh();
                break;
            case 2:
                clear();
                delwin(menuwin);

                print_large_title();
                draw_cat(cursor_position);

                WINDOW *memwin = criar_win(row, col);
                mvwprintw(memwin, 0, 3, "Estado de ocupacao da memoria:");
                mvwprintw(memwin, 26, 52, "Pressione ESC para voltar ao menu");
                wrefresh(memwin);

                Segmento *lista_seg = NULL; // receber ponteiro pra cabeca da lista
                int k = 4;

                if (!lista_seg)
                {
                    mvwprintw(memwin, 1, 1, "==== SEGMENTO ====");
                    mvwprintw(memwin, 2, 2, "vazio.");
                }
                else
                {
                    while (lista_seg)
                    {
                        mvwprintw(memwin, 1, 1, "Pressione ENTER para avançar.");
                        mvwprintw(memwin, 2, 2, "SEGMENTO");
                        mvwprintw(memwin, 3, 2, "id: %d", lista_seg->id);
                        mvwprintw(memwin, 4, 2, "Presença: %d", lista_seg->bit_presenca);
                        mvwprintw(memwin, 5, 2, "Dirty: %d", lista_seg->dirty_bit);
                        mvwprintw(memwin, 6, 2, "Quantidade de paginas: %d", lista_seg->quantidade_pag);

                        for (int i = 0; i < NUM_TOTAL_PAG; i++)
                        {
                            mvwprintw(memwin, 6 + i, 2, "Vetor de pagina: [%d]\n", lista_seg->index_vet_pag[i]);
                        }
                        print_process(lista_seg->processo, memwin, k, 1);
                        // imprimir_processo(lista_seg->processo);
                        lista_seg = lista_seg->prox;
                        // mvwprintw(memwin, 6, 1, "/===================/");
                    }
                }

                wrefresh(memwin);
                getch(); // sai da tela

                delwin(memwin);
                menuwin = criar_win(row, col);
                mvwprintw(menuwin, 0, 3, "Escolha uma opcao:");
                keypad(menuwin, true);
                refresh();
                break;
            case 3:
                endwin();
                return 0;
            }
            break;
        default:
            break;
        }
    }

    printw("OP: %s", opcoes[highlight]);

    getch();

    endwin();
}

WINDOW *criar_win(int row, int col)
{
    WINDOW *win = newwin(row / 2, col / 2, row / 4, col / 4);
    box(win, 0, 0);

    wbkgd(win, COLOR_PAIR(2));
    refresh();
    wrefresh(win);
    return win;
}

void draw_cat(int cursor_position)
{
    int j = 3;
    const char *cat[] = {" /\\_/\\ ", "( o.o )", " > ^ < "};

    for (int i = 0; i < 3; i++)
    {
        mvprintw(j++, cursor_position, "%s", cat[i]);
    }
}

void print_large_title()
{
    attron(COLOR_PAIR(2) | A_BOLD);

    mvprintw(5, 61, " ______                               _____     _____ ");
    mvprintw(6, 61, "/ _____|     _     _______   _   _   / ___ \\   / ____|");
    mvprintw(7, 61, "| |  __     / \\   |__   __| | | | | | |   | | ( (____ ");
    mvprintw(8, 61, "| | |_ |   / _ \\     | |    | | | | | |   | |  \\____ \\");
    mvprintw(9, 61, "| |__| |  /  _  \\    | |    | |_| | | |___| |   ____) )");
    mvprintw(10, 61, "\\______| /__/ \\__\\   |_|    \\_____/  \\_____/   |_____/ ");

    attroff(COLOR_PAIR(2) | A_BOLD);
    refresh();
}

void print_process(Processo *processo, WINDOW *win, int j, int flag)
{
    int qtd_inst = 0;

    mvwprintw(win, j + 3, 1, "===== PROCESSO =====");
    mvwprintw(win, j + 4, 2, "Nome: %s", processo->nome);
    mvwprintw(win, j + 5, 2, "PID: %d", processo->pid);
    mvwprintw(win, j + 6, 2, "Prioridade: %d", processo->prioridade);
    mvwprintw(win, j + 7, 2, "Tamanho: %lf", processo->tam);
    mvwprintw(win, j + 8, 2, "Status: %d", processo->prioridade);
    mvwprintw(win, j + 9, 2, "Criação: %d", processo->criacao);
    mvwprintw(win, j + 10, 2, "Semáforos: ");

    for (int i = 0; processo->lista_sem[i] != '\0'; i++)
    {
        mvwprintw(win, j + 11, 3 + i, "%c", processo->lista_sem[i]);
        // mvwprintw(procwin, j+11, 4+i, " ");
    }
    mvwprintw(win, j + 12, 1, "/===================/");

    if (flag == 1)
    {
        processo->lista_instrucao = processo->cabeca_instrucao;
        while (processo->lista_instrucao)
        {
            mvwprintw(win, j + 13, 7, "INSTRUCAO:");
            if (processo->lista_instrucao->tipo == 0)
                mvwprintw(win, j + 14, 2, "Tipo: exec");
            if (processo->lista_instrucao->tipo == 1)
                mvwprintw(win, j + 14, 2, "Tipo: read");
            if (processo->lista_instrucao->tipo == 2)
                mvwprintw(win, j + 14, 2, "Tipo: write");
            if (processo->lista_instrucao->tipo == 3)
                mvwprintw(win, j + 14, 2, "Tipo: P");
            if (processo->lista_instrucao->tipo == 4)
                mvwprintw(win, j + 14, 2, "Tipo: V");
            if (processo->lista_instrucao->tipo == 5)
                mvwprintw(win, j + 14, 2, "Tipo: print");
            if (processo->lista_instrucao->tipo == 0 || processo->lista_instrucao->tipo == 1 ||
                processo->lista_instrucao->tipo == 2 || processo->lista_instrucao->tipo == 5)
                mvwprintw(win, j + 15, 2, "Número: %d", processo->lista_instrucao->num);
            else
                mvwprintw(win, j + 16, 2, "Semaforo: %d", processo->lista_instrucao->semaforo);

            processo->lista_instrucao = processo->lista_instrucao->prox;
            mvwprintw(win, j + 17, 1, "====================");
            qtd_inst++;
        }
        mvwprintw(win, j + 18, 2, "QTD INST: %d", qtd_inst);
        processo->lista_instrucao = processo->cabeca_instrucao;
    }
    else
    {
        mvwprintw(win, j + 13, 7, "INSTRUCAO:");
        if (processo->lista_instrucao->tipo == 0)
            mvwprintw(win, j + 14, 2, "Tipo: exec");
        if (processo->lista_instrucao->tipo == 1)
            mvwprintw(win, j + 14, 2, "Tipo: read");
        if (processo->lista_instrucao->tipo == 2)
            mvwprintw(win, j + 14, 2, "Tipo: write");
        if (processo->lista_instrucao->tipo == 3)
            mvwprintw(win, j + 14, 2, "Tipo: P");
        if (processo->lista_instrucao->tipo == 4)
            mvwprintw(win, j + 14, 2, "Tipo: V");
        if (processo->lista_instrucao->tipo == 5)
            mvwprintw(win, j + 14, 2, "Tipo: print");
        if (processo->lista_instrucao->tipo == 0 || processo->lista_instrucao->tipo == 1 ||
            processo->lista_instrucao->tipo == 2 || processo->lista_instrucao->tipo == 5)
            mvwprintw(win, j + 15, 2, "Número: %d", processo->lista_instrucao->num);
        else
            mvwprintw(win, j + 16, 2, "Semaforo: %d", processo->lista_instrucao->semaforo);
    }
}

void print_criar_processo()
{
}
