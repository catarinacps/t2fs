#include <stdio.h>
#include "../include/diskio.h"
#include "../include/auxlib2.h"
#include "../include/apidisk.h"


#define SIZEREGMFT 512
#define SIZETUPLA 16

typedef struct {
	byte data[SIZEREGMFT];
	int pointer=0;
}REGMFT;

int loadMFT(REGMFT *reg, int numMFT);