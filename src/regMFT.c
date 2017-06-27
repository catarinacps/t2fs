#include <math.h>
#include "../include/regMFT.h"
#include "../include/apidisk.h"

//carrega um registro MFT indicado por numMFT e seta o ponteiro para zero
//considera que o tamanho de um registro eh dois setores
//ass:Gabriel
void loadMFT(REGMFT *reg, int numMFT){
	read_sector(bootBlock.blocksize+numMFT*2,reg->data);
	read_sector(bootBlock.blocksize+numMFT*2+1,reg->data+SECTORSIZE);
	reg->pointer=0;
	reg->numMFT=numMFT;
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
int getTuplaType(REGMFT reg){
	return (int)reg.data[SIZETUPLA*reg.pointer];
}

//retorna 1 se a tupla indica registro livre, 0 caso contrario
//ass:Gabriel
int isTuplaFree(REGMFT reg){
	if(getTuplaType(reg) == -1)
		return OK;
	else return ERRO;
}

//retorna 1 se a tupla indica registro livre, 0 caso contrario
//ass:Gabriel
int isTuplaEnd(REGMFT reg){
	if(getTuplaType(reg)==0)
		return OK;
	else return ERRO;
}

//retorna 1 se a tupla indica registro livre, 0 caso contrario
//ass:Gabriel
int isTuplaChain(REGMFT reg){
	if(getTuplaType(reg) == 1)
		return OK;
	else return ERRO;
}

//retorna 1 se a tupla indica registro livre, 0 caso contrario
//ass:Gabriel
int isTuplaJmp(REGMFT reg){
	if(getTuplaType(reg) == 2)
		return OK;
	else return ERRO;
}

//----------------------------------------------------------------------------------------------

//retorna VBN de reg
//ass:Gabriel
int getVBN(REGMFT reg){
	int out=0;
	for (int i=0; i<4; i++) {
		out += (int) reg.data[SIZETUPLA*reg.pointer + 4 + i]*pow(256,i);
	}
	return out;
}

//retorna LBN de reg
//ass:Gabriel
int getLBN(REGMFT reg){
	int out=0;
	for (int i=0; i<4; i++) {
		out += (int) reg.data[SIZETUPLA*reg.pointer + 8 + i]*pow(256,i);
	}
	return out;
}

//retorna numero de blocos continuos de reg
//ass:Gabriel
int getCont(REGMFT reg){
	int out=0;

	for (int i=0; i<4; i++) {
		out += (int) reg.data[SIZETUPLA*reg.pointer + 12 + i]*pow(256,i);
	}
	return out;
}

//----------------------------------------------------------------------------------------------

//seta o tipo na copia do registro
//ass:Henrique
int setRegType(int numMFT, int type, int numTupla) {
	char buffer[SECTOR_SIZE];

	if (type <= 2 && type >= -1) {
		if(numTupla >= NUMTUPLAS/2){
			read_sector(bootBlock.blockSize + numMFT*2 + 1, buffer);
		}
		else{
			read_sector(bootBlock.blockSize + numMFT*2, buffer);
		}
		

		if (type != -1){
			buffer[numTupla*16] = (char) type;
			buffer[numTupla*16 + 1] = 0;
			buffer[numTupla*16 + 2] = 0;
			buffer[numTupla*16 + 3] = 0;
			
		} else {
			buffer[numTupla*16] = 0xFF;
			buffer[numTupla*16 + 1] = 0xFF;
			buffer[numTupla*16 + 2] = 0xFF;
			buffer[numTupla*16 + 3] = 0xFF;
		}
		
		
		if(numTupla >= NUMTUPLAS/2){
			write_sector(bootBlock.blockSize + numMFT*2 + 1, buffer);
		}
		else{
			write_sector(bootBlock.blockSize + numMFT*2, buffer);
		}
		
		return OK;
	} else {
		return ERRO;
	}
}

//seta numero de blocos contiguos do registro
//ass:Nicolas
int setRegCont(int numMFT , int cont, int numTupla){
	char buffer[SECTOR_SIZE];
	
	if(cont > 0){
		if(numTupla >= NUMTUPLAS/2){
			read_sector(bootBlock.blockSize + numMFT*2 + 1, buffer);
		}
		else{
			read_sector(bootBlock.blockSize + numMFT*2, buffer);
		}
		
		for (int i=0; i<4; i++) {
			buffer[numTupla*16 + 12 + i] = (byte)(cont/pow(256,i));
		}	

	
		if(numTupla >= NUMTUPLAS/2){
			write_sector(bootBlock.blockSize + numMFT*2 + 1, buffer);
		}
		else{
			write_sector(bootBlock.blockSize + numMFT*2, buffer);
		}
		
		return OK;
	}
	else{
		return ERRO;
	}
	
}

int setLBN(int numMFT, int lbn, int numTupla){
	char buffer[SECTOR_SIZE];
	
	if(lbn > 0){
		if(numTupla >= NUMTUPLAS/2){
			read_sector(bootBlock.blockSize + numMFT*2 + 1, buffer);
		}
		else{
			read_sector(bootBlock.blockSize + numMFT*2, buffer);
		}
		
		for (int i=0; i<4; i++) {
			buffer[numTupla*16 + 8 + i] = (byte)(lbn/pow(256,i));
		}	

	
		if(numTupla >= NUMTUPLAS/2){
			write_sector(bootBlock.blockSize + numMFT*2 + 1, buffer);
		}
		else{
			write_sector(bootBlock.blockSize + numMFT*2, buffer);
		}
		
		return OK;
	}
	else{
		return ERRO;
	}
}

int setVBN(int numMFT, int vbn, int numTupla){
	char buffer[SECTOR_SIZE];
	
	if(vbn > 0){
		if(numTupla >= NUMTUPLAS/2){
			read_sector(bootBlock.blockSize + numMFT*2 + 1, buffer);
		}
		else{
			read_sector(bootBlock.blockSize + numMFT*2, buffer);
		}
		
		for (int i=0; i<4; i++) {
			buffer[numTupla*16 + 4 + i] = (byte)(vbn/pow(256,i));
		}	

	
		if(numTupla >= NUMTUPLAS/2){
			write_sector(bootBlock.blockSize + numMFT*2 + 1, buffer);
		}
		else{
			write_sector(bootBlock.blockSize + numMFT*2, buffer);
		}
		
		return OK;
	}
	else{
		return ERRO;
	}
}