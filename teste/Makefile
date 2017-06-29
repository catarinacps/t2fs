#
# Makefile para programas exemplo
# 

CC=gcc
LIB_DIR=../lib

TARGET = teste_create teste_open

all: $(TARGET)

teste_create: teste_create.c $(LIB_DIR)/libt2fs.a
	$(CC) -o teste_create teste_create.c -L$(LIB_DIR) -lt2fs -Wall

teste_open: teste_open.c $(LIB_DIR)/libt2fs.a
	$(CC) -o teste_open teste_open.c -L$(LIB_DIR) -lt2fs -Wall
	
clean:
	rm -rf $(TARGET) *.o *~




