#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <immintrin.h>
#include <time.h> 

#include "logging/logging.hpp"

typedef struct node
{
    const char*  elem;
    struct node* next;
    __m256i* avx_elem;

} node_t;

typedef struct WordsBuf
{
    const char** buf;
    size_t capacity;
    size_t size;
} wordsbuf_t;

typedef struct AVXWordsBuf
{
    __m256i* buf;
    size_t   capacity;
    size_t   size;
} avx_wordsbuf_t;

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

typedef struct TextBuf
{
    char* buf;
    size_t size;
} textbuf_t;


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
int TextToBuffer(FILE* file, textbuf_t* textbuf);
FILE* OpenReadFile(const char* filename);
FILE* OpenWriteFile(const char* filename);

void WordsBufCtor(wordsbuf_t* words_buf);
void FillWordsBuf(textbuf_t* textbuf, wordsbuf_t* words_buf);
void WordToBuf(wordsbuf_t* words_buf, const char* word);
void HTableResize(htab_t* hashtable, size_t new_size);

void HTableSaveStats(htab_t* hashtable, const char* filename);
void TestHashFunc(wordsbuf_t* words_buf, size_t (*HashFunc)(const char * word), const char* hashfunc_name);
void TestSearching(wordsbuf_t* words_buf, avx_wordsbuf_t* avx_wordsbuf, size_t (*HashFunc)(const char * word));


size_t Hash_Always1(const char* word);
size_t Hash_FirstASCII(const char* word);
size_t Hash_Strlen(const char* word);
size_t Hash_SumASCII(const char* word);
size_t Hash_ROL(const char* word);
size_t Hash_ROR(const char* word);
size_t Hash_Rs(const char * word);

size_t Rol(size_t value, int shift);

void FreeTextBuf(textbuf_t* textbuf);
void FreeWordsBuf(wordsbuf_t* words_buf);


int avx_strcmp(__m256i* word1, __m256i* word2);
void FillAVXWordsBuf(avx_wordsbuf_t* avx_wordsbuf, wordsbuf_t* words_buf);
//end prototypes