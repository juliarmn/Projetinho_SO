#include "disco.h"

int buscar_trilha(Disco *HD, int num_trilha){
    //Testando tem trilhas
    if(!HD->cabeca_trilhas){
        return 0;
    }

    Trilhas *aux = HD->cabeca_trilhas->prox;
    
    //Testando se ser a cabeca    
    if(HD->cabeca_trilhas->num_trilha == num_trilha){
        return 1;
    }

    //andando na lista e verificando se temos o elemento na trilha
    while(HD->cabeca_trilhas != aux){

        if(aux->num_trilha == num_trilha){
            return 1;
        }
    }
    return 0;
}

void inserir_trilha(Processo *processo, int num_trilha, Disco **HD){
    

    if(buscar_trilha((*HD), num_trilha){
        printf("Trilha ocupada\n");
        return;
    }
    Trilhas *novo = malloc(sizeof(Trilhas));
    // So tem 1 elemento
    //Se for menor que a cabeca
    if((*HD)->cabeca_trilhas->num_trilha > num_trilha){
        novo->ant = (*HD)->cabeca_trilhas->ant;
        novo->prox = (*HD)->cabeca_trilhas;
        (*HD)->cabeca_trilhas->ant->prox = novo;
        (*HD)->cabeca_trilhas->ant = novo;
        (*HD)->cabeca_trilhas = novo;
    }
    //Se nao procura a posicao dele na lista


}