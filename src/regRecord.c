#include "../include/regRecord.h"

// carrega a primeira entrada de diretorio e seta o ponteiro para zero
// ass:Nicolas
void loadFirstRecord(REGRECORD *regR, REGMFT regM){
	read_sector(bootBlock.blocksize * getLBN(regM) , regR->data);
	regR->pointer = 0;	// qual dos 4 diretorios dentro do setor
	regR->sectPointer = 0; // qual dos 4 setores dentro do bloco
	regR->blkPointer = getLBN(regM); 
	regR->regM=regM;
}

// passa para a proxima entrada de diretorio se puder
// a funcao e uma merda
// ass:Nicolas
void nextRecord(REGRECORD *regR, REGMFT *regM){

	if (regR->pointer < 3) {		// pode pegar 4 diretorios dentro de 1 setor
		regR->pointer++;
		return OK;	
	}
	else {
		if (regR->sectPointer < bootBlock.blocksize - 1) {	// pode pegar 4 setores dentro de 1 bloco (geralmente, depende do blocksize)
			regR->pointer = 0;
			regR->sectPointer++;
			read_sector(bootBlock.blocksize * regR->blkPointer + regR->sectPointer , regR->data);
			return OK;
		}
		else {
			if (regR->blkPointer - getLBN(*regM) < getCont(*regM) - 1) {		// temos que pegar o proximo bloco, neste caso ele é contiguo na memoria (está na mesma tupla)
				regR->blkPointer++;
				regR->pointer = 0;
				regR->sectPointer = 0;
				read_sector(bootBlock.blocksize * regR->blkPointer + regR->sectPointer , regR->data);
				return OK;	
			}
			else {			// temos que pegar o proximo bloco mas ele NAO está contiguo, entao pegar a proxima tupla
				if (nextTupla(regM) == OK) {
					if (isTuplaChain(*regM) == OK) {
						regR->pointer = 0;
						regR->sectPointer = 0;
						regR->blkPointer = getLBN(*regM);
						read_sector(bootBlock.blocksize * regR->blkPointer + regR->sectPointer , regR->data);
						return OK;
					} else if (isTuplaJmp(*regM) == OK) {		// puta merda q troco feio
						loadMFT(regM, getVBN(*regM));			// pega o registro adicional

						if (isTuplaChain(*regM) == OK) {
							regR->pointer = 0;
							regR->sectPointer = 0;
							regR->blkPointer = getLBN(*regM);
							read_sector(bootBlock.blocksize * regR->blkPointer + regR->sectPointer , regR->data);
							return OK;
						} else {
							return ERRO; //sera q cai aki? acho q nao
						}
					} else {
						backTupla(regM);
						return ERRO_EOF;	//aki acaba a tupla
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
	
	if(strcpy(regR->data[SIZERECORD*regR->pointer+1], buffer))
		return OK;
	else
		return ERRO;
}

//seta tamanho total em blocos de reg
//ass:Gabriel
int setBlocksFileSize(REGRECORD *regR, int size){
	for(int i=0;i<4;i++){
		regR->data[SIZERECORD*regR->pointer + 52 + i] = (byte)(size/pow(256,i)); //magica logica aritmetica
	}
}

//seta tamanho total em bytes de reg
//ass:Gabriel
int setBytesFileSize(REGRECORD *regR, int size){
	for(int i=0;i<4;i++){
		regR->data[SIZERECORD*regR->pointer + 56 + i] = (byte)(size/pow(256,i));
	}
}

//seta numero de registro MFT de reg
//ass:Gabriel
int setMFTNumber(REGRECORD *regR, int numMFT){
	for(int i=0;i<4;i++){
		regR->data[SIZERECORD*regR->pointer + 60 + i] = (byte)(numMFT/pow(256,i));
	}
}

//ass:Gabriel
int writeNewFileRecord(char *name, int numMFT, REGRECORD *regR, REGMFT *regM, REGRECORD *regAvo) {	//so usamos numMFT de regM
	REGMFT regM2;
	REGRECORD regR2;
	unsigned char buffer[SECTOR_SIZE];
	bool flagEOF = FALSE;
	int numFreeBlock, currentVBN, numFreeMFT;

	loadMFT(&regM2, regM->numMFT);
	loadFirstRecord(&regR2, regM2);
	
	while (isRecordFree(regR2) == ERRO && flagEOF != 1) {
		if (nextRecord(&regR2, &regM2) == ERRO_EOF) {
			flagEOF = TRUE;

			if (getBitmap2(getLBN(regM2) + getCont(regM2)) == 0) {		// achamos um novo bloco, formatamos ele e escrevemos ele
				setBitmap2(getLBN(regM2) + getCont(regM2), 1);
				setRegCont(regM2.numMFT, getCont(regM2) + 1, regM2.pointer);	// a partir daqui, regM2 esta desatualizado pra kct mlk

				for (int i=0; i < 4; i++) {
					read_sector((getLBN(regM2) + getCont(regM2)) * bootBlock.blockSize + i, buffer);
					for (int j=0; j < 4; j++) {
						buffer[SIZERECORD * j] = 0;
						buffer[SIZERECORD * j + 1] = 0;
						buffer[SIZERECORD * j + 2] = 0;
						buffer[SIZERECORD * j + 3] = 0;
					}
					write_sector((getLBN(regM2) + getCont(regM2)) * bootBlock.blockSize + i, buffer);
				}
			} else {
				if ((numFreeBlock = searchBitmap2(0)) > 0) {
					if (regM2.pointer < (NUMTUPLAS - 2)) {
						setBitmap2(numFreeBlock, 1);	// agora o bloco numFreeBlock esta ocupado no bitmap

						currentVBN = getVBN(regM2) + getCont(regM2);
						nextTupla(&regM2);
						setRegType(regM2.numMFT, 1, regM2.pointer);
						setVBN(regM2.numMFT, currentVBN, regM2.pointer);
						setLBN(regM2.numMFT, numFreeBlock, regM2.pointer);
						setRegCont(regM2.numMFT, 1, regM2.pointer);

						nextTupla(&regM2);
						setRegType(regM2.numMFT, 0, regM2.pointer);
						backTupla(&regM2);

						for (int i=0; i < 4; i++) {
							read_sector((getLBN(regM2) + getCont(regM2)) * bootBlock.blockSize + i, buffer);
							for (int j=0; j < 4; j++) {
								buffer[SIZERECORD * j] = 0;
								buffer[SIZERECORD * j + 1] = 0;
								buffer[SIZERECORD * j + 2] = 0;
								buffer[SIZERECORD * j + 3] = 0;
							}
							write_sector((getLBN(regM2) + getCont(regM2)) * bootBlock.blockSize + i, buffer);
						}
					} else {
						if ((numFreeMFT = findFreeMFT()) > 0) {
							currentVBN = getVBN(regM2) + getCont(regM2);

							nextTupla(&regM2);
							setRegType(regM2.numMFT, 2, regM2.pointer);
							setVBN(regM2.numMFT, numFreeMFT, regM2.pointer);
							loadMFT(&regM2, numFreeMFT);

							setBitmap2(numFreeBlock, 1);	// agora o bloco numFreeBlock esta ocupado no bitmap

							setRegType(regM2.numMFT, 1, regM2.pointer);
							setVBN(regM2.numMFT, currentVBN, regM2.pointer);
							setLBN(regM2.numMFT, numFreeBlock, regM2.pointer);
							setRegCont(regM2.numMFT, 1, regM2.pointer);

							nextTupla(&regM2);
							setRegType(regM2.numMFT, 0, regM2.pointer);
							backTupla(&regM2);

							for (int i=0; i < 4; i++) {
								read_sector((getLBN(regM2) + getCont(regM2)) * bootBlock.blockSize + i, buffer);
								for (int j=0; j < 4; j++) {
									buffer[SIZERECORD * j] = 0;
									buffer[SIZERECORD * j + 1] = 0;
									buffer[SIZERECORD * j + 2] = 0;
									buffer[SIZERECORD * j + 3] = 0;
								}
								write_sector((getLBN(regM2) + getCont(regM2)) * bootBlock.blockSize + i, buffer);
							}
						}
					}
				} else {
					printf("bah deu mto ruim foi mal ae\n");
					return ERRO;
				}
			}
		}
	}
	//aki regR2 tem um registro livre
	setRecordType(&regR2, 1);
	setRecordName(&regR2, name);
	setBlocksFileSize(&regR2, 0);
	setBytesFileSize(&regR2, 0);
	setMFTNumber(&regR2, numMFT);
	
	write_sector(regR2->blkPointer * bootBlock.blockSize + regR2->sectPointer, regR2->data);
	
	//falta atualizar o registro da pasta na pasta pai
	if(regAvo != NULL){
		setBytesFileSize(regAvo, getBytesFileSize(*regAvo) + SIZERECORD);
		write_sector(regAvo->blkPointer * bootBllock.blockSize + regAvo->sectPointer, regAvo->data);
	}
	
	return OK;
	
}








