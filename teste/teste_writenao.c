#include "../include/t2fs.h"
#include <stdio.h>

// droga
int main() {
	FILE2 rendeu, rendeudroga;
    char buffer[50], bufferdroga[2100], writedroga[1500];

    for(int i=0; i<50; i++){
        buffer[i]=0;
    }

    for(int i=0; i<2100; i++){
        bufferdroga[i]=0;
    }

    for(int i=0; i<1498; i++){
        writedroga[i]='b';
    }
    writedroga[1498]='a';
    writedroga[1499]='\0';

	puts(writedroga);

    printf("\nIniciando teste da funcao write2.\n");
    printf("Criando o arquivo katiau...\n");

    if((rendeu = create2("/katiau")) < 0) {
        printf("Retorno menor que zero. btw, handle e %d", rendeu);
    } else {
        printf("Aparentemente deu boa no create. btw, handle e %d\n", rendeu);

        if (write2(rendeu, "Olha la o relampago Marquinhos!", 31) > 0) {
            printf("eu diria que deu boa no write\n");
            if(seek2(rendeu, 0)==0){
                printf("deu boa no seek\n");
                if(read2(rendeu,buffer,40) != -1){
                    printf("deu boa no read, isso foi oq ele leu:\n");
                    puts(buffer);
                }else{
                    printf("deu ruim no read\n");
                }
            }else{
                printf("deu caca no seek\n");
            }
        } else {
            printf("deu erro no write\n");
        }

        if((rendeudroga = create2("/katiau2")) >= 0) {
            if (write2(rendeudroga, writedroga, 1500) > 0) {
                printf("eu diria que deu boa no write\n");
                if(seek2(rendeudroga, 1300)==0){
                        printf("deu boa no seek\n");
                        if(read2(rendeudroga,bufferdroga,200) != -1){
                                printf("deu boa no read, isso foi oq ele leu:\n");
                                puts(bufferdroga);
                        }else{
                            printf("deu ruim no read\n");
                        }
                    }else{
                        printf("deu caca no seek\n");
                    }
            } else {
                printf("deu erro no write\n");
            }
        }
    }

    printf("\nFim do teste.\n");

    return 0;
}
