#include <stdio.h>
#include <math.h>

//#include "auxlib2.h"
#include "apidisk.h"

#define SECTORSIZE 256
#define SIZEREGMFT 512
#define SIZETUPLA 16
#define NUMTUPLAS 32
#define ERRO -3
#define OK 0

typedef struct {
    unsigned char data[SIZEREGMFT];
    int pointer;
    int numMFT;
} REGMFT;

int blockSize;

// carrega um registro MFT indicado por numMFT e seta o ponteiro para zero
// considera que o tamanho de um registro eh dois setores
// ass:Gabriel
void loadMFT(REGMFT *reg, int numMFT, int blkSize);

// passa para a proxima tupla se puder e retorna 0, 1 caso contrario
// ass:Gabriel
int nextTupla(REGMFT *reg);

// passa para a tupla anterior se puder e retorna 0, 1 caso contrario
// ass:Gabriel
int backTupla(REGMFT *reg);

//----------------------------------------------------------------------------------------------
/*Type
        -1: tupla livre
         0: fim de encadeamento
         1:	mapeamento VBN-LBN
         2:	registro adicional em VBN
*/
// retorna o tipo da tupla. gg litle-endian
// ass:Gabriel
int getTuplaType(REGMFT reg);

// retorna 1 se a tupla indica registro livre, 0 caso contrario
// ass:Gabriel
int isTuplaFree(REGMFT reg);

// retorna 1 se a tupla indica fim de encadeamento, 0 caso contrario
// ass:Gabriel
int isTuplaEnd(REGMFT reg);

// retorna 1 se a tupla indica mapeamento, 0 caso contrario
// ass:Gabriel
int isTuplaChain(REGMFT reg);

// retorna 1 se a tupla indica registro adicional, 0 caso contrario
// ass:Gabriel
int isTuplaJmp(REGMFT reg);

//----------------------------------------------------------------------------------------------

// retorna VBN de reg
// ass:Gabriel
int getVBN(REGMFT reg);

// retorna LBN de reg
// ass:Gabriel
int getLBN(REGMFT reg);

// retorna numero de blocos continuos de reg
// ass:Gabriel
int getContinuosBlocks(REGMFT reg);

// seta o tipo na copia do registro
// ass:Henrique
int setRegType(int numMFT, int type, int numTupla, REGMFT *regM);

// seta numero de blocos contiguos do registro
// ass:Nicolas
int setRegCont(int numMFT, int cont, int numTupla, REGMFT *regM);

// bunda: henrique
int setLBN(int numMFT, int lbn, int numTupla, REGMFT *regM);

// bunda: henrique
int setVBN(int numMFT, int vbn, int numTupla, REGMFT *regM);