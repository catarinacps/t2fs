#include "../include/t2fs.h"
#include <stdio.h>

int main() {
	FILE2 handleF1, handleF2, handleF3, handleF4, handleF5;
	DIR2 handleD1, handleD2;
    char buffer1[120], buffer2[120], buffer3[120], buffer4[120], buffer5[120], buffer6[120];
	DIRENT2 dir1, dir2;

    for(int i=0; i<120; i++){
        buffer1[i]=0;
		buffer2[i]=0;
		buffer3[i]=0;
		buffer4[i]=0;
		buffer5[i]=0;
		buffer6[i]=0;
    }

    printf("\nIniciando TESTE GOD\n");
    
	if(identify2(buffer1, 120) != 0){
		printf("\ndeu erro 1");
	} else{
		printf("\n");
		puts(buffer1);
	}
	
	if((handleD1 = opendir2("/")) < 0){
		printf("\ndeu erro 2");
	} else{
		while((readdir2(handleD1, &dir1)) == 0){
			printf("\n");
			puts(dir1.name);
            printf("Tipo:%d Size:%d \n", dir1.fileType, dir1.fileSize);
		}
		if(closedir2(handleD1) != 0){
			printf("\ndeu erro 3");
		}
	}
	
	if((handleF1 = open2("/file2")) < 0){
		printf("\ndeu erro 4");
	} else{
		if((read2(handleF1, buffer2, 40)) < 0){
			printf("\ndeu erro 5");
		} else{
			printf("\n");
			puts(buffer2);
			if(close2(handleF1) != 0){
				printf("\ndeu erro 6");
			}
		}
	}

	if(mkdir2("/pastoso") != 0){
		printf("\ndeu erro 7");
	} else{
		if((handleD2 = opendir2("/pastoso")) < 0){
			printf("\ndeu erro 8");
		} else{
			if((readdir2(handleD2, &dir2)) < 0){
				printf("\nNÃO CONSEGUI LER O DIR 'pastoso', mas isso era esperado entao ta sussa vlw flw");
			} else{
				printf("\ndeu erro 9");
			}
			if((handleF2 = create2("/pastoso/testada")) < 0){
				printf("\ndeu erro 10");
			} else{
				if((read2(handleF2, buffer3, 40)) < 0){
					printf("\nNÃO CONSEGUI LER O 'testada', mas isso era esperado entao ta sussa vlw flw");
				} else{
					printf("\ndeu erro 11");
				}
				if((write2(handleF2,"era uma historia de amor de aventura e magia",50)) < 0){
					printf("\ndeu erro 12");
				} else{
					if((seek2(handleF2, 0)) < 0){
						printf("\ndeu erro 13");
					} else{
						if((read2(handleF2, buffer3, 40)) < 0){
							printf("\ndeu erro 14");
						} else{
							printf("\n");
							puts(buffer3);
							if((close2(handleF2)) < 0){
								printf("\ndeu erro 15");
							}
						}
					}
				}
			}
		}
	}
	
	if((write2(handleF2,"totosinho na balada totosinho reuniao",50)) < 0){
		printf("\nnao escreveu os totosinho mas ta tranquilaço porque era pra dar erro mesmo");
	} else{
		printf("\ndeu erro 16");
	}
	
	if((rmdir2("/pastoso"))<0){
		printf("\nnao conseguiu remover o pastoso porque tem testada dentro");
	} else{
		printf("\ndeu erro na parte de excluir o pastoso");
	}
	
	if((handleF3 = open2("/pastoso/testada")) < 0){
		printf("\ndeu erro 16b");
	} else{
		if((seek2(handleF3, -1)) < 0){
			printf("\ndeu erro 17");
		} else{
			if((write2(handleF3,"SANDY E JUNIOR S2",30)) < 0){
				printf("\ndeu erro 18");
			} else{
				if((seek2(handleF3, 4)) < 0){
					printf("\ndeu erro 19");
				} else{
					if((read2(handleF3, buffer4, 40)) < 0){
						printf("\ndeu erro 20");
					} else{
						printf("\n");
						puts(buffer4);
						if((delete2("/pastoso/testada")) < 0){
								printf("\ndeu erro 21");
							}
					}
				}
			}
		}
	}
	
	if((handleF4 = open2("/pastoso/testada")) < 0){
		printf("\nNAO ABRIU A TESTADA DO PASTOSO, MAS ELE NEM EXISTE MESMO NÉ PROGRAMADOR BURRO");
	} else{
		printf("\ndeu erro 22");
	}
	
	if((rmdir2("/pastoso"))<0){
		printf("\ndeu erro 23");
	}
	
	if((handleF5 = open2("/file1")) < 0){
		printf("\ndeu erro 24");
	} else{
		if((read2(handleF5, buffer5, 40)) < 0){
			printf("\ndeu erro 25");
		} else{
			printf("\n");
			puts(buffer5);
			if((seek2(handleF5, 1024)) < 0){
				printf("\ndeu erro porque o idiota tentou seekar o vazio né mlk");
			} else{
				printf("\ndeu erro 26");
			}
			
			if((seek2(handleF5, 6)) < 0){
				printf("\ndeu erro 27");
			} else{
				if((truncate2(handleF5)) < 0){
					printf("\ndeu erro 28");
				} else{
					seek2(handleF5,0);
					if((read2(handleF5, buffer6, 40)) < 0){
						printf("\ndeu erro 29");
					} else{
						printf("\n");
						puts(buffer6);
						if((close2(handleF5)) < 0){
							printf("\ndeu erro 30");
						}
					}
				}
			}
		}
	}
	
	
    printf("\nFim do teste.\n");

    return 0;
}