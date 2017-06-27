#include "../include/t2fs.h"
#include "../include/auxlib2.h"

int identify2 (char *name, int size) {
    
}

FILE2 create2 (char *filename) {
    int freeRegNum;
	REGRECORD regR, regAvo;
	REGMFT regM;
	char *token, *aux;

    if (openSpots() > 0 && isValidPath(filename) == OK && fileExists(filename, &regR, &regM, &regAvo) == MISSING_FILE) {
        if ((freeRegNum = findFreeMFT()) != ERRO) {
            setRegType(freeRegNum,0);
			for (int i=0; i<20; i++){
				if (arquivosAbertos[i].estaAberto == ERRO) {
					arquivosAbertos[i].handle = handleUltraMasterGenerator++;
					arquivosAbertos[i].numMFT = freeRegNum;
					arquivosAbertos[i].currentPointer = 0;
					arquivosAbertos[i].estaAberto = OK;
					
					//achar registro do diretorio
					token = strtok(filename,"/");
					do {
						aux = token;
						token = strtok(NULL,"/");
					} while(token!=NULL);
					
					writeNewFileRecord(aux, arquivosAbertos[i].numMFT, &regR, &regM, &regAvo);
					
					//dar write sector
					
					return arquivosAbertos[i].handle;
				}
			}
        } else { 
			return ERRO;
		}
    } else {
		return ERRO;
	}
}

int delete2 (char *filename) {
    return 0;
}

FILE2 open2 (char *filename) {
    return 0;
}

int close2 (FILE2 handle) {
    return 0;
}

int read2 (FILE2 handle, char *buffer, int size) {
    return 0;
}

int write2 (FILE2 handle, char *buffer, int size) {
    return 0;
}

int truncate2 (FILE2 handle) {
    return 0;
}

int seek2 (FILE2 handle, DWORD offset) {
    return 0;
}

int mkdir2 (char *pathname) {
    return 0;
}

int rmdir2 (char *pathname) {
    return 0;
}

DIR2 opendir2 (char *pathname) {
    return 0;
}

int readdir2 (DIR2 handle, DIRENT2 *dentry) {
    return 0;
}

int closedir2 (DIR2 handle) {
    return 0;
}

