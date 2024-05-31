#ifndef INTERFACE_H
#define INTERFACE_H

#include "../escalonador/rr.h"
#include "../memoria/memoria.h"
#include "../processo/processo.h"
#include "../semaforo/semaforo.h"

#include <ncurses.h>
#include <string.h>

// ======= FUNCOES =======
WINDOW *criar_win(int row, int col);
void draw_cat(int cursor_position);
void print_large_title();
void print_process(Processo *processo, WINDOW *win, int j, int flag);
Processo *print_criar_processo();
void print_tela_exec(Round_robin *cabeca);
void print_tela_memoria(Segmento *lista_seg);
int menu();

#endif
