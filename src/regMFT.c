#include "../include/regMFT.h"

//carrega um registro MFT indicado por numMFT e seta o ponteiro para zero
//considera que o tamanho de um registro eh dois setores
//ass:Gabriel
void loadMFT(REGMFT *reg, int numMFT){
	read_sector(bootBlock.blocksize+numMFT,reg->data);
	read_sector(bootBlock.blocksize+numMFT,reg->data+SECTORSIZE);
	reg->pointer=0;
}

//passa para a proxima tupla
//ass:Gabriel
void nextTupla(REGMFT *reg){
	reg->pointer++;
}

//retorna o tipo da tupla. gg litle-endian
//ass:Gabriel
int getTuplaType(REGMFT *reg){
	return (int*)reg->data[SIZETUPLA*reg->pointer];
}

//retorna 1 se a tupla indica registro livre, 0 caso contrario
//ass:Gabriel
int isTuplaFree(REGMFT *reg){
	if(getTuplaType(reg)==-1)
		return 1;
	else return 0;
}