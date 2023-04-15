#include "includes/hash-table.hpp"

#define HASHFUNC 0

const char* READ_FILE_NAME = "input.txt";

int main()
{
    htab_t hashtable = {};
    textbuf_t textbuf = {};

    ReadFile(READ_FILE_NAME, &textbuf);    
    
    HTableCtor(&hashtable, 10);
    
    FillHTable(&hashtable, &textbuf);

    //InsertWord(&hashtable, "Dima");
    //InsertWord(&hashtable, "Asss");

    //SearchWord(&hashtable, "Dimas");
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
    int value = 0;
    int i = 0;
    while (word[i] != '\0')
    {
        value = ((value << 1) | (value >> (sizeof (int) - 1))) ^ word[i];
        i++;
    }
    return value;
}

int Hash_6(const char* word)
{
    int value = 0;
    int i = 0;
    while (word[i] != '\0')
    {
        value = ((value >> 1) | (value << (sizeof (int) - 1))) ^ word[i];
        i++;
    }
    return value;
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

FILE* OpenReadFile(const char* filename)
{
    FILE* r_file = fopen(filename, "rb");

    if(r_file == NULL)
    {
        print_log(FRAMED, "FileError: No such file or directory");
        log("File %s not found\n", filename);
        return nullptr;
    }

    return r_file;
}

int TextToBuffer(FILE* file, textbuf_t* textbuf)
{
    size_t num_read_sym = fread(textbuf->buf, sizeof(char), textbuf->size, file);
    Assert(num_read_sym == 0);

    return 1;
}

void FillHTable(htab_t* hashtable, textbuf_t* textbuf)
{
    int buf_index = 0;
    char* word = nullptr;
    while (buf_index < textbuf->size)
    {
        word = (char*) textbuf->buf + buf_index;
        while (isalpha(textbuf->buf[buf_index]))
        {
            buf_index++;
        }

        textbuf->buf[buf_index] = '\0';
        buf_index++;

        InsertWord(hashtable, word);        
    }
}

int GetFileSize(FILE* file)
{
    Assert(file == NULL);
    
    fseek(file, 0, SEEK_END);

    int size = ftell(file);

    fseek(file, 0, SEEK_SET);

    log("File size = %d\n", size);

    return size;
}

int ReadFile(const char* filename, textbuf_t* textbuf)
{
    FILE* text_file = OpenReadFile(filename);

    textbuf->size = GetFileSize(text_file);

    char* temp_ptr = (char*) calloc (textbuf->size, sizeof(char));
    Assert(temp_ptr == nullptr);

    textbuf->buf = temp_ptr;

    TextToBuffer(text_file, textbuf);

    fclose(text_file);

    return 0;
}