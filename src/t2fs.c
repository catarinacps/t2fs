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
					strcpy(arquivosAbertos[i].path,filename2);
					
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
// departamento de engenharia eletrica e uma bosta
// s2 brum asdasdawdasdawdasdawd
int delete2 (char *filename) {
	REGRECORD *regR, *regAvo;
	REGMFT regM, regMfile, regMemes;
	char filename2[200];
	int diretoriosBloco = 0, tuplaCounter = 0, lastMFTbeforeJump;

	unsigned char buffer[SECTOR_SIZE];

	strcpy(filename2,filename);	//se nao da segmentation falut pq strtok n gosta de parametros

	initLib();

	if (isValidPath(filename2) == OK && fileExists(filename2, &regR, &regM, &regAvo) == OK) {
		for (int i=0; i<20; i++) {
			if (arquivosAbertos[i].numMFT == getMFTNumber(*regR)) {
				close2(arquivosAbertos[i].handle);
			}
		}

		if (regAvo != NULL) {
			setBytesFileSize(regAvo, getBytesFileSize(*regAvo) - SIZERECORD);
			write_sector(regAvo->blkPointer * bootBlock.blockSize + regAvo->sectPointer, regAvo->data);
		}

		loadMFT(&regMfile, getMFTNumber(*regR), bootBlock.blockSize);

		while (isTuplaEnd(regMfile) == ERRO) {
			if (isTuplaJmp(regMfile) == OK) {
				setRegType(getMFTNumber(*regR), -1, 0);
				loadMFT(&regMfile, getVBN(regMfile), bootBlock.blockSize);
			} else {
				setRegType(getMFTNumber(*regR), -1, 0);
				for (int i=0; i<getContinuosBlocks(regMfile); i++) {
					setBitmap2(getLBN(regMfile) + i, 0);
				}
				nextTupla(&regMfile);
			}
		}
		setRegType(getMFTNumber(*regR), -1, 0);

		setRecordType(regR, 0);
		write_sector(bootBlock.blockSize * regR->blkPointer + regR->sectPointer, regR->data);

		// agora veremos se e necessario cortar blocos do diretorio apos deletar o record
		// XP INCOMING >>>>> :fire: :fire: :top: :ok_hand:

		for (regR->sectPointer=0; regR->sectPointer<bootBlock.blockSize; regR->sectPointer++) {
			read_sector(bootBlock.blockSize * regR->blkPointer + regR->sectPointer , regR->data);
			for (regR->pointer=0; regR->pointer<4; regR->pointer++) {
				if (isRecordFree(*regR) != OK) {
					diretoriosBloco++;
				}
			}
		}

		if (diretoriosBloco == 0) {
			regMemes = regR->regM;
			lastMFTbeforeJump = regMemes.numMFT;

			while (isTuplaEnd(regMemes) == ERRO) {
				if (isTuplaJmp(regMemes) == OK) {
					tuplaCounter = 0;
					lastMFTbeforeJump = regMemes.numMFT;
					loadMFT(&regMemes, getVBN(regMemes), bootBlock.blockSize);
				} else {
					tuplaCounter++;
					nextTupla(&regMemes);
				}
			}
			tuplaCounter--;
			backTupla(&regMemes);

			for (int i=0; i<bootBlock.blockSize; i++) {
				read_sector(bootBlock.blockSize * (getLBN(regMemes) + getContinuosBlocks(regMemes) - 1) + i, buffer);
				write_sector(bootBlock.blockSize * regR->blkPointer + i, buffer);
			}

			if (getContinuosBlocks(regMemes) > 1) {
				setRegCont(regMemes.numMFT, getContinuosBlocks(regMemes) - 1, tuplaCounter);
				setBitmap2(getContinuosBlocks(regMemes) + getLBN(regMemes), 0);
			} else {
				setBitmap2(getLBN(regMemes), 0);
				if (tuplaCounter > 0) {
					setRegType(regMemes.numMFT, 0, tuplaCounter);
					setRegType(regMemes.numMFT, -1, tuplaCounter + 1);
				} else {
					setRegType(regMemes.numMFT, -1, tuplaCounter);
					setRegType(regMemes.numMFT, -1, tuplaCounter + 1);
					setRegType(lastMFTbeforeJump, 0, 31);	// niemiec acha que nao
				}
			}
		}

		return OK;
    } else {
		return ERRO;
	}
	
    return 0;
}

// bunda:henrique
FILE2 open2 (char *filename) {
	REGRECORD *regR, *regAvo;
	REGMFT regM;
	char filename2[200];

	strcpy(filename2,filename);	//se nao da segmentation falut pq strtok n gosta de parametros

	initLib();

    if (openSpots() > 0 && isValidPath(filename2) == OK && fileExists(filename2, &regR, &regM, &regAvo) == OK) {
		for (int i=0; i<20; i++) {
			if (arquivosAbertos[i].numMFT == getMFTNumber(*regR)) {
				return ERRO;
			}
		}
		for (int i=0; i<20; i++){
			if (arquivosAbertos[i].estaAberto == ERRO) {
				arquivosAbertos[i].handle = getHandle();
				arquivosAbertos[i].numMFT = getMFTNumber(*regR);
				arquivosAbertos[i].currentPointer = 0;
				arquivosAbertos[i].estaAberto = OK;
				strcpy(arquivosAbertos[i].path,filename2);

				return arquivosAbertos[i].handle;
			}
		}
		return ERRO; //nunca chegará aki
    } else {
		return ERRO;
	}
}

