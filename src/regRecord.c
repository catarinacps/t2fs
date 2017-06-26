#include "../include/regRecord.h"
#include "../include/regMFT.h"

//carrega a primeira entrada de diretorio e seta o ponteiro para zero
//ass:Nicolas
void loadFirstRecord(REGRECORD *regR, REGMFT regM){
	read_sector(bootBlock.blocksize * getLBN(regM) , regR->data);
	regR->pointer = 0;	// qual dos 4 diretorios dentro do setor
	regR->sectPointer = 0; // qual dos 4 setores dentro do bloco
	regR->blkPointer = getLBN(regM); 
}

// passa para a proxima entrada de diretorio se puder
// a funcao e uma merda
//ass:Nicolas
void nextRecord(REGRECORD *regR, REGMFT *regM){
	REGMFT regMaux;

	if (regR->pointer < 3) {		// pode pegar 4 diretorios dentro de 1 setor
		regR->pointer++;
		if(isRecordFree(*regR)==OK){
			regR->pointer--;
			return ERRO_MSM_SETOR;
		}else{
			return OK;
		}	
	}
	else {
		if (regR->sectPointer < bootBlock.blocksize - 1) {	// pode pegar 4 setores dentro de 1 bloco (geralmente, depende do blocksize)
			regR->pointer = 0;
			regR->sectPointer++;
			read_sector(bootBlock.blocksize * regR->blkPointer + regR->sectPointer , regR->data);
			if(isRecordFree(*regR)==OK){
				regR->sectPointer--;
				regR->pointer=3;
				read_sector(bootBlock.blocksize * regR->blkPointer + regR->sectPointer , regR->data);
				return ERRO_MSM_BLOCO;
			}else{
				return OK;
			}
		}
		else {
			if (regR->blkPointer - getLBN(*regM) < getCont(*regM) - 1) {		// temos que pegar o proximo bloco, neste caso ele é contiguo na memoria (está na mesma tupla)
				regR->blkPointer++;
				regR->pointer = 0;
				regR->sectPointer = 0;
				read_sector(bootBlock.blocksize * regR->blkPointer + regR->sectPointer , regR->data);
				if(isRecordFree(*regR)==OK){
					regR->blkPointer--;
					regR->sectPointer=3;
					regR->pointer=3;
					read_sector(bootBlock.blocksize * regR->blkPointer + regR->sectPointer , regR->data);
					return ERRO_MSM_TUPLA;
				}else{
					return OK;
				}	
			}
			else {			// temos que pegar o proximo bloco mas ele NAO está contiguo, entao pegar a proxima tupla
				if (nextTupla(regM) == OK) {
					if (isTuplaChain(*regM) == OK) {
						regR->pointer = 0;
						regR->sectPointer = 0;
						regR->blkPointer = getLBN(*regM);
						read_sector(bootBlock.blocksize * regR->blkPointer + regR->sectPointer , regR->data);
						if(isRecordFree(*regR)==OK){
							backTupla(regM);
							regR->blkPointer=getLBN(*regM)+getCont(*regM)-1;
							regR->sectPointer=3;
							regR->pointer=3;
							read_sector(bootBlock.blocksize * regR->blkPointer + regR->sectPointer , regR->data);
							return ERRO_PROXIMA_TUPLA;
						}else{
							return OK;
						}
					} else if (isTuplaJmp(*regM) == OK) {		// puta merda q troco feio
						regMaux = *regM;
						loadMFT(regM, getVBN(*regM));			// pega o registro adicional

						if (isTuplaChain(*regM) == OK) {
							regR->pointer = 0;
							regR->sectPointer = 0;
							regR->blkPointer = getLBN(*regM);
							read_sector(bootBlock.blocksize * regR->blkPointer + regR->sectPointer , regR->data);
							if(isRecordFree(*regR)==OK){
								*regM=regMaux;
								regR->blkPointer=getLBN(*regM)+getCont(*regM)-1;
								regR->sectPointer=3;
								regR->pointer=3;
								read_sector(bootBlock.blocksize * regR->blkPointer + regR->sectPointer , regR->data);
								return ERRO_PROXIMO_MFT;
							}else{
								return OK;
							}
						} else {
							*regM=regMaux;
							return ERRO; //sera q cai aki? acho q nao
						}
					} else {
						backTupla(regM);
						return ERRO_EOF;
					}
				} else {
					return ERRO;	//nunca vai chegar aki ;-)
				}
			}
		}	
	}
	return OK;
}

//passa para a entrada de diretorio anterior se puder e retorna 0, 1 caso contrario
//ass:Nicolas
//int backRecord(REGRECORD *regR, REGMFT *regM);

//----------------------------------------------------------------------------------------------
/*Type
	 0: registro invalido (livre)
	 1: arquivo regular (file)
	 2:	arquivo de diretorio (dir)
*/
//retorna o tipo da entrada de diretorio.
//ass:Nicolas
int getRecordType(REGRECORD regR){
	return regR.data[SIZERECORD*regR.pointer];
}

