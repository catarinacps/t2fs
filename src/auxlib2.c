#include "../include/auxlib2.h"
int handleUltraMasterGenerator=1;
int hasBegun=ERRO;

//BLANK SPACE
//ass:Henrique
int openSpots() {
    int espacosAbertos = 0;
    for(int i = 0; i<20; i++) {
        if (arquivosAbertos[i].estaAberto == ERRO) {
            espacosAbertos++;
        }
    }

    return espacosAbertos;
}

//ass:Henrique
int isValidPath(char caminho[]) {
    char *token, caminho2[200];
	
	strcpy(caminho2, caminho);

	if (caminho[0] != '/') {
		return ERRO;
	}
	
    token = strtok(caminho2, "/");

    while (token != NULL) {
        if (strspn(token, CARACTERES_VALIDOS) != strlen(token)) {
            return ERRO;
        }
        token = strtok(NULL, "/"); //NULL pq nicolas disse
    }
	return OK;
}

// seu lugar e no museu
// talvez n seja mais uma merda
// ass: isReal
int isRealPath(char caminho[]) {
	REGMFT regM;
	REGRECORD regR;
	char buffer[51], caminho2[200], *token, *tokenAux;

	strcpy(caminho2, caminho);

	loadMFT(&regM, 1, bootBlock.blockSize);
	loadFirstRecord(&regR, regM, bootBlock.blockSize);

	token = strtok(caminho2, "/");
	tokenAux = token;

	if (token == NULL) {
		return ERRO;
	}
	while ((token = strtok(NULL, "/"))!=NULL) {
		getRecordName(regR, buffer);
		while (strcmp(buffer, tokenAux) != 0) {
			if (nextRecord(&regR, &regM) == ERRO) {
				return ERRO;
			}
			getRecordName(regR, buffer);
		}

		if (isRecordFile(regR) == OK) {
			return ERRO;
		}
		loadMFT(&regM, getMFTNumber(regR), bootBlock.blockSize);
		loadFirstRecord(&regR, regM, bootBlock.blockSize);
		tokenAux = token;
	}

	return OK;
}


int fileExists(char caminho[], REGRECORD **regRout, REGMFT *regMout, REGRECORD **regRout2) {
	REGMFT regM;
	REGRECORD *regR, *regR2;
	char buffer[51], caminho2[200], *token, *tokenAux;
	regR=malloc(sizeof(REGRECORD));
	regR2=NULL;

	strcpy(caminho2, caminho);

	loadMFT(&regM, 1, bootBlock.blockSize);
	loadFirstRecord(regR, regM, bootBlock.blockSize);

	token = strtok(caminho2, "/");
	tokenAux = token;

	if (token == NULL) {	//caminho invalido (root)
		*regRout=regR;
		*regMout=regM;
		*regRout2=regR2;
		return ERRO;
	}
	while ((token = strtok(NULL, "/"))!=NULL) {
		getRecordName(*regR, buffer);
		while (strcmp(buffer, tokenAux) != 0) {
			if ( nextRecord(regR, &regM) == ERRO_EOF) {
				*regRout=regR;	//o caminho esta erraado, ele nao achou um dos diretorios do caminho
				*regMout=regM;
				*regRout2=regR2;
				return ERRO;
			}
			getRecordName(*regR, buffer);
		}
		regR2=regR;
		if (isRecordFile(*regR) == OK) {
			//ele achou um arquivo com o nome da pasta do caminho
			*regRout=regR;
			*regMout=regM;
			*regRout2=regR2;
			return ERRO;
		}
		loadMFT(&regM, getMFTNumber(*regR), bootBlock.blockSize);
		loadFirstRecord(regR, regM, bootBlock.blockSize);
		tokenAux = token;
	}
	
	getRecordName(*regR, buffer);
	while (strcmp(buffer, tokenAux) != 0) {
		if ( nextRecord(regR, &regM) == ERRO_EOF) {
			*regRout=regR;		//ele achou o caminho mas nao achou o arquivo; regRout o ultimo arquivo da pasta
			*regMout=regM;
			*regRout2=regR2;
			return MISSING_FILE;			//eh isso q a gnt quer no create
		}
		getRecordName(*regR, buffer);
	}

	*regRout=regR;
	*regMout=regM;
	*regRout2=regR2;
	if (isRecordFile(*regR) == OK) {
		return OK;
	} else {
		return ERRO;
	}
}

