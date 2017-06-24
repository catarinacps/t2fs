#include <string.h>
#include "../include/diskio.h"

//ass:Nicolas
int readBytes(int numBytes, char *buffer, unsigned int sector, unsigned short int offset) {
    unsigned char auxBuffer[256];
    int quantidadeSetores = numBytes;

    quantidadeSetores /= 256;
    quantidadeSetores++;

    for(int i = quantidadeSetores; i > 0; i--) {
        read_sector(sector, auxBuffer);

        if(i == quantidadeSetores) {
            strcat(buffer, (const char *)auxBuffer + offset);
        } else {
            strcat(buffer, (const char *)auxBuffer);
        }
        sector++;
    }

    return 0;
}