#include "../include/t2fs.h"
#include "../include/auxlib2.h"

int identify2 (char *name, int size) {
	initLib();
    return 0;
}

FILE2 create2 (char *filename) {
    int freeRegNum;
	REGRECORD *regR, *regAvo;
	REGMFT regM;
	char *token, *aux;
	char filename2[200];

	strcpy(filename2,filename);//se nao da segmentation falut pq strtok n gosta de parametros

	initLib();

    if (openSpots() > 0 && isValidPath(filename2) == OK && fileExists(filename2, &regR, &regM, &regAvo) == MISSING_FILE) {
		if ((freeRegNum = findFreeMFT()) != ERRO) {
            setRegType(freeRegNum,0,0);
			for (int i=0; i<20; i++){
				if (arquivosAbertos[i].estaAberto == ERRO) {
					arquivosAbertos[i].handle = getHandle();
					arquivosAbertos[i].numMFT = freeRegNum;
					arquivosAbertos[i].currentPointer = 0;
					arquivosAbertos[i].estaAberto = OK;
					
					//achar registro do diretorio
					token = strtok(filename2,"/");
					do {
						aux = token;
						token = strtok(NULL,"/");
					} while(token!=NULL);
					
					writeNewFileRecord(aux, arquivosAbertos[i].numMFT, regR, &regM, regAvo);
					
					//dar write sector
					
					return arquivosAbertos[i].handle;
				}
			}
			return ERRO; //nunca chegará aki
        } else { 
			return ERRO;
		}
    } else {
		return ERRO;
	}
}

// bunda:henrique
int delete2 (char *filename) {
	initLib();
    return 0;
}

// bunda:henrique
FILE2 open2 (char *filename) {
	REGRECORD *regR, *regAvo;
	REGMFT regM;
	char *token, *aux;
	char filename2[200];

	strcpy(filename2,filename);	//se nao da segmentation falut pq strtok n gosta de parametros

	initLib();

    if (openSpots() > 0 && isValidPath(filename2) == OK && fileExists(filename2, &regR, &regM, &regAvo) == OK) {
		for (int i=0; i<20; i++){
			if (arquivosAbertos[i].estaAberto == ERRO) {
				arquivosAbertos[i].handle = getHandle();
				printf("mft regr %d mft regm %d \n", regR->regM.numMFT, regM.numMFT);
				arquivosAbertos[i].numMFT = getMFTNumber(*regR);
				arquivosAbertos[i].currentPointer = 0;
				arquivosAbertos[i].estaAberto = OK;

				return arquivosAbertos[i].handle;
			}
		}
		return ERRO; //nunca chegará aki
    } else {
		return ERRO;
	}
}

int close2 (FILE2 handle) {
	initLib();
    return 0;
}

int read2 (FILE2 handle, char *buffer, int size) {
	initLib();
    return 0;
}

int write2 (FILE2 handle, char *buffer, int size) {
	initLib();
    return 0;
}

int truncate2 (FILE2 handle) {
	initLib();
    return 0;
}

int seek2 (FILE2 handle, DWORD offset) {
	initLib();
    return 0;
}

int mkdir2 (char *pathname) {
	initLib();
    return 0;
}

int rmdir2 (char *pathname) {
	initLib();
    return 0;
}

DIR2 opendir2 (char *pathname) {
	initLib();
    return 0;
}

int readdir2 (DIR2 handle, DIRENT2 *dentry) {
	initLib();
    return 0;
}

int closedir2 (DIR2 handle) {
	initLib();
    return 0;
}

