#include <stdio.h>
#include <string.h>

#include "lista.h"
#include "t2fs.h"
#include "regMFT.h"


#define ERRO -1
#define OK 0
#define CARACTERES_VALIDOS "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ.0123456789"

typedef struct {
    DIR2 handle;
    int numMFT;
    int currentPointer; //pode ficar maior ( ͡° ͜ʖ ͡°)
    int estaAberto;
} ODIN;

typedef struct {
    FILE2 handle;
    int numMFT;
    int currentPointer; //pode ficar maior ( ͡° ͜ʖ ͡°)²
    int estaAberto;
} OFILE;

struct t2fs_bootBlock bootBlock;

OFILE arquivosAbertos[20];
LISTA *diretoriosAbertos;
int handleUltraMasterGenerator = 1;

int openSpots();

int isValidPath(char caminho[]);

int isRealPath(char caminho[]);

int loadBootBlock();

