#
# Makefile para programas exemplo
# 

CC=gcc
INC_DIR=../include
BIN_DIR=../lib

TARGET = main

all: $(TARGET)

main: main.c $(INC_DIR)/apidisk.h $(BIN_DIR)/apidisk.o
	$(CC) -o main main.c  $^ -Wall
	
clean:
	rm -rf $(TARGET) *.o *~