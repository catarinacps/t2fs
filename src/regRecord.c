#include "../include/regRecord.h"

// carrega a primeira entrada de diretorio e seta o ponteiro para zero
// ass:Nicolas
void loadFirstRecord(REGRECORD *regR, REGMFT regM, int blockSize) {
    blocoSize = blockSize;
    read_sector(blocoSize * getLBN(regM), regR->data);
    regR->pointer = 0;     // qual dos 4 diretorios dentro do setor
    regR->sectPointer = 0; // qual dos 4 setores dentro do bloco
    regR->blkPointer = getLBN(regM);
    regR->regM = regM;
}

// passa para a proxima entrada de diretorio se puder
// a funcao e uma merda
// ass:Nicolas
int nextRecord(REGRECORD *regR, REGMFT *regM) {

    if (regR->pointer < 3) { // pode pegar 4 diretorios dentro de 1 setor
        regR->pointer++;
        return OK;
    } else {
        if (regR->sectPointer < blocoSize - 1) { // pode pegar 4 setores dentro de 1 bloco (geralmente, depende do blocksize)
            regR->pointer = 0;
            regR->sectPointer++;
            read_sector(blocoSize * regR->blkPointer + regR->sectPointer, regR->data);
            return OK;
        } else {
            if (regR->blkPointer - getLBN(*regM) <
                getContinuosBlocks(*regM) -
                    1) { // temos que pegar o proximo bloco, neste caso ele é contiguo na memoria (está na mesma tupla)
                regR->blkPointer++;
                regR->pointer = 0;
                regR->sectPointer = 0;
                read_sector(blocoSize * regR->blkPointer + regR->sectPointer, regR->data);
                return OK;
            } else { // temos que pegar o proximo bloco mas ele NAO está contiguo, entao pegar a proxima tupla
                if (nextTupla(regM) == OK) {
                    if (isTuplaChain(*regM) == OK) {
                        regR->pointer = 0;
                        regR->sectPointer = 0;
                        regR->blkPointer = getLBN(*regM);
                        read_sector(blocoSize * regR->blkPointer + regR->sectPointer, regR->data);
                        return OK;
                    } else if (isTuplaJmp(*regM) == OK) {        // puta merda q troco feio
                        loadMFT(regM, getVBN(*regM), blocoSize); // pega o registro adicional

                        if (isTuplaChain(*regM) == OK) {
                            regR->pointer = 0;
                            regR->sectPointer = 0;
                            regR->blkPointer = getLBN(*regM);
                            read_sector(blocoSize * regR->blkPointer + regR->sectPointer, regR->data);
                            return OK;
                        } else {
                            return ERRO; // sera q cai aqui? acho q nao
                        }
                    } else {
                        backTupla(regM);
                        return ERRO_EOF; // aqui acaba a tupla
                    }
                } else {
                    return ERRO; // nunca vai chegar aqui ;-)
                }
            }
        }
    }
    return OK;
}

// passa para a entrada de diretorio anterior se puder e retorna 0, 1 caso contrario
// ass:Nicolas
// int backRecord(REGRECORD *regR, REGMFT *regM);

//----------------------------------------------------------------------------------------------
/*Type
         0: registro invalido (livre)
         1: arquivo regular (file)
         2:	arquivo de diretorio (dir)
*/
// retorna o tipo da entrada de diretorio.
// ass:Nicolas
int getRecordType(REGRECORD regR) {
    return regR.data[SIZERECORD * regR.pointer];
}

// retorna 1 se a entrada de diretorio indica registro livre, 0 caso contrario
// ass:Nicolas
int isRecordFree(REGRECORD regR) {
    if (getRecordType(regR) != 1 && getRecordType(regR) != 2)
        return OK;
    else
        return ERRO;
}

// retorna 1 se a entrada de diretorio indica um arquivo, 0 caso contrario
// ass:Nicolas
int isRecordFile(REGRECORD regR) {
    if (getRecordType(regR) == 1)
        return OK;
    else
        return ERRO;
}

// retorna 1 se a entrada de diretorio indica um diretorio, 0 caso contrario
// ass:Nicolas
int isRecordDir(REGRECORD regR) {
    if (getRecordType(regR) == 2)
        return OK;
    else
        return ERRO;
}

//----------------------------------------------------------------------------------------------

// carrega nome de reg, retorna 0 se conseguiu, 1 caso contrario
// ass:Nicolas
int getRecordName(REGRECORD regR, char *buffer) {
    if (strcpy(buffer, (char *)regR.data + SIZERECORD * regR.pointer + 1))
        return OK;
    else
        return ERRO;
}

// retorna tamanho total em blocos de reg
// ass:Nicolas
int getBlocksFileSize(REGRECORD regR) {
    int out = 0;
    int exp = 1;
    for (int i = 0; i < 4; i++) {
        out += (int)regR.data[SIZERECORD * regR.pointer + 52 + i] * exp;
        exp *= 256;
    }
    return out;
}

// retorna tamanho total em bytes de reg
// ass:Nicolas
int getBytesFileSize(REGRECORD regR) {
    int out = 0;
    int exp = 1;
    for (int i = 0; i < 4; i++) {
        out += (int)regR.data[SIZERECORD * regR.pointer + 56 + i] * exp;
        exp *= 256;
    }
    return out;
}

// retorna numero de registro MFT de reg
// ass:Nicolas
int getMFTNumber(REGRECORD regR) {
    int out = 0;
    int exp = 1;
    for (int i = 0; i < 4; i++) {
        out += (int)regR.data[SIZERECORD * regR.pointer + 60 + i] * exp;
        exp *= 256;
    }
    return out;
}
//-----------------------------------------------------------------------------------------
// seta o tipo da entrada de diretorio.
// ass:Gabriel
int setRecordType(REGRECORD *regR, int type) {
    if (type >= 0 && type <= 2) {
        regR->data[SIZERECORD * regR->pointer] = (char)type;

        return OK;
    } else {
        return ERRO;
    }
}

// seta nome de reg, retorna OK se conseguiu, ERRO caso contrario
// ass:Gabriel
int setRecordName(REGRECORD *regR, char *buffer) {

    if (strcpy((char *)regR->data + SIZERECORD * regR->pointer + 1, buffer))
        return OK;
    else
        return ERRO;
}

// seta tamanho total em blocos de reg
// ass:Gabriel
void setBlocksFileSize(REGRECORD *regR, int size) {
    int exp = 1;
    for (int i = 0; i < 4; i++) {
        regR->data[SIZERECORD * regR->pointer + 52 + i] = (char)(size / exp); // magica logica aritmetica
        exp *= 256;
    }
}

// seta tamanho total em bytes de reg
// ass:Gabriel
void setBytesFileSize(REGRECORD *regR, int size) {
    int exp = 1;
    for (int i = 0; i < 4; i++) {
        regR->data[SIZERECORD * regR->pointer + 56 + i] = (char)(size / exp);
        exp *= 256;
    }
}

// seta numero de registro MFT de reg
// ass:Gabriel
void setMFTNumber(REGRECORD *regR, int numMFT) {
    int exp = 1;
    for (int i = 0; i < 4; i++) {
        regR->data[SIZERECORD * regR->pointer + 60 + i] = (char)(numMFT / exp);
        exp *= 256;
    }
}
