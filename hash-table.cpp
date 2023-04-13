#include "includes/hash-table.hpp"

#define HASHFUNC 0

int main()
{
    htab_t hashtable = {};
    HTableCtor(&hashtable, 10);
    
    InsertWord(&hashtable, "Dima");
    InsertWord(&hashtable, "Asss");

    SearchWord(&hashtable, "Dimas");
    HTableDump(&hashtable);
    
    HTableDtor(&hashtable);
}

void InsertWord(htab_t* hashtable, const char* word)
{
    int index = Hash_3(word);

    if (index >= hashtable->size)
    {
        print_log(FRAMED, "HashError: hash value is too large");
    }

    node_t* next = hashtable->table[index].head;
    hashtable->table[index].head = NewNode(word);
    hashtable->table[index].head->next = next;
    hashtable->table[index].size++;
}

int SearchWord(htab_t* hashtable, const char* word)
{
    int index = Hash_3(word);

    if (index >= hashtable->size)
    {
        print_log(FRAMED, "SearchError: hash value is too large to be in table");
        return 0;
    }

    node_t* node = hashtable->table[index].head;
    bool found = 0;
    for (int list_i = 0; list_i < hashtable->table[index].size; list_i++)
    {
        if (!strcmp(node->elem, word))
        {
            printf("Word \"%s\" found!\n", word);
            found = 1;
            break;
        }
        node = node->next;
    }
    if (!found) 
    {
        printf("Word \"%s\" not found.\n", word);
        return 0;
    }
    return 1;
}

node_t* NewNode(const char* word)
{
    node_t* node = (node_t*) calloc (1, sizeof(node_t));
    Assert(node == nullptr);
    
    node->elem = word;
    
    return node;
}

int Hash_1(const char* word)
{
    return 1;
}

int Hash_2(const char* word)
{
    return word[0];
}

int Hash_3(const char* word)
{
    return strlen(word);
}

int Hash_4(const char* word)
{
    int sum = 0;
    int i = 0;
    while (word[i] != '\0')
    {
        sum += word[i];
        i++;
    }
    return sum;
}

int Hash_5(const char* word)
{
    int sum = 0;
    int i = 0;
    while (word[i] != '\0')
    {
        sum += word[i];
        i++;
    }
    return sum;
}

int Hash_6(const char* word)
{
    int sum = 0;
    int i = 0;
    while (word[i] != '\0')
    {
        sum += word[i];
        i++;
    }
    return sum;
}

void HTableCtor(htab_t* hashtable, int size)
{
    list_t* temp_ptr = (list_t*) calloc (size, sizeof(list_t));

    Assert(temp_ptr == nullptr);

    hashtable->table = temp_ptr;
    hashtable->size  = size;
}

void HTableDtor(htab_t* hashtable)
{
    Assert(hashtable == nullptr);
    Assert(hashtable->table == nullptr);

    node_t* next_node = nullptr;

    for (int i = 0; i < hashtable->size; i++)
    {
        node_t* node = hashtable->table[i].head;
        for (int list_i = 0; list_i < hashtable->table[i].size; list_i++)
        {
            next_node = node->next;
            free(node);
            node = next_node;
        }
        log("\n");
    }

    free(hashtable->table);
}

void HTableDump(htab_t* hashtable)
{
    Assert(hashtable == nullptr);

    for (int i = 0; i < hashtable->size; i++)
    {
        log("[%d] (%d elems): ", i, hashtable->table[i].size);
        node_t* node = hashtable->table[i].head;
        for (int list_i = 0; list_i < hashtable->table[i].size; list_i++)
        {
            log("%s ", node->elem);
            node = node->next;
        }
        log("\n");
    }
}