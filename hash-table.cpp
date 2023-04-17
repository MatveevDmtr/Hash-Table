#include "includes/hash-table.hpp"

#define HASHFUNC 0

const char* READ_FILE_NAME = "input.txt";
const char* CSV_FILE_NAME = "result/table_stats.csv";
const int DEFAULT_WORDS_NUM = 100;
const int ELEMS_IN_LIST = 15;

int main()
{
    htab_t hashtable = {};
    textbuf_t textbuf = {};

    ReadFile(READ_FILE_NAME, &textbuf);    
    
    HTableCtor(&hashtable, 10);
    
    FillHTable(&hashtable, &textbuf);

    HTableDump(&hashtable);

    HTableSaveStats(&hashtable, CSV_FILE_NAME);
    
    HTableDtor(&hashtable);
}

int InsertWord(htab_t* hashtable, const char* word)
{
    int index = Hash_3(word) % hashtable->size;
    log("table size: %d\n", hashtable->size);

    log("index in InsertWord(): %d\n", index);

    if (index >= hashtable->size)
    {
        print_log(FRAMED, "HashError: hash value is too large");
    }

    node_t* node = hashtable->table[index].head;
    for (int i = 0; i < hashtable->table[index].size; i++)
    {
        if (!strcmp(word, node->elem))
        {
            log("Word %s is already here\n", word);
            return 0;
        }
        node = node->next;
    }

    node_t* next = hashtable->table[index].head;
    hashtable->table[index].head = NewNode(word);
    hashtable->table[index].head->next = next;
    hashtable->table[index].size++;

    return 0;
}

int SearchWord(htab_t* hashtable, const char* word)
{
    int index = Hash_3(word) % hashtable->size;

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

    log("\nHashTable Dump:\n");

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

void HTableSaveStats(htab_t* hashtable, const char* filename)
{
    FILE* csv_file = OpenWriteFile(filename);

    Assert(hashtable == nullptr);

    log("\nHashTable Write\n");

    fprintf(csv_file, "Hash, Number of words\n");

    for (int i = 0; i < hashtable->size; i++)
    {
        log("write line\n");
        fprintf(csv_file, "%d, %d\n", i, hashtable->table[i].size);
    }

    fclose(csv_file);
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

FILE* OpenWriteFile(const char* filename)
{
    FILE* w_file = fopen(filename, "wb");

    if(w_file == NULL)
    {
        print_log(FRAMED, "FileError: No such file or directory");
        log("File %s not found\n", filename);
        return nullptr;
    }

    return w_file;
}

int TextToBuffer(FILE* file, textbuf_t* textbuf)
{
    size_t num_read_sym = fread(textbuf->buf, sizeof(char), textbuf->size, file);
    Assert(num_read_sym == 0);

    return 1;
}

void FillHTable(htab_t* hashtable, textbuf_t* textbuf)
{
    int textbuf_i = 0;
    char* word = nullptr;

    const char** temp_ptr_wbuf = (const char**) calloc (DEFAULT_WORDS_NUM, sizeof(char*));
    Assert(temp_ptr_wbuf == nullptr);
    wordsbuf_t words_buf = {.buf = temp_ptr_wbuf,
                            .size = DEFAULT_WORDS_NUM,
                            .index = 0};

    while (textbuf_i < textbuf->size)
    {
        word = (char*) textbuf->buf + textbuf_i;
        while (isalpha(textbuf->buf[textbuf_i]))
        {
            textbuf_i++;
        }

        textbuf->buf[textbuf_i] = '\0';
        textbuf_i++;

        Assert(word == nullptr);
        if (word && *word != '\0')  WordToBuf(&words_buf, word);        
    }

    list_t* temp_ptr_table = (list_t*) realloc (hashtable->table, ((words_buf.index / ELEMS_IN_LIST) + 1) * sizeof(list_t));
    Assert(temp_ptr_table == nullptr);
    hashtable->table = temp_ptr_table;
    hashtable->size = (words_buf.index / ELEMS_IN_LIST) + 1;

    for (int i = 0; i < words_buf.index; i++)
    {
        Assert(words_buf.buf[i] == nullptr);
        InsertWord(hashtable, words_buf.buf[i]);
    }
    free(words_buf.buf);
}

void WordToBuf(wordsbuf_t* words_buf, const char* word)
{
    if (words_buf->index >= words_buf->size)
    {
        const char** temp_ptr = (const char**) realloc (words_buf->buf, 2 * words_buf->size * sizeof(char*));
        Assert(temp_ptr == nullptr);
        words_buf->buf = temp_ptr;
    }

    words_buf->buf[words_buf->index] = word;
    words_buf->index++;
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