#include "../include/t2fs.h"
#include "../include/auxlib2.h"

int identify2 (char *name, int size) {
	initLib();
    return 0;
}

FILE2 create2 (char *filename) {
    int freeRegNum;
	REGRECORD *regR, *regAvo;
	REGMFT regM, regM2;
	char *token, *aux;
	char filename2[200];

	strcpy(filename2,filename);//se nao da segmentation falut pq strtok n gosta de parametros

	initLib();

    if (openSpots() > 0 && isValidPath(filename2) == OK && fileExists(filename2, &regR, &regM, &regAvo) == MISSING_FILE) {
		if ((freeRegNum = findFreeMFT()) != ERRO) {
			loadMFT(&regM2, freeRegNum, bootBlock.blockSize);
            setRegType(freeRegNum,0,0, &regM2);
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
					
					writeNewRecord(aux, arquivosAbertos[i].numMFT, regR, &regM, regAvo, IS_FILE);
					
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
	REGMFT regM, regMfile, regMemes, regM2;
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
				loadMFT(&regM2,getMFTNumber(*regR),bootBlock.blockSize);
				setRegType(getMFTNumber(*regR), -1, 0, &regM2);
				loadMFT(&regMfile, getVBN(regMfile), bootBlock.blockSize);
			} else {
				loadMFT(&regM2,getMFTNumber(*regR),bootBlock.blockSize);
				setRegType(getMFTNumber(*regR), -1, 0, &regM2);
				for (int i=0; i<getContinuosBlocks(regMfile); i++) {
					setBitmap2(getLBN(regMfile) + i, 0);
				}
				nextTupla(&regMfile);
			}
		}
		loadMFT(&regM2,getMFTNumber(*regR),bootBlock.blockSize);
		setRegType(getMFTNumber(*regR), -1, 0, &regM2);

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
				setRegCont(regMemes.numMFT, getContinuosBlocks(regMemes) - 1, tuplaCounter,&regMemes);
				setBitmap2(getContinuosBlocks(regMemes) + getLBN(regMemes), 0);
			} else {
				setBitmap2(getLBN(regMemes), 0);
				if (tuplaCounter > 0) {
					setRegType(regMemes.numMFT, 0, tuplaCounter, &regMemes);
					setRegType(regMemes.numMFT, -1, tuplaCounter + 1, &regMemes);
				} else {
					setRegType(regMemes.numMFT, -1, tuplaCounter, &regMemes);
					setRegType(regMemes.numMFT, -1, tuplaCounter + 1, &regMemes);
					loadMFT(&regM2,lastMFTbeforeJump,bootBlock.blockSize);
					setRegType(lastMFTbeforeJump, 0, 31, &regM2);	// niemiec acha que nao
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

			if (arquivosAbertos[i].currentPointer >= getBytesFileSize(*regR)) {
				return ERRO;
			}
			
			if(size + arquivosAbertos[i].currentPointer < getBytesFileSize(*regR)){
				VBNatual=arquivosAbertos[i].currentPointer / (bootBlock.blockSize * SECTOR_SIZE);
				while (achou == ERRO) {
					if (isTuplaJmp(regMfile) == OK) {
						loadMFT(&regMfile, getVBN(regMfile), bootBlock.blockSize);
					} else {
						if(VBNatual >= getVBN(regMfile) && VBNatual < (getVBN(regMfile) + getContinuosBlocks(regMfile))){
							achou=OK;
						}else{
							nextTupla(&regMfile);
						}
					}
				}
				VBNfinal=(arquivosAbertos[i].currentPointer + size) / (bootBlock.blockSize * SECTOR_SIZE);
				nossoBuffer=malloc((VBNfinal - VBNatual +1) * (bootBlock.blockSize * SECTOR_SIZE));

				corteInicio = arquivosAbertos[i].currentPointer - VBNatual * bootBlock.blockSize * SECTOR_SIZE;

				for(int j=VBNatual; j<=VBNfinal; j++){
					if (readBlock(regMfile, j, nossoBuffer + (j - VBNatual) * bootBlock.blockSize * SECTOR_SIZE) == ERRO) {
						nextTupla(&regMfile);
						if (isTuplaJmp(regMfile) == OK) {
							loadMFT(&regMfile, getVBN(regMfile), bootBlock.blockSize);
						}
					}
				}

				arquivosAbertos[i].currentPointer += size;

				strncpy(buffer, nossoBuffer + corteInicio, size);

				return size;
			} else {	// se o ponteiro saisse do arquivo
				VBNatual=arquivosAbertos[i].currentPointer / (bootBlock.blockSize * SECTOR_SIZE);
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
				VBNfinal = (getBytesFileSize(*regR)) / (bootBlock.blockSize * SECTOR_SIZE);
				nossoBuffer = malloc((VBNfinal - VBNatual +1) * (bootBlock.blockSize * SECTOR_SIZE));

				corteInicio = arquivosAbertos[i].currentPointer - VBNatual * bootBlock.blockSize * SECTOR_SIZE;

				for(int j=VBNatual; j<=VBNfinal; j++){
					if (readBlock(regMfile, j, nossoBuffer + (j - VBNatual) * bootBlock.blockSize * SECTOR_SIZE) == ERRO) {
						nextTupla(&regMfile);
						if (isTuplaJmp(regMfile) == OK) {
							loadMFT(&regMfile, getVBN(regMfile), bootBlock.blockSize);
						}
					}
				}
				bytesLidos = getBytesFileSize(*regR) - arquivosAbertos[i].currentPointer;
				arquivosAbertos[i].currentPointer = getBlocksFileSize(*regR);

				strncpy(buffer, nossoBuffer + corteInicio, bytesLidos);

				return bytesLidos;
			}
		}
	}
	return ERRO;

}

