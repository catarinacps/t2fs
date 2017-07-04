#include <stdio.h>
#include <string.h>

//#include "auxlib2.h"
//#include "apidisk.h"
#include "regMFT.h"
#include "bitmap2.h"
//#include "t2fs.h"

#define SIZERECORD 64
#define ERRO -3
#define OK 0
#define ERRO_MSM_BLOCO -4
#define ERRO_MSM_TUPLA -5
#define ERRO_PROXIMA_TUPLA -6
#define ERRO_PROXIMO_MFT -7
#define ERRO_EOF -8


typedef struct {
	unsigned char data[SECTOR_SIZE];
	int pointer;	 //qual dos 4 diretorios dentro do setor
	int sectPointer;
	int blkPointer;
	REGMFT regM;
}REGRECORD;

int blocoSize;

//carrega a primeira entrada de diretorio e seta o ponteiro para zero
//ass:Nicolas
void loadFirstRecord(REGRECORD *regR, REGMFT regM, int blockSize);

//passa para a proxima entrada de diretorio se puder e retorna 0, 1 caso contrario
//ass:Nicolas
int nextRecord(REGRECORD *regR, REGMFT *regM);

//passa para a entrada de diretorio anterior se puder e retorna 0, 1 caso contrario
//ass:Nicolas
//int backRecord(REGRECORD *regR, REGMFT *regM);

//----------------------------------------------------------------------------------------------
/*Type
	 0: registro invalido (livre)
	 1: arquivo regular (file)
	 2:	arquivo de diretorio (dir)
*/
//retorna o tipo da entrada de diretorio. gg litle-endian
//ass:Nicolas
int getRecordType(REGRECORD regR);

//retorna 1 se a entrada de diretorio indica registro livre, 0 caso contrario
//ass:Nicolas
int isRecordFree(REGRECORD regR);

//retorna 1 se a entrada de diretorio indica um arquivo, 0 caso contrario
//ass:Nicolas
int isRecordFile(REGRECORD regR);

//retorna 1 se a entrada de diretorio indica um diretorio, 0 caso contrario
//ass:Nicolas
int isRecordDir(REGRECORD regR);

//----------------------------------------------------------------------------------------------

//retorna nome de reg
//ass:Nicolas
int getRecordName(REGRECORD regR, char *buffer);

//retorna tamanho total em blocos de reg
//ass:Nicolas
int getBlocksFileSize(REGRECORD regR);

//retorna tamanho total em bytes de reg
//ass:Nicolas
int getBytesFileSize(REGRECORD regR);

//retorna numero de registro MFT de reg
//ass:Nicolas
int getMFTNumber(REGRECORD regR);

//----------------------------------------------------------------------------------------------

//seta o tipo da entrada de diretorio.
//ass:Gabriel
int setRecordType(REGRECORD *regR, int type);

//seta nome de reg, retorna OK se conseguiu, ERRO caso contrario
//ass:Gabriel
int setRecordName(REGRECORD *regR, char *buffer);

//seta tamanho total em blocos de reg
//ass:Gabriel
void setBlocksFileSize(REGRECORD *regR, int size);

//seta tamanho total em bytes de reg
//ass:Gabriel
void setBytesFileSize(REGRECORD *regR, int size);

//seta numero de registro MFT de reg
//ass:Gabriel
void setMFTNumber(REGRECORD *regR, int numMFT);

