#include "../include/t2fs.h"
#include <stdio.h>

int main() {
	int rendeu;
    DIRENT2 dir;
    printf("\nIniciando teste da funcao readdir2.\n");
    printf("Abrindo o diretorio root...\n");

    if((rendeu = opendir2("/")) < 0) {
        printf("Retorno menor que zero. btw, handle e %d", rendeu);
    } else {
        printf("Aparentemente deu boa. btw, handle e %d", rendeu);
        while(readdir2(rendeu, &dir)== 0){
            puts(dir.name);
            printf("Tipo:%d Size:%d \n", dir.fileType, dir.fileSize);
        }
        puts("Fim do diretorio");
    }
    printf("\nFim do teste.\n");

    return 0;
}
