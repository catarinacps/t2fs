#include "../include/t2fs.h"
#include <stdio.h>

int main(){
    printf("/nIniciando teste da funcao create2./n");
    printf("Criando arquivo memes em root.../n");
    if(create2("/memes")<0)
        printf("Retorno menor que zero.");
    else
        printf("Aparentemente deu boa.");
    printf("/nFim do teste./n");
    return 0;
}