//bunda nicolas
int write2 (FILE2 handle, char *buffer, int size) {
	REGMFT regM, regMfile, regMfileAux;
	REGRECORD *regR, *regAvo;
	int VBNatual, achou=ERRO, VBNfinal, VBNeof, corteInicio, newBlocks, freeBlock, freeMFT;
	char *nossoBuffer;


	initLib();
	
	if (size == 0) {
		return 0;
	}
	if (size < 0) {
		return ERRO;
	}
	
	for (int i=0; i<20; i++) {
		if (arquivosAbertos[i].handle == handle) {

			fileExists(arquivosAbertos[i].path, &regR, &regM, &regAvo);
			loadMFT(&regMfile, getMFTNumber(*regR), bootBlock.blockSize);
			
			VBNatual = arquivosAbertos[i].currentPointer / (bootBlock.blockSize * SECTOR_SIZE);
			VBNfinal = (arquivosAbertos[i].currentPointer + size) / (bootBlock.blockSize * SECTOR_SIZE);
			VBNeof = (getBytesFileSize(*regR) - 1) / (bootBlock.blockSize * SECTOR_SIZE);
			
			nossoBuffer = malloc((VBNfinal - VBNatual +1) * (bootBlock.blockSize * SECTOR_SIZE));
			corteInicio = arquivosAbertos[i].currentPointer - VBNatual * bootBlock.blockSize * SECTOR_SIZE;

			if(getBytesFileSize(*regR) == 0){
				setRegType(regMfile.numMFT, 1, regMfile.pointer,&regMfile);
				setRegType(regMfile.numMFT, 0, regMfile.pointer + 1,&regMfile);
				setVBN(regMfile.numMFT, 0, regMfile.pointer,&regMfile);
				if((freeBlock = searchBitmap2(0)) > 0){
					setBitmap2(freeBlock, 1);
					setLBN(regMfile.numMFT, freeBlock, regMfile.pointer,&regMfile);
				}
				else{
					printf("CABOU O DISCO <3");
					return ERRO;
				}
				setRegCont(regMfile.numMFT, 1, regMfile.pointer,&regMfile);
				//printf("\ngetVBN:%d VBNatual:%d getCont:%d BFS:%d ", getVBN(regMfile), VBNatual, getContinuosBlocks(regMfile), getBytesFileSize(*regR));
				//return ERRO;
			}
			while (achou == ERRO) {
				if (isTuplaJmp(regMfile) == OK) {
					loadMFT(&regMfile, getVBN(regMfile), bootBlock.blockSize);
				} else {
					printf("\ngetVBN:%d VBNatual:%d getCont:%d BFS:%d ", getVBN(regMfile), VBNatual, getContinuosBlocks(regMfile), getBytesFileSize(*regR));
					if(VBNatual >= getVBN(regMfile) && VBNatual < getVBN(regMfile) + getContinuosBlocks(regMfile)){
						achou=OK;
					}else{
						nextTupla(&regMfile);
					}
				}
			}
			
			if (VBNfinal <= VBNeof){
				
				regMfileAux = regMfile;
				
				for(int j=VBNatual; j<=VBNfinal; j++){
					if (readBlock(regMfile, j, nossoBuffer + (j - VBNatual) * bootBlock.blockSize * SECTOR_SIZE) == ERRO) {
						nextTupla(&regMfile);
						if (isTuplaJmp(regMfile) == OK) {
							loadMFT(&regMfile, getVBN(regMfile), bootBlock.blockSize);
						}
					}
				}
				
				strncpy(nossoBuffer + corteInicio, buffer, size);
				
				for(int j=VBNatual; j<=VBNfinal; j++){
					if (writeBlock(regMfileAux, j, nossoBuffer + (j - VBNatual) * bootBlock.blockSize * SECTOR_SIZE) == ERRO) {
						nextTupla(&regMfileAux);
						if (isTuplaJmp(regMfileAux) == OK) {
							loadMFT(&regMfileAux, getVBN(regMfileAux), bootBlock.blockSize);
						}
					}
				}
				
				arquivosAbertos[i].currentPointer += size;
				if(arquivosAbertos[i].currentPointer > getBytesFileSize(*regR)){
					setBytesFileSize(regR, arquivosAbertos[i].currentPointer);
					write_sector(bootBlock.blockSize * regR->blkPointer + regR->sectPointer, regR->data);
				}
				
				return size;
				
			}else{
				newBlocks = VBNfinal - VBNeof;
				
				while (isTuplaEnd(regMfileAux) == ERRO) {
					if (isTuplaJmp(regMfileAux) == OK) {
						loadMFT(&regMfileAux, getVBN(regMfileAux), bootBlock.blockSize);
					} else {
						nextTupla(&regMfileAux);
					}
				}
				backTupla(&regMfileAux);  // encontrou a ultima tupla usada do registro
				for(int j = 0; j < newBlocks; j++){
					if(getBitmap2(getLBN(regMfileAux) + getContinuosBlocks(regMfileAux)) == 0){
						setBitmap2(getLBN(regMfileAux) + getContinuosBlocks(regMfileAux), 1);
						setRegCont(regMfileAux.numMFT, getContinuosBlocks(regMfileAux) + 1, regMfileAux.pointer,&regMfileAux);
					}
					else{
						nextTupla(&regMfileAux);
						if(regMfileAux.pointer < NUMTUPLAS - 1){
							setRegType(regMfileAux.numMFT, 1, regMfileAux.pointer,&regMfileAux);
							setRegType(regMfileAux.numMFT, 0, regMfileAux.pointer + 1,&regMfileAux);
							setVBN(regMfileAux.numMFT, VBNeof + 1 + j, regMfileAux.pointer,&regMfileAux);
							if((freeBlock = searchBitmap2(0)) > 0){
								setBitmap2(freeBlock, 1);
								setLBN(regMfileAux.numMFT, freeBlock, regMfileAux.pointer,&regMfileAux);
							}
							else{
								printf("CABOU O DISCO <3");
								return ERRO;
							}
							setRegCont(regMfileAux.numMFT, 1, regMfileAux.pointer,&regMfileAux);
						
						}
						else{  // jump time
							if((freeMFT = findFreeMFT()) == ERRO){
								return ERRO;
							}
							else{
								setRegType(regMfileAux.numMFT, 2, regMfileAux.pointer + 1,&regMfileAux);
								setVBN(regMfileAux.numMFT, freeMFT, regMfileAux.pointer + 1,&regMfileAux);
								loadMFT(&regMfileAux, freeMFT, bootBlock.blockSize);
								
								setRegType(regMfileAux.numMFT, 1, regMfileAux.pointer,&regMfileAux);
								setRegType(regMfileAux.numMFT, 0, regMfileAux.pointer + 1,&regMfileAux);
								setVBN(regMfileAux.numMFT, VBNeof + 1 + j, regMfileAux.pointer,&regMfileAux);
								if((freeBlock = searchBitmap2(0)) > 0){
									setBitmap2(freeBlock, 1);
									setLBN(regMfileAux.numMFT, freeBlock, regMfileAux.pointer,&regMfileAux);
								}
								else{
									printf("CABOU O DISCO <3");
									return ERRO;
								}
								setRegCont(regMfileAux.numMFT, 1, regMfileAux.pointer,&regMfileAux);
							}
						}
					}
				}
			
				// ja alocamos os blocos novos
				
				regMfileAux = regMfile;
				
				for(int j=VBNatual; j<=VBNeof; j++){
					if (readBlock(regMfile, j, nossoBuffer + (j - VBNatual) * bootBlock.blockSize * SECTOR_SIZE) == ERRO) {
						nextTupla(&regMfile);
						if (isTuplaJmp(regMfile) == OK) {
							loadMFT(&regMfile, getVBN(regMfile), bootBlock.blockSize);
						}
					}
				}
				
				strncpy(nossoBuffer + corteInicio, buffer, size);
				
				for(int j=VBNatual; j<=VBNfinal; j++){
					if (writeBlock(regMfileAux, j, nossoBuffer + (j - VBNatual) * bootBlock.blockSize * SECTOR_SIZE) == ERRO) {
						nextTupla(&regMfileAux);
						if (isTuplaJmp(regMfileAux) == OK) {
							loadMFT(&regMfileAux, getVBN(regMfileAux), bootBlock.blockSize);
						}
					}
				}
				
				arquivosAbertos[i].currentPointer += size;
				if(arquivosAbertos[i].currentPointer > getBytesFileSize(*regR)){
					setBytesFileSize(regR, arquivosAbertos[i].currentPointer);
					write_sector(bootBlock.blockSize * regR->blkPointer + regR->sectPointer, regR->data);
				}
				
				return size;
				
			}
			
			
		}
	}
	return ERRO;

}

