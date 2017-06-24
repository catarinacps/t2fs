#include "../include/auxlib2.h"

//BLANK SPACE
int openSpots() {
    int espacosAbertos = 0;
    for(int i = 0; i<20; i++) {
        if (arquivosAbertos[i].estaAberto == 0) {
            espacosAbertos++;
        }
    }

    return espacosAbertos;
}

int isValidPath(char caminho[]){
    char *token, woods[2];

    strcpy(woods, "/");
    token = strtok(caminho, woods);

    while (token != NULL) {
        if (strspn(token, CARACTERES_VALIDOS) != strlen(token)) {
            return ERRO;
        }
        token = strtok(caminho, woods);
    }

    // preicerolder
    // voltar aqui dps de fazer interface com o disco. a grafica. sim.
}