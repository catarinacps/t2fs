#	-- The T2FS file system --
#
#	Makefile do arquivo da library estática
#
#	@param target
#		"all" - compila a library
#		"clean" - limpa os binários gerados na compilação
#		"redo" - realiza a regra clean e então compila a library
#
#	@authors	Henrique Correa Pereira da Silva
#				Nicolas Eymael da Silva
#				Grabriel Stefaniak Niemiec

#	Flags de compilaçao. Debug para uso no GDB
CC = gcc
DEBUG = -g
CFLAGS = -I$(INC_DIR) -Wall -m32 -std=c99 $(DEBUG)

#	Diretorios do projeto
INC_DIR = include
OBJ_DIR = bin
LIB_DIR = lib
SRC_DIR = src

#	Caminho do arquivo estático final da library
TARGET = $(LIB_DIR)/libt2fs.a

#	Dependencias, ou seja, arquivos de header
_DEPS = auxlib2.h bitmap2.h apidisk.h t2fs.h regMFT.h regRecord.h lista.h
DEPS = $(patsubst %,$(INC_DIR)/%,$(_DEPS))

#	Objetos a serem criados
_OBJ = auxlib2.o t2fs.o regMFT.o regRecord.o lista.o
OBJ = $(patsubst %,$(OBJ_DIR)/%,$(_OBJ))

#	Exceções para a regra "clean"
_EXP = apidisk.o bitmap2.o
EXP = $(patsubst %,$(LIB_DIR)/%,$(_EXP))


all: $(TARGET)

$(TARGET): $(OBJ) $(EXP)
	ar rcs $@ $^

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

.PHONY: clean all

clean:
	rm -f $(OBJ) $(INC_DIR)/*~ $(LIB_DIR)/*.a *~

redo: clean
	make all
