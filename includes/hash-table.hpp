#ifndef HASH_TABLE_H
#define HASH_TABLE_H

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <immintrin.h>
#include <time.h> 

#include "hash_functions.hpp"
#include "text_buffers.hpp"

#include "logging/logging.hpp"

typedef struct node
{
    const char*  elem;
    struct node* next;
    __m256i* avx_elem;

} node_t;

typedef struct hashtable_list
{
    node_t* head;
    size_t  size;
} list_t;

typedef struct hashtable
{
    list_t* table;
    size_t  size;
    size_t (*HashFunc)(const char * word);
} htab_t;


//start prototypes
void HTableCtor(htab_t* hashtable, size_t size, size_t (*HashFunc)(const char * word));
void HTableDtor(htab_t* hashtable);

int InsertWord(htab_t* hashtable, const char* word, __m256i* avx_word);
int SearchWord(htab_t* hashtable, const char* word, __m256i* avx_word);
int SearchInList(list_t* list, const char* word, __m256i* avx_word);
node_t* NewNode(const char* word, __m256i* avx_word);
void HTableDump(htab_t* hashtable);

int ReadFile(const char* filename, textbuf_t* textbuf);
size_t GetFileSize(FILE* file);
void FillHTable(htab_t* hashtable, wordsbuf_t* words_buf, avx_wordsbuf_t* avx_wordsbuf);

FILE* OpenWriteFile(const char* filename);

void HTableResize(htab_t* hashtable, size_t new_size);

void HTableSaveStats(htab_t* hashtable, const char* filename);
void TestHashFunc(wordsbuf_t* words_buf, avx_wordsbuf_t* avx_wordsbuf, size_t (*HashFunc)(const char * word), const char* hashfunc_name);
void TestSearching(wordsbuf_t* words_buf, avx_wordsbuf_t* avx_wordsbuf, size_t (*HashFunc)(const char * word));


int avx_strcmp(__m256i* word1, __m256i* word2);
void FillAVXWordsBuf(avx_wordsbuf_t* avx_wordsbuf, wordsbuf_t* words_buf);
//end prototypes

#endif //guard HASH_TABLE_H