#
# Makefile para programas exemplo
# 

CC=gcc
LIB_DIR=../lib

TARGET = teste_create teste_open teste_delete teste_read teste_write teste_truncate

all: $(TARGET)

teste_create: teste_create.c $(LIB_DIR)/libt2fs.a
	$(CC) -o teste_create teste_create.c -L$(LIB_DIR) -lt2fs -Wall

teste_open: teste_open.c $(LIB_DIR)/libt2fs.a
	$(CC) -o teste_open teste_open.c -L$(LIB_DIR) -lt2fs -Wall

teste_delete: teste_delete.c $(LIB_DIR)/libt2fs.a
	$(CC) -o teste_delete teste_delete.c -L$(LIB_DIR) -lt2fs -Wall

teste_read: teste_read.c $(LIB_DIR)/libt2fs.a
	$(CC) -o teste_read teste_read.c -L$(LIB_DIR) -lt2fs -Wall

teste_write: teste_write.c $(LIB_DIR)/libt2fs.a
	$(CC) -o teste_write teste_write.c -L$(LIB_DIR) -lt2fs -Wall

teste_truncate: teste_truncate.c $(LIB_DIR)/libt2fs.a
	$(CC) -o teste_truncate teste_truncate.c -L$(LIB_DIR) -lt2fs -Wall

	
clean:
	rm -rf $(TARGET) *.o *~




