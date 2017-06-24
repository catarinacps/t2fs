#include <stdio.h>
#include <string.h>

#include "lista.h"
#include "t2fs.h"


#define ERRO -1
#define OK 0
#define CARACTERES_VALIDOS "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ."

typedef struct {
    DIR2 handle;
    int numMFT;
    int currentPointer; //pode ficar maior ( ͡° ͜ʖ ͡°)
    bool estaAberto;
} ODIN;

typedef struct {
    FILE2 handle;
    int numMFT;
    int currentPointer; //pode ficar maior ( ͡° ͜ʖ ͡°)²
    bool estaAberto;
} OFILE;

OFILE arquivosAbertos[20];
LISTA *diretoriosAbertos;

int openSpots();

int isValidPath(char caminho[]);
