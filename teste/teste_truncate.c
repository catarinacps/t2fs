#include "../include/t2fs.h"
#include <stdio.h>

int main() {
	FILE2 rendeu;
    char buffer[50];

    for(int i=0; i<50; i++){
        buffer[i]=0;
    }

    printf("\nIniciando teste da funcao truncate2.\n");
    printf("Abrindo o arquivo file1...\n");

    if((rendeu = open2("/file1")) < 0) {
        printf("Retorno menor que zero. btw, handle e %d", rendeu);
    } else {
        printf("Aparentemente deu boa no open. btw, handle e %d", rendeu);
        if (read2(rendeu, buffer, 50) > 0) {
            printf("\neu diria que deu boa");
            puts(buffer);
            if(seek2(rendeu,6)==0){
                printf("\n deu boa no seek");
                if(truncate2(rendeu)==0){
                    printf("\ndeu boa no truncate");
                    seek2(rendeu,0);
                    for(int i=0; i<50; i++){
                         buffer[i]=0;
                    }
                    read2(rendeu, buffer, 50);
                    puts(buffer);
                }else{
                    printf("\nnao rendeu ;-(");
                }
            }else{
                printf("\ndeu caca no seek");
            }

        } else {
            printf("\ndeu merda no read");
        }
    }
    printf("\nFim do teste.\n");

    return 0;
}