// bunda: henrique
int close2 (FILE2 handle) {
	initLib();

	for (int i=0; i<20; i++) {
		if (arquivosAbertos[i].handle == handle) {
			arquivosAbertos[i].currentPointer = 0;
			arquivosAbertos[i].estaAberto = ERRO;
			arquivosAbertos[i].handle = 0;
			arquivosAbertos[i].numMFT = 0;

			return OK;
		}
	}

	return ERRO;	// existem casos em que chega aqui
}

int read2 (FILE2 handle, char *buffer, int size) {
	REGMFT regM, regMfile;
	REGRECORD *regR, *regAvo;
	int VBNatual, achou=ERRO, VBNfinal, corteInicio, bytesLidos;
	char *nossoBuffer;


	initLib();
	if(size==0)
		return 0;
	if(size<0)
		return ERRO;
	
	for (int i=0; i<20; i++) {
		if (arquivosAbertos[i].handle == handle) {

			fileExists(arquivosAbertos[i].path, &regR, &regM, &regAvo);
			loadMFT(&regMfile, getMFTNumber(*regR), bootBlock.blockSize);
			
			if(size + arquivosAbertos[i].currentPointer < getBytesFileSize(*regR)){
				VBNatual=arquivosAbertos[i].currentPointer / (bootBlock.blockSize * SIZE_SECTOR);
				while (achou == ERRO) {
					if (isTuplaJmp(regMfile) == OK) {
						loadMFT(&regMfile, getVBN(regMfile), bootBlock.blockSize);
					} else {
						if(VBNatual >= getVBN(regMfile) && VBNatual < getVBN(regMfile) + getContinuosBlocks(regMfile)){
							achou=OK;
						}else{
							nextTupla(&regMfile);
						}
					}
				}
				VBNfinal=(arquivosAbertos[i].currentPointer + size) / (bootBlock.blockSize * SIZE_SECTOR);
				nossoBuffer=malloc(VBNfinal - VBNatual +1);

				corteInicio = arquivosAbertos[i].currentPointer - VBNatual * bootBlock.blockSize * SECTOR_SIZE;

				for(int j=VBNatual; j<=VBNfinal; j++){
					readBloco(regMfile, j, nossoBuffer + (j - VBNatual) * bootBlock.blockSize);
				}

				arquivosAbertos[i].currentPointer += size;

				strncpy(buffer, nossoBuffer + corteInicio, size);

				return size;
			} else {
				VBNatual=arquivosAbertos[i].currentPointer / (bootBlock.blockSize * SIZE_SECTOR);
				while (achou == ERRO) {
					if (isTuplaJmp(regMfile) == OK) {
						loadMFT(&regMfile, getVBN(regMfile), bootBlock.blockSize);
					} else {
						if(VBNatual >= getVBN(regMfile) && VBNatual < getVBN(regMfile) + getContinuosBlocks(regMfile)){
							achou=OK;
						}else{
							nextTupla(&regMfile);
						}
					}
				}
				VBNfinal=(getBytesFileSize(*regR)) / (bootBlock.blockSize * SIZE_SECTOR);
				nossoBuffer=malloc(VBNfinal - VBNatual +1);

				corteInicio = arquivosAbertos[i].currentPointer - VBNatual * bootBlock.blockSize * SECTOR_SIZE;

				for(int j=VBNatual; j<=VBNfinal; j++){
					readBloco(regMfile, j, nossoBuffer + (j - VBNatual) * bootBlock.blockSize);
				}
				bytesLidos=getBytesFileSize(*regR) - arquivosAbertos[i].currentPointer;
				arquivosAbertos[i].currentPointer = getBlocksFileSize(*regR);

				strncpy(buffer, nossoBuffer + corteInicio, bytesLidos);

				return bytesLidos;
			}
		}
	}
	return ERRO;

}

int write2 (FILE2 handle, char *buffer, int size) {
	initLib();
    return ERRO;
}

int truncate2 (FILE2 handle) {
	initLib();
    return ERRO;
}

//ass:gabriel
int seek2 (FILE2 handle, DWORD offset) {
	REGMFT regM;
	REGRECORD *regR, *regAvo;

	initLib();
	for (int i=0; i<20; i++) {
		if (arquivosAbertos[i].handle == handle) {
			fileExists(arquivosAbertos[i].path, &regR, &regM, &regAvo);
			if(offset > getBytesFileSize(*regR)){
				return ERRO;
			}
			else if(offset == -1){
				arquivosAbertos[i].currentPointer = getBytesFileSize(*regR);
			}else{
				arquivosAbertos[i].currentPointer = offset;
			}

			return OK;
		}
	}
    return ERRO;
}

int mkdir2 (char *pathname) {
	initLib();
    return ERRO;
}

int rmdir2 (char *pathname) {
	initLib();
    return ERRO;
}

DIR2 opendir2 (char *pathname) {
	initLib();
    return ERRO;
}

int readdir2 (DIR2 handle, DIRENT2 *dentry) {
	initLib();
    return ERRO;
}

int closedir2 (DIR2 handle) {
	initLib();
    return ERRO;
}

