#include "../include/auxlib2.h"
#include "../include/diskio.h"

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
int isValidPath(char caminho[]){
    char *token, woods[2], regMFT[512];
	

    strcpy(woods, "/");
    token = strtok(caminho, woods);

    while (token != NULL) {
        if (strspn(token, CARACTERES_VALIDOS) != strlen(token)) {
            return ERRO;
        }
        token = strtok(NULL, woods); //NULL pq nicolas disse
    }
	
    // preicerolder
    // voltar aqui dps de fazer interface com o disco. a grafica. sim.
	readBytes(512,regMFT,bootBlock.blocksSize+2,0);//512=tamanho de registroMFT
	
}

//ass:Gabriel
int booooooootao(){
	char bootBuffer[14];
	readBytes(14,bootBuffer,0,0);
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
		}
	}else{
		printf("ERRO DE FORMATACAO DE DISCO\nO disco nao esta formatado direito.");
	}
}