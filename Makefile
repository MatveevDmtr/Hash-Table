# Переменная CC указывает компилятор, используемый для сборки
CC=g++
#В переменной CFLAGS лежат флаги, которые передаются компилятору
CFLAGS=-c -DLOGGING

all: hash-table execute clean	

hash-table: hash-table.o logging.o
	$(CC) hash-table.o logging.o -o hash-table

hash-table.o: hash-table.cpp
	$(CC) $(CFLAGS) hash-table.cpp 

logging.o: includes/logging/logging.cpp
	$(CC) $(CFLAGS) includes/logging/logging.cpp

clean:
	rm *.o

execute:
	./hash-table