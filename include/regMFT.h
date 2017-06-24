#include <stdio.h>
#include "../include/diskio.h"
#include "../include/auxlib2.h"
#include "../include/apidisk.h"


#define SIZEREGMFT 512
#define SIZETUPLA 16
#define NUMTUPLAS 32

typedef struct {
	byte data[SIZEREGMFT];
	int pointer=0;
}REGMFT;

//carrega um registro MFT indicado por numMFT e seta o ponteiro para zero
//considera que o tamanho de um registro eh dois setores
//ass:Gabriel
void loadMFT(REGMFT *reg, int numMFT);

//passa para a proxima tupla se puder e retorna 0, 1 caso contrario
//ass:Gabriel
void nextTupla(REGMFT *reg);

//passa para a tupla anterior se puder e retorna 0, 1 caso contrario
//ass:Gabriel
int backTupla(REGMFT *reg);

//----------------------------------------------------------------------------------------------
/*Type
	-1: tupla livre
	 0: fim de encadeamento
	 1:	mapeamento VBN-LBN
	 2:	registro adicional em VBN
*/
//retorna o tipo da tupla. gg litle-endian
//ass:Gabriel
int getTuplaType(REGMFT *reg);

//retorna 1 se a tupla indica registro livre, 0 caso contrario
//ass:Gabriel
int isTuplaFree(REGMFT *reg);

//retorna 1 se a tupla indica fim de encadeamento, 0 caso contrario
//ass:Gabriel
int isTuplaEnd(REGMFT *reg);

//retorna 1 se a tupla indica mapeamento, 0 caso contrario
//ass:Gabriel
int isTuplaChain(REGMFT *reg);

//retorna 1 se a tupla indica registro adicional, 0 caso contrario
//ass:Gabriel
int isTuplaJmp(REGMFT *reg);

//----------------------------------------------------------------------------------------------

//retorna VBN de reg
//ass:Gabriel
int getVBN(REGMFT *reg);

//retorna LBN de reg
//ass:Gabriel
int getLBN(REGMFT *reg);

//retorna numero de blocos continuos de reg
//ass:Gabriel
int getCont(REGMFT *reg);

