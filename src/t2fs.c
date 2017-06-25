#include "../include/t2fs.h"
#include "../include/auxlib2.h"

int identify2 (char *name, int size) {
    
}

FILE2 create2 (char *filename) {
    int freeRegNum;
	REGRECORD regR;

    if (openSpots() > 0 && isValidPath(filename) == OK && fileExists(filename) == ERRO) {
        if ((freeRegNum = findFreeMFT()) != ERRO) {
            setRegType(freeRegNum,0);
			for(int i=0; i<20; i++){
				if(arquivosAbertos[i].estaAberto==OK){
					arquivosAbertos[i].handle=handleUltraMasterGenerator++;
					arquivosAbertos[i].numMFT=freeRegNum;
					arquivosAbertos[i].currentPointer=0;
					arquivosAbertos[i].estaAberto=OK;
					
					//achar registor do diretorio
					//dar write sector
					
					return arquivosAbertos[i].handle;
				}
			}
        }else return ERRO;
    }else return ERRO;
}

int delete2 (char *filename) {
    
}

FILE2 open2 (char *filename) {
    
}

int close2 (FILE2 handle) {
    
}

int read2 (FILE2 handle, char *buffer, int size) {
    
}

int write2 (FILE2 handle, char *buffer, int size) {
    
}

int truncate2 (FILE2 handle) {
    
}

int seek2 (FILE2 handle, DWORD offset) {
    
}

int mkdir2 (char *pathname) {
    
}

int rmdir2 (char *pathname) {
    
}

DIR2 opendir2 (char *pathname) {
    
}

int readdir2 (DIR2 handle, DIRENT2 *dentry) {
    
}

int closedir2 (DIR2 handle) {
    
}

