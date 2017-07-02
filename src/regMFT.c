#include "../include/regMFT.h"


//carrega um registro MFT indicado por numMFT e seta o ponteiro para zero
//considera que o tamanho de um registro eh dois setores
//ass:Gabriel
void loadMFT(REGMFT *reg, int numMFT, int blkSize){
	blockSize=blkSize;
	read_sector(blockSize+numMFT*2,reg->data);
	read_sector(blockSize+numMFT*2+1,reg->data+SECTORSIZE);
	reg->pointer=0;
	reg->numMFT=numMFT;
}

//passa para a proxima tupla se puder e retorna 0, 1 caso contrario
//ass:Gabriel
int nextTupla(REGMFT *reg){
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
	int i=0, exp=1, soma=0;
	for(i=0;i<4;i++){
		soma+=reg.data[SIZETUPLA*reg.pointer+i]*exp;
		exp*=256;
	}
	return soma;
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
	int exp=1;
	for (int i=0; i<4; i++) {
		printf("\n i=%d coisa:%d ", i, (unsigned int) (reg.data[SIZETUPLA*reg.pointer + 4 + i]*exp));
		out += (int) reg.data[SIZETUPLA*reg.pointer + 4 + i]*exp;
		exp*=256;
	}
	return out;
}

//retorna LBN de reg
//ass:Gabriel
int getLBN(REGMFT reg){
	int out=0;
	int exp=1;
	for (int i=0; i<4; i++) {
		out += (int) reg.data[SIZETUPLA*reg.pointer + 8 + i]*exp;
		exp*=256;
	}
	return out;
}

//retorna numero de blocos continuos de reg
//ass:Gabriel
int getContinuosBlocks(REGMFT reg){
	int out=0;
	int exp=1;
	for (int i=0; i<4; i++) {
		out += (int) reg.data[SIZETUPLA*reg.pointer + 12 + i]*exp;
		exp*=256;
	}
	return out;
}

//----------------------------------------------------------------------------------------------

//seta o tipo na copia do registro
//ass:Henrique
int setRegType(int numMFT, int type, int numTupla, REGMFT *regM) {
	unsigned char buffer[SECTOR_SIZE];
	int numeroTupla;

	if (type <= 2 && type >= -1) {
		if(numTupla >= NUMTUPLAS/2){
			read_sector(blockSize + numMFT*2 + 1, buffer);
			numeroTupla=numTupla-NUMTUPLAS/2;
		}
		else{
			read_sector(blockSize + numMFT*2, buffer);
			numeroTupla=numTupla;
		}
		

		if (type != -1){
			buffer[numeroTupla*16] = (char) type;
			buffer[numeroTupla*16 + 1] = 0;
			buffer[numeroTupla*16 + 2] = 0;
			buffer[numeroTupla*16 + 3] = 0;
			
		} else {
			buffer[numeroTupla*16] = 0xFF;
			buffer[numeroTupla*16 + 1] = 0xFF;
			buffer[numeroTupla*16 + 2] = 0xFF;
			buffer[numeroTupla*16 + 3] = 0xFF;
		}
		
		if(numTupla >= NUMTUPLAS/2){
			write_sector(blockSize + numMFT*2 + 1, buffer);
			regM->data[numTupla*16]=buffer[numeroTupla*16];
			regM->data[numTupla*16+1]=buffer[numeroTupla*16+1];
			regM->data[numTupla*16+2]=buffer[numeroTupla*16+2];
			regM->data[numTupla*16+3]=buffer[numeroTupla*16+3];
		}
		else{
			write_sector(blockSize + numMFT*2, buffer);
			regM->data[numTupla*16]=buffer[numeroTupla*16];
			regM->data[numTupla*16+1]=buffer[numeroTupla*16+1];
			regM->data[numTupla*16+2]=buffer[numeroTupla*16+2];
			regM->data[numTupla*16+3]=buffer[numeroTupla*16+3];
		}
		
		return OK;
	} else {
		return ERRO;
	}
}

