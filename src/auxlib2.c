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

//ass: henrique
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
		if(regR2==NULL){
			regR2=malloc(sizeof(REGRECORD));
		}
		*regR2=*regR;
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
			return MISSING_FILE;			//eh isso q a gnt quer no create e no mkdir
		}
		getRecordName(*regR, buffer);
	}

	*regRout=regR;
	*regMout=regM;
	*regRout2=regR2;
	if (isRecordFile(*regR) == OK) {
		return OK;
	} else {
		return IS_A_DIR;
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
int writeNewRecord(char *name, int numMFT, REGRECORD *regR, REGMFT *regM, REGRECORD *regAvo, int tipo) {	//so usamos numMFT de regM
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
				setRegCont(regM2.numMFT, getContinuosBlocks(regM2) + 1, regM2.pointer, &regM2);

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
						setRegType(regM2.numMFT, 1, regM2.pointer, &regM2);
						setVBN(regM2.numMFT, currentVBN, regM2.pointer,&regM2);
						setLBN(regM2.numMFT, numFreeBlock, regM2.pointer,&regM2);
						setRegCont(regM2.numMFT, 1, regM2.pointer,&regM2);

						nextTupla(&regM2);
						setRegType(regM2.numMFT, 0, regM2.pointer, &regM2);
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
							setRegType(regM2.numMFT, 2, regM2.pointer, &regM2);
							setVBN(regM2.numMFT, numFreeMFT, regM2.pointer,&regM2);
							loadMFT(&regM2, numFreeMFT, bootBlock.blockSize);

							setBitmap2(numFreeBlock, 1);	// agora o bloco numFreeBlock esta ocupado no bitmap

							setRegType(regM2.numMFT, 1, regM2.pointer, &regM2);
							setVBN(regM2.numMFT, currentVBN, regM2.pointer,&regM2);
							setLBN(regM2.numMFT, numFreeBlock, regM2.pointer,&regM2);
							setRegCont(regM2.numMFT, 1, regM2.pointer,&regM2);

							nextTupla(&regM2);
							setRegType(regM2.numMFT, 0, regM2.pointer, &regM2);
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
					printf("\nbah deu mto ruim foi mal ae\n");
					return ERRO;
				}
			}
		}
	}
	//aki regR2 tem um registro livre
	if(tipo==IS_FILE){
		setRecordType(&regR2, 1);
		setBlocksFileSize(&regR2, 0);	
	}else{
		setRecordType(&regR2, 2);
		setBlocksFileSize(&regR2, 1);
	}
	
	
	setRecordName(&regR2, name);
	setBytesFileSize(&regR2, 0);
	setMFTNumber(&regR2, numMFT);
	
	write_sector(regR2.blkPointer * bootBlock.blockSize + regR2.sectPointer, regR2.data);
	
	//falta atualizar o registro da pasta na pasta pai
	if(regAvo != NULL){
		
		setBytesFileSize(regAvo, getBytesFileSize(*regAvo) + SIZERECORD);
		printf("\n size: %d", getBytesFileSize(*regAvo));
		write_sector(regAvo->blkPointer * bootBlock.blockSize + regAvo->sectPointer, regAvo->data);
		printf("\n setor:%d blkptr: %d sctptr:%d", regAvo->blkPointer * bootBlock.blockSize + regAvo->sectPointer, regAvo->blkPointer, regAvo->sectPointer);
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
		diretoriosAbertos=initLista;
		hasBegun=OK;
		for(i=0; i<20; i++){
			arquivosAbertos[i].estaAberto=ERRO;
		}
		diretoriosAbertos=initLista();	
	}
		
}

//ass:gabriel o cansado
int readBlock(REGMFT regM, int VBN, char *buffer){
	if(VBN >= getVBN(regM) && VBN < getVBN(regM) + getContinuosBlocks(regM)){
		for (int i=0; i<bootBlock.blockSize; i++) {
			read_sector(bootBlock.blockSize * getLBN(regM) + i, (unsigned char *) buffer + i * SECTOR_SIZE);
		}

		return OK;
		
	}else{
		return ERRO;
	}
}

// bunda: henrique
int writeBlock(REGMFT regM, int VBN, char *buffer) {
	if(VBN >= getVBN(regM) && VBN < getVBN(regM) + getContinuosBlocks(regM)){
		for (int i=0; i<bootBlock.blockSize; i++) {
			write_sector(bootBlock.blockSize * getLBN(regM) + i, (unsigned char *) buffer + i * SECTOR_SIZE);
		}

		return OK;
	} else {
		return ERRO;
	}
}

//---------------------Funcs de lista-------------------------
//ass: gabriel o obstinado

ODIN* findDir(DIR2 handle){
	ODIN *dir;
	if(diretoriosAbertos==NULL){
		return NULL;
	}else{
		while(diretoriosAbertos->prox != NULL){
			if(((ODIN*)diretoriosAbertos->dados)->handle == handle){
				dir=(ODIN*)diretoriosAbertos->dados;
				diretoriosAbertos=getFirstNodeLista(diretoriosAbertos);
				return dir;
			}else{
				diretoriosAbertos=diretoriosAbertos->prox;
			}
		}
		if(((ODIN*)diretoriosAbertos->dados)->handle==handle){
			dir=(ODIN*)diretoriosAbertos->dados;
			diretoriosAbertos=getFirstNodeLista(diretoriosAbertos);
			return dir;
		}else{
			diretoriosAbertos=getFirstNodeLista(diretoriosAbertos);
			return NULL;
		}
	}
}

int isOpenDir(char *pathname){
	if(diretoriosAbertos==NULL){
		return ERRO;
	}else{
		while(diretoriosAbertos->prox != NULL){
			if(strcmp(pathname, ((ODIN*)diretoriosAbertos->dados)->path) == OK){
				diretoriosAbertos=getFirstNodeLista(diretoriosAbertos);
				return OK;
			}else{
				diretoriosAbertos=diretoriosAbertos->prox;
			}
		}
		if(strcmp(pathname, ((ODIN*)diretoriosAbertos->dados)->path) == OK){
			diretoriosAbertos=getFirstNodeLista(diretoriosAbertos);
			return OK;
		}else{
			diretoriosAbertos=getFirstNodeLista(diretoriosAbertos);
			return ERRO;
		}
	}
}


DIR2 insertDir(int numMFT, char *pathname){
	ODIN *dir = malloc(sizeof(ODIN));

	dir->handle=getHandle();
	strcpy(dir->pathname, pathname);
	dir->numMFT=numMFT;
	dir->currentpointer=0;

	diretoriosAbertos=insertLista(diretoriosAbertos, (void *)dir);

	return dir->handle;
}

int removeDir(DIR handle){
	ODIN *dir;
	int i=0;
	if(diretoriosAbertos==NULL){
		return ERRO;
	}else{
		while(diretoriosAbertos->prox != NULL){
			if(((ODIN*)diretoriosAbertos->dados)->handle == handle){
				diretoriosAbertos=removeLista(diretoriosAbertos, i);
				diretoriosAbertos=getFirstNodeLista(diretoriosAbertos);
				return OK;
			}else{
				diretoriosAbertos=diretoriosAbertos->prox;
				i++;
			}
		}
		if(((ODIN*)diretoriosAbertos->dados)->handle==handle){
			diretoriosAbertos=removeLista(diretoriosAbertos, i);
			diretoriosAbertos=getFirstNodeLista(diretoriosAbertos);
			return OK;
		}else{
			diretoriosAbertos=getFirstNodeLista(diretoriosAbertos);
			return ERRO;
		}
	}
}


//-----------------------------------------------------------
