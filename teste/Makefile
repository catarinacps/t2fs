#
# Makefile para programas exemplo
# 

CC=gcc
LIB_DIR=../lib
CFLAGS = -Wall -m32 -std=c99

TARGET = teste_create teste_open teste_delete teste_read teste_write teste_truncate teste_mkdir

all: $(TARGET)

teste_create: teste_create.c $(LIB_DIR)/libt2fs.a
	$(CC) -o teste_create teste_create.c -L$(LIB_DIR) -lt2fs $(CFLAGS)

teste_open: teste_open.c $(LIB_DIR)/libt2fs.a
	$(CC) -o teste_open teste_open.c -L$(LIB_DIR) -lt2fs $(CFLAGS)

teste_delete: teste_delete.c $(LIB_DIR)/libt2fs.a
	$(CC) -o teste_delete teste_delete.c -L$(LIB_DIR) -lt2fs $(CFLAGS)

teste_read: teste_read.c $(LIB_DIR)/libt2fs.a
	$(CC) -o teste_read teste_read.c -L$(LIB_DIR) -lt2fs $(CFLAGS)

teste_write: teste_write.c $(LIB_DIR)/libt2fs.a
	$(CC) -o teste_write teste_write.c -L$(LIB_DIR) -lt2fs $(CFLAGS)

teste_truncate: teste_truncate.c $(LIB_DIR)/libt2fs.a
	$(CC) -o teste_truncate teste_truncate.c -L$(LIB_DIR) -lt2fs $(CFLAGS)

teste_mkdir: teste_mkdir.c $(LIB_DIR)/libt2fs.a
	$(CC) -o teste_mkdir teste_mkdir.c -L$(LIB_DIR) -lt2fs $(CFLAGS)
	
clean:
	rm -rf $(TARGET) *.o *~




