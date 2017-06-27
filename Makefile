#	-- The compact threads library --
#
#	Makefile do arquivo da library estática
#
#	@param target
#		Pode ser "all" ou "clean", sendo que "all"
#		compila a library e "clean" limpa todos arquivos criados
#
#	@authors	Henrique Correa Pereira da Silva
#				Nicolas Eymael da Silva
#				Grabriel Stefaniak Niemiec

#	Flags de compilaçao. Debug para uso no GDB
CC = gcc
DEBUG = -g
CFLAGS = -I$(INC_DIR) -Wall $(DEBUG) -o2

#	Diretorios do projeto
INC_DIR = include
OBJ_DIR = bin
LIB_DIR = lib
SRC_DIR = src

#	Caminho do arquivo estático final da library
TARGET = $(LIB_DIR)/libt2fs.a

#	Dependencias, ou seja, arquivos de header
_DEPS = bitmap2.h apidisk.h t2fs.h regMFT.h regRecord.h auxlib2.h
DEPS = $(patsubst %,$(INC_DIR)/%,$(_DEPS))

#	Objetos a serem criados
_OBJ = t2fs.o regMFT.o regRecord.o auxlib2.o
OBJ = $(patsubst %,$(OBJ_DIR)/%,$(_OBJ))

#	Exceções para a regra "clean"
_EXP = apidisk.o bitmap2.o
EXP = $(patsubst %,$(LIB_DIR)/%,$(_EXP))


all: $(TARGET)

$(TARGET): $(OBJ) $(EXP)
	ar rcs $@ $^

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

.PHONY: clean

clean:
	rm -f $(OBJ) $(INC_DIR)/*~ $(LIB_DIR)/*.a *~
