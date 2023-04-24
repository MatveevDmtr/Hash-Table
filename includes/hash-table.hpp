#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include "logging/logging.hpp"

typedef struct node
{
    const char*        elem;
    struct node* next;
} node_t;

typedef struct WordsBuf
{
    const char** buf;
    size_t size;
    size_t index;
} wordsbuf_t;

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

int InsertWord(htab_t* hashtable, const char* word);
int SearchWord(htab_t* hashtable, const char* word);
int SearchInList(list_t* list, const char* word);
int Hash(const char* word);
node_t* NewNode(const char* word);
void HTableDump(htab_t* hashtable);

int ReadFile(const char* filename, textbuf_t* textbuf);
size_t GetFileSize(FILE* file);
void FillHTable(htab_t* hashtable, textbuf_t* textbuf);
int TextToBuffer(FILE* file, textbuf_t* textbuf);
FILE* OpenReadFile(const char* filename);
FILE* OpenWriteFile(const char* filename);

void WordToBuf(wordsbuf_t* words_buf, const char* word);
void HTableSaveStats(htab_t* hashtable, const char* filename);
void TestHashFunc(textbuf_t* textbuf, size_t (*HashFunc)(const char * word), const char* hashfunc_name);


size_t Hash_Always1(const char* word);
size_t Hash_FirstASCII(const char* word);
size_t Hash_Strlen(const char* word);
size_t Hash_SumASCII(const char* word);
size_t Hash_ROL(const char* word);
size_t Hash_ROR(const char* word);
size_t Hash_Rs(const char * word);
//end prototypes