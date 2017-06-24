#include "../include/regMFT.h"

//carrega um registro MFT indicado por numMFT e seta o ponteiro para zero
//considera que o tamanho de um registro eh dois setores
//ass:Gabriel
void loadMFT(REGMFT *reg, int numMFT){
	read_sector(bootBlock.blocksize+numMFT*2,reg->data);
	read_sector(bootBlock.blocksize+numMFT*2+1,reg->data+SECTORSIZE);
	reg->pointer=0;
}

//passa para a proxima tupla se puder e retorna 0, 1 caso contrario
//ass:Gabriel
void nextTupla(REGMFT *reg){
	if(reg->pointer<NUMTUPLAS-1){
			reg->pointer++;
			return OK;
	}
	else
		return ERRO;
}

//passa para a tupla anterior se puder e retorna 0, 1 caso contrario
//ass:Gabriel
int backTupla(REGMFT *reg){
	if(reg->pointer>0){
		reg->pointer--;
		return OK;
	}
	else
		return ERRO;
}

//----------------------------------------------------------------------------------------------
/*Type
	-1: tupla livre
	 0: fim de encadeamento
	 1:	mapeamento VBN-LBN
	 2:	registro adicional em VBN
*/
//retorna o tipo da tupla. gg litle-endian
//ass:Gabriel
int getTuplaType(REGMFT *reg){
	return (int*)reg->data[SIZETUPLA*reg->pointer];
}

//retorna 1 se a tupla indica registro livre, 0 caso contrario
//ass:Gabriel
int isTuplaFree(REGMFT *reg){
	if(getTuplaType(reg)==-1)
		return OK;
	else return ERRO;
}

//retorna 1 se a tupla indica registro livre, 0 caso contrario
//ass:Gabriel
int isTuplaEnd(REGMFT *reg){
	if(getTuplaType(reg)==0)
		return OK;
	else return ERRO;
}

//retorna 1 se a tupla indica registro livre, 0 caso contrario
//ass:Gabriel
int isTuplaChain(REGMFT *reg){
	if(getTuplaType(reg)==1)
		return OK;
	else return ERRO;
}

//retorna 1 se a tupla indica registro livre, 0 caso contrario
//ass:Gabriel
int isTuplaJmp(REGMFT *reg){
	if(getTuplaType(reg)==2)
		return OK;
	else return ERRO;
}
//----------------------------------------------------------------------------------------------

//retorna VBN de reg
//ass:Gabriel
int getVBN(REGMFT *reg){
	int out=0;
	for(int i=0;i<4;i++){
		out+=reg->data[SIZETUPLA*reg->pointer+4+i]*256^i;
	}
	return out;
}

//retorna LBN de reg
//ass:Gabriel
int getLBN(REGMFT *reg){
	int out=0;
	for(int i=0;i<4;i++){
		out+=reg->data[SIZETUPLA*reg->pointer+8+i]*256^i;
	}
	return out;
}

//retorna numero de blocos continuos de reg
//ass:Gabriel
int getCont(REGMFT *reg){
	int out=0;
	for(int i=0;i<4;i++){
		out+=reg->data[SIZETUPLA*reg->pointer+12+i]*256^i;
	}
	return out;
}























