#include "../include/t2fs.h"
#include <stdio.h>

int main() {
	FILE2 rendeu;
    char buffer[50];

    for(int i=0; i<50; i++){
        buffer[i]=0;
    }

    printf("\nIniciando teste da funcao read2.\n");
    printf("Abrindo o arquivo file1...\n");

    if((rendeu = open2("/file1")) < 0) {
        printf("Retorno menor que zero. btw, handle e %d", rendeu);
    } else {
        printf("Aparentemente deu boa no open. btw, handle e %d", rendeu);

        if (read2(rendeu, buffer, 50) > 0) {
            printf("eu diria que deu boa\n");
            puts(buffer);
        } else {
            printf("deu merda");
        }
    }
    printf("\nFim do teste.\n");

    return 0;
}