// bunda: henrique
int findFreeMFT() {
	int numMFTreg = bootBlock.blockSize * bootBlock.MFTBlocksSize / 2;
	REGMFT regM;
	printf(" blockSize=%d ", bootBlock.blockSize);
	printf(" MFTBlocksSize=%d ", bootBlock.MFTBlocksSize);

	for (int i=4; i < numMFTreg; i++) {
		loadMFT(&regM, i, bootBlock.blockSize);
		if (isTuplaFree(regM) == OK) {		// existem garantias dos seres superiores q sempre vai ter espaco livre
			return i;
		}
	}

	printf("disco lotado, mesmo que n era pra isso acontecer");
	return ERRO;
}

//ass:Gabriel
int loadBootBlock(){
	//char bootBuffer[14];
	//readBytes(14,bootBuffer,0,0);
	unsigned char bootBuffer[256];
	read_sector(0,bootBuffer);
	
	if(strncmp((char*)bootBuffer,"T2FS",4)==0){
		if((WORD)bootBuffer[4]+(WORD)bootBuffer[5]*256==0x7E11){
			strcpy(bootBlock.id,"T2FS");
			bootBlock.version=0x7E11;
			bootBlock.blockSize=(WORD)bootBuffer[6]+(WORD)bootBuffer[7]*256;
			bootBlock.MFTBlocksSize=(WORD)bootBuffer[8]+(WORD)bootBuffer[9]*256;
			bootBlock.MFTBlocksSize=(DWORD)bootBuffer[10]+(DWORD)bootBuffer[11]*256+
									(DWORD)bootBuffer[12]*65536+(DWORD)bootBuffer[13]*16777216;
		}else{
			printf("ERRO DE FORMATACAO DE DISCO\nO disco nao esta formatado direito.");
			return ERRO;
		}
	}else{
		printf("ERRO DE FORMATACAO DE DISCO\nO disco nao esta formatado direito.");
		return ERRO;
	}
	return OK;
}
//ass:Gabriel
int writeNewFileRecord(char *name, int numMFT, REGRECORD *regR, REGMFT *regM, REGRECORD *regAvo) {	//so usamos numMFT de regM
	REGMFT regM2;
	REGRECORD regR2;
	unsigned char buffer[SECTOR_SIZE];
	int flagEOF = 0;
	int numFreeBlock, currentVBN, numFreeMFT;

	loadMFT(&regM2, regM->numMFT,bootBlock.blockSize);
	loadFirstRecord(&regR2, regM2,bootBlock.blockSize);
	
	while (isRecordFree(regR2) == ERRO && flagEOF != 1) {
		if (nextRecord(&regR2, &regM2) == ERRO_EOF) {
			flagEOF = 1;

			if (getBitmap2(getLBN(regM2) + getContinuosBlocks(regM2)) == 0) {		// achamos um novo bloco, formatamos ele e escrevemos ele
				setBitmap2(getLBN(regM2) + getContinuosBlocks(regM2), 1);
				setRegCont(regM2.numMFT, getContinuosBlocks(regM2) + 1, regM2.pointer);	// a partir daqui, regM2 esta desatualizado pra kct mlk

				for (int i=0; i < 4; i++) {
					read_sector((getLBN(regM2) + getContinuosBlocks(regM2)) * bootBlock.blockSize + i, buffer);
					for (int j=0; j < 4; j++) {
						buffer[SIZERECORD * j] = 0;
						buffer[SIZERECORD * j + 1] = 0;
						buffer[SIZERECORD * j + 2] = 0;
						buffer[SIZERECORD * j + 3] = 0;
					}
					write_sector((getLBN(regM2) + getContinuosBlocks(regM2)) * bootBlock.blockSize + i, buffer);
				}
			} else {
				if ((numFreeBlock = searchBitmap2(0)) > 0) {
					if (regM2.pointer < (NUMTUPLAS - 2)) {
						setBitmap2(numFreeBlock, 1);	// agora o bloco numFreeBlock esta ocupado no bitmap

						currentVBN = getVBN(regM2) + getContinuosBlocks(regM2);
						nextTupla(&regM2);
						setRegType(regM2.numMFT, 1, regM2.pointer);
						setVBN(regM2.numMFT, currentVBN, regM2.pointer);
						setLBN(regM2.numMFT, numFreeBlock, regM2.pointer);
						setRegCont(regM2.numMFT, 1, regM2.pointer);

						nextTupla(&regM2);
						setRegType(regM2.numMFT, 0, regM2.pointer);
						backTupla(&regM2);

						for (int i=0; i < 4; i++) {
							read_sector((getLBN(regM2) + getContinuosBlocks(regM2)) * bootBlock.blockSize + i, buffer);
							for (int j=0; j < 4; j++) {
								buffer[SIZERECORD * j] = 0;
								buffer[SIZERECORD * j + 1] = 0;
								buffer[SIZERECORD * j + 2] = 0;
								buffer[SIZERECORD * j + 3] = 0;
							}
							write_sector((getLBN(regM2) + getContinuosBlocks(regM2)) * bootBlock.blockSize + i, buffer);
						}
					} else {
						if ((numFreeMFT = findFreeMFT()) > 0) {
							currentVBN = getVBN(regM2) + getContinuosBlocks(regM2);

							nextTupla(&regM2);
							setRegType(regM2.numMFT, 2, regM2.pointer);
							setVBN(regM2.numMFT, numFreeMFT, regM2.pointer);
							loadMFT(&regM2, numFreeMFT, bootBlock.blockSize);

							setBitmap2(numFreeBlock, 1);	// agora o bloco numFreeBlock esta ocupado no bitmap

							setRegType(regM2.numMFT, 1, regM2.pointer);
							setVBN(regM2.numMFT, currentVBN, regM2.pointer);
							setLBN(regM2.numMFT, numFreeBlock, regM2.pointer);
							setRegCont(regM2.numMFT, 1, regM2.pointer);

							nextTupla(&regM2);
							setRegType(regM2.numMFT, 0, regM2.pointer);
							backTupla(&regM2);

							for (int i=0; i < 4; i++) {
								read_sector((getLBN(regM2) + getContinuosBlocks(regM2)) * bootBlock.blockSize + i, buffer);
								for (int j=0; j < 4; j++) {
									buffer[SIZERECORD * j] = 0;
									buffer[SIZERECORD * j + 1] = 0;
									buffer[SIZERECORD * j + 2] = 0;
									buffer[SIZERECORD * j + 3] = 0;
								}
								write_sector((getLBN(regM2) + getContinuosBlocks(regM2)) * bootBlock.blockSize + i, buffer);
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
	
	write_sector(regR2.blkPointer * bootBlock.blockSize + regR2.sectPointer, regR2.data);
	
	//falta atualizar o registro da pasta na pasta pai
	if(regAvo != NULL){
		setBytesFileSize(regAvo, getBytesFileSize(*regAvo) + SIZERECORD);
		write_sector(regAvo->blkPointer * bootBlock.blockSize + regAvo->sectPointer, regAvo->data);
	}
	
	return OK;
	
}

//ass:gabriel
int getHandle(){
	return handleUltraMasterGenerator++;
}

//ass:gabriel
void initLib(){
	int i;
	if(hasBegun!=OK){
		loadBootBlock();
		hasBegun=OK;
		for(i=0; i<20; i++){
			arquivosAbertos[i].estaAberto=ERRO;
		}
		diretoriosAbertos=initLista();	
	}
		
}
