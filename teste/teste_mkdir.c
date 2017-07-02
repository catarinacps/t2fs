#include "../include/t2fs.h"
#include <stdio.h>

int main(){
    printf("\nIniciando teste da funcao mkdir2.\n");
    printf("Criando pasta memes em root...\n");
    if(mkdir2("/memes")<0)
        printf("Retorno menor que zero.");
    else
        printf("Aparentemente deu boa.");
    printf("\nFim do teste.\n");
    return 0;
}