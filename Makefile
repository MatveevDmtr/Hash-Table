# Переменная CC указывает компилятор, используемый для сборки
CC=g++
#В переменной CFLAGS лежат флаги, которые передаются компилятору
CFLAGS=-c -g -no-pie -mavx2 -msse2 -fno-omit-frame-pointer -std=c++14
OPTFLAG=-O3
MY_OPT=-DOPT_CMP -DOPT_SEARCH -DOPT_CRC32

all: hash-table execute clear

hl: hash-table_headed_lists execute clear

hash-table: hash-table.o logging.o hash_functions.o text_buffers.o asm_rol asm_crc32 asm_search
	$(CC) hash-table.o logging.o hash_functions.o text_buffers.o includes/asm_files/hash_rol.o includes/asm_files/crc32.o includes/asm_files/asm_SearchInList.o -o hash-table

hash-table_headed_lists: hash-table_headed_lists.o logging.o hash_functions.o text_buffers.o asm_rol asm_crc32 asm_search_headed_lists
	$(CC) hash-table_headed_lists.o logging.o hash_functions.o text_buffers.o includes/asm_files/hash_rol.o includes/asm_files/crc32.o includes/asm_files/asm_SearchInList_headed_lists.o -o hash-table

hash-table.o: hash-table.cpp
	$(CC) $(CFLAGS) $(OPTFLAG) $(MY_OPT) hash-table.cpp
	
hash-table_headed_lists.o: hash-table_headed_lists.cpp
	$(CC) $(CFLAGS) $(OPTFLAG) $(MY_OPT) hash-table_headed_lists.cpp 

logging.o: includes/logging/logging.cpp
	$(CC) $(CFLAGS) $(OPTFLAG) $(MY_OPT) includes/logging/logging.cpp 

hash_functions.o: hash_functions.cpp
	$(CC) $(CFLAGS) $(OPTFLAG) $(MY_OPT) hash_functions.cpp

text_buffers.o: text_buffers.cpp
	$(CC) $(CFLAGS) $(OPTFLAG) $(MY_OPT) text_buffers.cpp

asm_rol: includes/asm_files/hash_rol.asm
	nasm -f elf64 includes/asm_files/hash_rol.asm

asm_crc32: includes/asm_files/crc32.asm
	nasm -f elf64 includes/asm_files/crc32.asm

asm_search: includes/asm_files/asm_SearchInList.asm
	nasm -f elf64 includes/asm_files/asm_SearchInList.asm

asm_search_headed_lists: includes/asm_files/asm_SearchInList_headed_lists.asm
	nasm -f elf64 includes/asm_files/asm_SearchInList_headed_lists.asm

clear:
	rm *.o

execute:
	./hash-table optimizations
#	valgrind --leak-check=full ./hash-table optimizations
#	valgrind --tool=callgrind ./hash-table optimizations
#	kcachegrind

clear_cache:
	rm callgrind.*