//seta numero de blocos contiguos do registro
//ass:Nicolas
int setRegCont(int numMFT , int cont, int numTupla, REGMFT *regM){
	unsigned char buffer[SECTOR_SIZE];
	int exp=1, numeroTupla;
	
	if(cont > 0){
		if(numTupla >= NUMTUPLAS/2){
			read_sector(blockSize + numMFT*2 + 1, buffer);
			numeroTupla=numTupla-NUMTUPLAS/2;
		}
		else{
			read_sector(blockSize + numMFT*2, buffer);
			numeroTupla=numTupla;
		}
		
		for (int i=0; i<4; i++) {
			buffer[numeroTupla*16 + 12 + i] = (char)(cont/exp);
			exp*=256;
		}	
		
	
		if(numTupla >= NUMTUPLAS/2){
			write_sector(blockSize + numMFT*2 + 1, buffer);
			regM->data[numTupla*16+12]=buffer[numeroTupla*16+12];
			regM->data[numTupla*16+1+12]=buffer[numeroTupla*16+1+12];
			regM->data[numTupla*16+2+12]=buffer[numeroTupla*16+2+12];
			regM->data[numTupla*16+3+12]=buffer[numeroTupla*16+3+12];
		}
		else{
			write_sector(blockSize + numMFT*2, buffer);
			regM->data[numTupla*16+12]=buffer[numeroTupla*16+12];
			regM->data[numTupla*16+1+12]=buffer[numeroTupla*16+1+12];
			regM->data[numTupla*16+2+12]=buffer[numeroTupla*16+2+12];
			regM->data[numTupla*16+3+12]=buffer[numeroTupla*16+3+12];
		}
		
		return OK;
	}
	else{
		return ERRO;
	}
	
}

//ass:gabriel
int setLBN(int numMFT, int lbn, int numTupla, REGMFT *regM){
	unsigned char buffer[SECTOR_SIZE];
	int exp=1, numeroTupla;
	
	if(lbn > 0){
		if(numTupla >= NUMTUPLAS/2){
			read_sector(blockSize + numMFT*2 + 1, buffer);
			numeroTupla=numTupla-NUMTUPLAS/2;
		}
		else{
			read_sector(blockSize + numMFT*2, buffer);
			numeroTupla=numTupla;
		}
		
		for (int i=0; i<4; i++) {
			buffer[numeroTupla*16 + 8 + i] = (char)(lbn/exp);
			exp*=256;
		}	
		
	
		if(numTupla >= NUMTUPLAS/2){
			write_sector(blockSize + numMFT*2 + 1, buffer);
			regM->data[numTupla*16+8]=buffer[numeroTupla*16+8];
			regM->data[numTupla*16+1+8]=buffer[numeroTupla*16+1+8];
			regM->data[numTupla*16+2+8]=buffer[numeroTupla*16+2+8];
			regM->data[numTupla*16+3+8]=buffer[numeroTupla*16+3+8];
		}
		else{
			write_sector(blockSize + numMFT*2, buffer);
			regM->data[numTupla*16+8]=buffer[numeroTupla*16+8];
			regM->data[numTupla*16+1+8]=buffer[numeroTupla*16+1+8];
			regM->data[numTupla*16+2+8]=buffer[numeroTupla*16+2+8];
			regM->data[numTupla*16+3+8]=buffer[numeroTupla*16+3+8];
		}
		
		return OK;
	}
	else{
		return ERRO;
	}
}

//ass:gabreil
int setVBN(int numMFT, int vbn, int numTupla, REGMFT *regM){
	unsigned char buffer[SECTOR_SIZE];
	int exp=1, numeroTupla;
	
	if(vbn >= 0){
		if(numTupla >= NUMTUPLAS/2){
			read_sector(blockSize + numMFT*2 + 1, buffer);
			numeroTupla=numTupla-NUMTUPLAS/2;
		}
		else{
			read_sector(blockSize + numMFT*2, buffer);
			numeroTupla=numTupla;
		}
		
		for (int i=0; i<4; i++) {
			buffer[numeroTupla*16 + 4 + i] = (char)(vbn/exp);
			exp*=256;
		}	
		
	
		if(numTupla >= NUMTUPLAS/2){
			write_sector(blockSize + numMFT*2 + 1, buffer);
			regM->data[numTupla*16+4]=buffer[numeroTupla*16+4];
			regM->data[numTupla*16+1+4]=buffer[numeroTupla*16+1+4];
			regM->data[numTupla*16+2+4]=buffer[numeroTupla*16+2+4];
			regM->data[numTupla*16+3+4]=buffer[numeroTupla*16+3+4];
		}
		else{
			write_sector(blockSize + numMFT*2, buffer);
			regM->data[numTupla*16+4]=buffer[numeroTupla*16+4];
			regM->data[numTupla*16+1+4]=buffer[numeroTupla*16+1+4];
			regM->data[numTupla*16+2+4]=buffer[numeroTupla*16+2+4];
			regM->data[numTupla*16+3+4]=buffer[numeroTupla*16+3+4];
		}
		
		return OK;
	}
	else{
		return ERRO;
	}
}
