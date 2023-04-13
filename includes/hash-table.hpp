#include <stdio.h>
#include "logging/logging.hpp"

typedef struct node
{
    const char*        elem;
    struct node* next;
} node_t;

typedef struct hashtable_list
{
    node_t* head;
    int     size;
} list_t;

typedef struct hashtable
{
    list_t* table;
    int     size;
} htab_t;


//start prototypes
void HTableCtor(htab_t* hashtable, int size);
void HTableDtor(htab_t* hashtable);

void InsertWord(htab_t* hashtable, const char* word);
int  SearchWord(htab_t* hashtable, const char* word);
int Hash(const char* word);
node_t* NewNode(const char* word);
void HTableDump(htab_t* hashtable);

int Hash_1(const char* word);
int Hash_2(const char* word);
int Hash_3(const char* word);
int Hash_4(const char* word);
int Hash_5(const char* word);
int Hash_6(const char* word);
//end prototypes