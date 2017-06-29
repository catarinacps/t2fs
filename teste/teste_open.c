#include "../include/t2fs.h"
#include <stdio.h>

int main() {
    printf("\nIniciando teste da funcao open2.\n");
    printf("Abrindo o arquivo file1...\n");

    if(open2("/file1") < 0) {
        printf("Retorno menor que zero.");
    } else {
        printf("Aparentemente deu boa.");
    }
    printf("\nFim do teste.\n");

    return 0;
}