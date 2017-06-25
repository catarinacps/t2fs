#include "../include/auxlib2.h"
#include "../include/diskio.h"
#include "../include/regMFT.h"
#include "../include/regRecord.h"

//BLANK SPACE
//ass:Henrique
int openSpots() {
    int espacosAbertos = 0;
    for(int i = 0; i<20; i++) {
        if (arquivosAbertos[i].estaAberto == 0) {
            espacosAbertos++;
        }
    }

    return espacosAbertos;
}

//ass:Henrique
int isValidPath(char caminho[]) {
    char *token, regMFT[512];
	
	if (caminho[0] != '/') {
		return ERRO;
	}
	
    token = strtok(caminho, "/");

    while (token != NULL) {
        if (strspn(token, CARACTERES_VALIDOS) != strlen(token)) {
            return ERRO;
        }
        token = strtok(NULL, "/"); //NULL pq nicolas disse
    }
	
    // preicerolder
    // voltar aqui dps de fazer interface com o disco. a grafica. sim.
	// os nomes parecem ok, agora vamos testar se as pastas realmente existem
	
	return isRealPath(caminho);
	
}

// seu lugar e no museu
// talvez n seja mais uma merda
// ass: isReal
int isRealPath(char caminho[]) {
	REGMFT regM;
	REGRECORD regR;
	char buffer[51], *token, *tokenAux;

	loadMFT(&regM, 1);
	loadFirstRecord(&regR, regM);

	token = strtok(caminho, "/");
	tokenAux = token;

	if (token == NULL) {
		return ERRO;
	}
	while (token = strtok(NULL, "/")) {
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
		loadMFT(&regM, getMFTNumber(regR));
		loadFirstRecord(&regR, regM);
		tokenAux = token;
	}

	return OK;
}

int fileExists(char caminho[]) {
	REGMFT regM;
	REGRECORD regR;
	char buffer[51], *token, *tokenAux;

	loadMFT(&regM, 1);
	loadFirstRecord(&regR, regM);

	token = strtok(caminho, "/");
	tokenAux = token;

	if (token == NULL) {
		return ERRO;
	}
	while (token = strtok(NULL, "/")) {
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
		loadMFT(&regM, getMFTNumber(regR));
		loadFirstRecord(&regR, regM);
		tokenAux = token;
	}

	getRecordName(regR, buffer);
	while (strcmp(buffer, tokenAux) != 0) {
		if (nextRecord(&regR, &regM) == ERRO) {
			return ERRO;
		}
		getRecordName(regR, buffer);
	}

	if (isRecordFile(regR) == OK) {
		return OK;
	} else {
		return ERRO;
	}
}

// bunda: henrique
int findFreeMFT() {
	int numMFTreg = bootBlock.blockSize * bootBlock.MFTBlocksSize / 2;
	REGMFT regM;

	for (int i=4; i < numMFTreg; i++) {
		loadMFT(&regM, i);
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
	char bootBuffer[256];
	read_sector(0,bootBuffer);
	
	if(strncmp(bootBuffer,"T2FS",4)==0){
		if((WORD)bootBuffer[4]+(WORD)bootBuffer[5]*256==0x7E11){
			strcpy(bootBlock.id,"T2FS");
			bootBlock.version=0x7E11;
			bootBlock.blockSize=(WORD)bootBuffer[6]+(WORD)bootBuffer[7]*256);
			bootBlock.MFTBlocksSize=(WORD)bootBuffer[8]+(WORD)bootBuffer[9]*256);
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