# Переменная CC указывает компилятор, используемый для сборки
CC=g++
#В переменной CFLAGS лежат флаги, которые передаются компилятору
CFLAGS=-c -g -no-pie -msse2 -fno-omit-frame-pointer
OPTFLAG=-O0

all: hash-table execute clear clear_cache	

hash-table: hash-table.o logging.o
	$(CC) hash-table.o logging.o -o hash-table

hash-table.o: hash-table.cpp
	$(CC) $(CFLAGS) $(OPTFLAG) hash-table.cpp 

logging.o: includes/logging/logging.cpp
	$(CC) $(CFLAGS) $(OPTFLAG) includes/logging/logging.cpp

clear:
	rm *.o

execute:
#	./hash-table
# valgrind --leak-check=full ./hash-table
	valgrind --tool=callgrind ./hash-table
	kcachegrind

clear_cache:
	rm callgrind.*