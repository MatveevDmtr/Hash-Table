# Переменная CC указывает компилятор, используемый для сборки
CC=g++
#В переменной CFLAGS лежат флаги, которые передаются компилятору
CFLAGS=-c -g -no-pie -mavx2 -msse2 -fno-omit-frame-pointer -std=c++14
OPTFLAG=-O0

all: hash-table execute clear

hash-table: hash-table.o logging.o asm_rol asm_crc32
	$(CC) hash-table.o logging.o includes/asm_files/hash_rol.o includes/asm_files/crc32.o -o hash-table

hash-table.o: hash-table.cpp
	$(CC) $(CFLAGS) $(OPTFLAG) hash-table.cpp 

logging.o: includes/logging/logging.cpp
	$(CC) $(CFLAGS) $(OPTFLAG) includes/logging/logging.cpp

asm_rol: includes/asm_files/hash_rol.asm
	nasm -f elf64 includes/asm_files/hash_rol.asm

asm_crc32: includes/asm_files/crc32.asm
	nasm -f elf64 includes/asm_files/crc32.asm

clear:
	rm *.o

execute:
	./hash-table
#   valgrind --leak-check=full ./hash-table
#	valgrind --tool=callgrind ./hash-table
#	kcachegrind

clear_cache:
	rm callgrind.*