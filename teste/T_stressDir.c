#include "t2fs.h"
#include <stdio.h>
#include <string.h>

int main(){
	//este teste supõe um disco reinicializado
	char dir[7]; // /dirXX
	int i;
	DWORD handle;
	for(i = 0; i < 51; i++) {
		sprintf(dir, "/dir%d", i);
		mkdir2(dir);
		handle = opendir2(dir);
		if(handle != -1 && i < 50) printf("Dir%d criado com sucesso. Seu handle é %d\n", i, handle);
		else if(handle == -1 && i == 50) printf("Muitos diretórios abertos. Dir%d não pôde ser criado.\n", i);
		else {
			printf("Erro crítico.\n");
			return 1;
		}
	}
	return 0;
}
