#include "../include/t2fs.h"
#include <stdio.h>

int main() {
	int rendeu;
    printf("\nIniciando teste da funcao open2.\n");
    printf("Abrindo o arquivo file1...\n");

    if((rendeu = open2("/file1")) < 0) {
        printf("Retorno menor que zero. btw, handle e %d", rendeu);
    } else {
        printf("Aparentemente deu boa. btw, handle e %d", rendeu);
    }
    printf("\nFim do teste.\n");

    return 0;
}