int truncate2 (FILE2 handle) {
	REGMFT regM, regMfile, regMemes;
	REGRECORD *regR, *regAvo;
	int VBNatual, achou=ERRO, VBNeof, contBlk;

	initLib();
	
	for (int i=0; i<20; i++) {
		if (arquivosAbertos[i].handle == handle) {

			fileExists(arquivosAbertos[i].path, &regR, &regM, &regAvo);
			loadMFT(&regMfile, getMFTNumber(*regR), bootBlock.blockSize);

			if (arquivosAbertos[i].currentPointer >= getBytesFileSize(*regR)) {
				return ERRO;
			}
			
			
			VBNatual=arquivosAbertos[i].currentPointer / (bootBlock.blockSize * SECTOR_SIZE);
			if(arquivosAbertos[i].currentPointer % (bootBlock.blockSize * SECTOR_SIZE)==0){
				VBNatual--;
			}
			while (achou == ERRO) {
				if (isTuplaJmp(regMfile) == OK) {
					loadMFT(&regMfile, getVBN(regMfile), bootBlock.blockSize);
				} else {
					if(VBNatual >= getVBN(regMfile) && VBNatual < (getVBN(regMfile) + getContinuosBlocks(regMfile))){
						achou=OK;
					}else{
						nextTupla(&regMfile);
					}
				}
			}
			VBNeof = (getBytesFileSize(*regR) - 1) / (bootBlock.blockSize * SECTOR_SIZE);

			regMemes=regMfile;
			contBlk=getContinuosBlocks(regMfile);
			for(int j=VBNatual+1; j<=VBNeof; j++){
				if(j >= getVBN(regMfile) && j < getVBN(regMfile) + contBlk){
					setBitmap2(j - getVBN(regMfile) + getLBN(regMfile),0);
					setRegCont(regMfile.numMFT, getContinuosBlocks(regMfile) - 1, regMfile.pointer, &regMfile);
					if(getContinuosBlocks(regMfile)==0){
						setRegType(regMfile.numMFT, -1, regMfile.pointer, &regMfile);
					}

				}else{
					nextTupla(&regMfile);
					if (isTuplaJmp(regMfile) == OK) {
						loadMFT(&regMfile, getVBN(regMfile), bootBlock.blockSize);
					}
					contBlk=getContinuosBlocks(regMfile);
				}
			}
			nextTupla(&regMemes);
			setRegType(regMemes.numMFT, 0, regMemes.pointer, &regMemes);

			setBytesFileSize(regR, arquivosAbertos[i].currentPointer);
			write_sector(regR->blkPointer * bootBlock.blockSize + regR->sectPointer, regR->data);
			return OK;
			}
		}
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
	int freeRegNum, freeBlkNum;
	REGRECORD *regR, *regAvo, regRneto;
	REGMFT regM, regM2;
	char *token, *aux;
	char pathname2[200];

	strcpy(pathname2,pathname);//se nao da segmentation falut pq strtok n gosta de parametros

	initLib();

    if (isValidPath(pathname2) == OK && fileExists(pathname2, &regR, &regM, &regAvo) == MISSING_FILE) {
		if ((freeRegNum = findFreeMFT()) != ERRO) {
			loadMFT(&regM2, freeRegNum, bootBlock.blockSize);
            
			setRegType(freeRegNum,1,0, &regM2);
			setRegType(freeRegNum,0,1, &regM2);
			setVBN(freeRegNum, 0, 0, &regM2);
			if((freeBlkNum = searchBitmap2(0)) == ERRO){
				return ERRO;
			}
			setBitmap2(freeBlkNum, 1);
			setLBN(freeRegNum, freeBlkNum, 0, &regM2);
			setRegCont(freeRegNum, 1, 0, &regM2);



			token = strtok(pathname2,"/");
			do {
				aux = token;
				token = strtok(NULL,"/");
			} while(token!=NULL);
					
			writeNewRecord(aux, freeRegNum, regR, &regM, regAvo, IS_DIR);

			loadFirstRecord(&regRneto, regM2, bootBlock.blockSize);
			for(int i=0; i<bootBlock.blockSize; i++){
				for(int j=0; j<4; j++){
					setRecordType(&regRneto, 0);
					nextRecord(&regRneto, &regM2);
				}
				write_sector(bootBlock.blockSize * regRneto.blkPointer + regRneto.sectPointer, regRneto.data);
			}
			return OK;
        } else { 
			return ERRO;
		}
    } else {
		return ERRO;
	}
}

int rmdir2 (char *pathname) {

	initLib();

    return ERRO;
}

DIR2 opendir2 (char *pathname) {
	REGRECORD *regR, *regAvo;
	REGMFT regM;
	char pathname2[200];

	strcpy(pathname2,pathname);//se nao da segmentation falut pq strtok n gosta de parametros

	initLib();

    if (isValidPath(pathname2) == OK && fileExists(pathname2, &regR, &regM, &regAvo) == IS_DIR) {
		if (isOpenDir(pathname2) == ERRO) {
			return insertDir(regM.numMFT, pathname2);
        } else { 
			return ERRO;
		}
    } else {
		return ERRO;
	}
}

int readdir2 (DIR2 handle, DIRENT2 *dentry) {

	initLib();

    return ERRO;
}

int closedir2 (DIR2 handle) {
	initLib();

	return removeDir(handle);
}