//retorna 1 se a entrada de diretorio indica registro livre, 0 caso contrario
//ass:Nicolas
int isRecordFree(REGRECORD regR){
	if(getRecordType(regR)!=1 && getRecordType(regR)!=2)
		return OK;
	else return ERRO;
}

//retorna 1 se a entrada de diretorio indica um arquivo, 0 caso contrario
//ass:Nicolas
int isRecordFile(REGRECORD regR){
	if(getRecordType(regR)==1)
		return OK;
	else return ERRO;
}

//retorna 1 se a entrada de diretorio indica um diretorio, 0 caso contrario
//ass:Nicolas
int isRecordDir(REGRECORD regR){
	if(getRecordType(regR)==2)
		return OK;
	else return ERRO;
}

//----------------------------------------------------------------------------------------------

//carrega nome de reg, retorna 0 se conseguiu, 1 caso contrario
//ass:Nicolas
int getRecordName(REGRECORD regR, char *buffer){
	if(strcpy(buffer, regR.data[SIZERECORD*regR.pointer+1]))
		return OK;
	else
		return ERRO;
}

//retorna tamanho total em blocos de reg
//ass:Nicolas
int getBlocksFileSize(REGRECORD regR){
	int out=0;
	for(int i=0;i<4;i++){
		out += (int) regR.data[SIZERECORD*regR.pointer + 52 + i]*pow(256,i);
	}
	return out;
}

//retorna tamanho total em bytes de reg
//ass:Nicolas
int getBytesFileSize(REGRECORD regR){
	int out=0;
	for(int i=0;i<4;i++){
		out += (int) regR.data[SIZERECORD*regR.pointer + 56 + i]*pow(256,i);
	}
	return out;
}

//retorna numero de registro MFT de reg
//ass:Nicolas
int getMFTNumber(REGRECORD regR){
	int out=0;
	for(int i=0;i<4;i++){
		out += (int) regR.data[SIZERECORD*regR.pointer + 60 + i]*pow(256,i);
	}
	return out;
}
//-----------------------------------------------------------------------------------------
//seta o tipo da entrada de diretorio.
//ass:Gabriel
int setRecordType(REGRECORD *regR, int type){
	int out=0;
	if(type >= 0 && type <= 2){
		regR->data[SIZERECORD*regR->pointer]=(byte)type;		
		
		return OK;
	}else{
		return ERRO;
	}
}

//seta nome de reg, retorna OK se conseguiu, ERRO caso contrario
//ass:Gabriel
int setRecordName(REGRECORD *regR, char *buffer){
	
	if(strcpy(regR.data[SIZERECORD*regR.pointer+1], buffer))
		return OK;
	else
		return ERRO;
}

//seta tamanho total em blocos de reg
//ass:Gabriel
int setBlocksFileSize(REGRECORD *regR, int size){
	for(int i=0;i<4;i++){
		regR.data[SIZERECORD*regR.pointer + 52 + i] = (byte)(size/pow(256,i)); //magica logica aritmetica
	}
}

//seta tamanho total em bytes de reg
//ass:Gabriel
int setBytesFileSize(REGRECORD *regR, int size){
	for(int i=0;i<4;i++){
		regR.data[SIZERECORD*regR.pointer + 56 + i] = (byte)(size/pow(256,i));
	}
}

//seta numero de registro MFT de reg
//ass:Gabriel
int setMFTNumber(REGRECORD *regR, int numMFT){
	for(int i=0;i<4;i++){
		regR.data[SIZERECORD*regR.pointer + 60 + i] = (byte)(numMFT/pow(256,i));
	}
}

int writeNewFileRecord(char *name, int numMFT, REGRECORD *regR, REGMFT *regM, int nextRecordOutput){
	
	switch(nextRecordOutput){
		case ERRO_MSM_SETOR:{
			regR->pointer++;
		}
		break;
		case ERRO_MSM_BLOCO:{
			regR->sectPointer++;
			regR->pointer=0;
		}
		break;	//a partir daqui tem q achar bloco novo
		case ERRO_MSM_TUPLA:{
			regR->blkPointer++;
			regR->sectPointer=0;
			regR->pointer=0;
		}
		break;
		case ERRO_PROXIMA_TUPLA:{
			nextTupla(regM);
			regR->blkPointer=getLBN(*regM);
			regR->sectPointer=0;
			regR->pointer=0;
		}
		break;
		case ERRO_PROXIMO_MFT:{
			loadMFT(regM, getVBN(*regM));
			regR->pointer = 0;
			regR->sectPointer = 0;
			regR->blkPointer = getLBN(*regM);
		}
		break;
		case ERRO_EOF:{
			//nao precisa nada ?
		}
		break;
		default:
			printf("algo deu errado no switch");
			return ERRO;
	}
	
}








