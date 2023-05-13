//#define OPT_CMP
//#define OPT_RS
//#define OPT_HASHROL
//#define OPT_ROL
//#define OPT_CRC32
//#define OPT_SEARCH

#include "includes/hash-table.hpp"

extern "C" size_t asm_HashROL(const char* word);
extern "C" size_t asm_crc32(__m256i* avx_word);
extern "C" size_t asm_SearchInList(list_t* list, const char* word, __m256i* avx_word);

const char* READ_FILE_NAME = "texts/Fahrenheit_451.txt";
const char* CSV_FILE_NAME = "result/table_stats.csv";
const char* RES_DIR = "result/statistics/";
const char* RES_EXT = ".csv";
const int HASHTABLE_SIZE = 3583; //3583 or 479
const int NUM_MEASURES = 2000;

int main()
{
    textbuf_t textbuf = {};

    ReadFile(READ_FILE_NAME, &textbuf);

    wordsbuf_t words_buf = {};
    WordsBufCtor(&words_buf);

    avx_wordsbuf_t avx_wordsbuf = {};

    FillWordsBuf(&textbuf, &words_buf);

    FillAVXWordsBuf(&avx_wordsbuf, &words_buf);
    log("avx words buf filled\n");

    //TestHashFunc(&words_buf, &avx_wordsbuf, Hash_Always1, "crc32");
    // TestHashFunc(&words_buf, Hash_FirstASCII, "hash_firstASCII");
    // TestHashFunc(&words_buf, Hash_Strlen, "hash_strlen");
    //TestHashFunc(&words_buf, &avx_wordsbuf, Hash_SumASCII, "sum_ascii");
    // TestHashFunc(&words_buf, &avx_wordsbuf, Hash_ROL, "hash_rol");
    // TestHashFunc(&words_buf, Hash_ROR, "hash_ror");
    // TestHashFunc(&words_buf, Hash_Rs, "hash_rs");

#ifdef OPT_HASHROL
    TestSearching(&words_buf, &avx_wordsbuf, asm_HashROL);
#else
    TestSearching(&words_buf, &avx_wordsbuf, Hash_ROL);
#endif

    FreeTextBuf(&textbuf);
    FreeWordsBuf(&words_buf);    
}

void TestSearching(wordsbuf_t* words_buf, avx_wordsbuf_t* avx_wordsbuf, size_t (*HashFunc)(const char * word))
{
    htab_t hashtable = {};

    HTableCtor(&hashtable, 10, HashFunc);
    
    FillHTable(&hashtable, words_buf, avx_wordsbuf);

    HTableDump(&hashtable);

    log("start searching words\n");

    printf("number of words: %ld\n", words_buf->size);

    clock_t time_start = clock();

    for (int num_measures = 0; num_measures < NUM_MEASURES; num_measures++) 
    {
        // printf("measures: %d\n", num_measures);
        for (int i = 0; i < words_buf->size; i++)
        {
            SearchWord(&hashtable, words_buf->buf[i], avx_wordsbuf->buf + i);
        }
    }

    clock_t time_end = clock();

    printf("Elapsed time: %f ms\n", (double)(time_end - time_start) / CLOCKS_PER_SEC * 1000 / NUM_MEASURES);

    HTableDtor(&hashtable);
}

void TestHashFunc(wordsbuf_t* words_buf, avx_wordsbuf_t* avx_wordsbuf, size_t (*HashFunc)(const char * word), const char* hashfunc_name)
{
    htab_t hashtable = {};

    HTableCtor(&hashtable, 10, HashFunc);
    
    FillHTable(&hashtable, words_buf, avx_wordsbuf);

    HTableSaveStats(&hashtable, hashfunc_name);
    
    HTableDtor(&hashtable);
}

int InsertWord(htab_t* hashtable, const char* word, __m256i* avx_word)
{
    //Assert(hashtable == nullptr);
    //Assert(avx_word == nullptr);

#ifdef OPT_CRC32
    size_t index = asm_crc32(avx_word) % hashtable->size;
#else
    size_t index = hashtable->HashFunc(word) % hashtable->size;
#endif

    node_t* node = hashtable->table[index].head;

    for (size_t i = 0; i < hashtable->table[index].size; i++)
    {
        if (strcmp(word, node->elem) == 0)
        {
            log("Word %s is already here\n", word);
            return 0;
        }
        node = node->next;
    }

    node_t* next = hashtable->table[index].head;
    hashtable->table[index].head = NewNode(word, avx_word);
    hashtable->table[index].head->next = next;
    hashtable->table[index].size++;

    return 0;
}

int SearchWord(htab_t* hashtable, const char* word, __m256i* avx_word)
{
    Assert(word == nullptr);
    Assert(avx_word == nullptr);

#ifdef OPT_CRC32
    log ("counting crc32\n");
    size_t index = asm_crc32(avx_word) % hashtable->size;
    log ("finish crc32\n");
#else
    size_t index = hashtable->HashFunc(word) % hashtable->size;
    log("index: %d\n", index);
#endif

    //log("%d\n", hashtable->table[index].head);

int res = 0;
#ifdef OPT_SEARCH
    res = asm_SearchInList(&(hashtable->table[index]), word, avx_word);
    //printf("%d", res);
#else
    res = SearchInList(&(hashtable->table[index]), word, avx_word);
#endif

    return res;
}

int SearchInList(list_t* list, const char* word, __m256i* avx_word)
{
    node_t* node = list->head;

    for (size_t list_i = 0; list_i < list->size; list_i++)
    {
#ifdef OPT_CMP
        if (avx_strcmp(node->avx_elem, avx_word) == 0)
        {
            //log("Word \"%s\" found!\n", word);
            return list_i;
        }
#else
        if (strcmp(node->elem, word) == 0)
        {
            //log("Word \"%s\" found!\n", word);
            return list_i;
        }

#endif
        
        node = node->next;
    }

    //log("Word \"%s\" not found!\n", word);    
    return -1;
}

node_t* NewNode(const char* word, __m256i* avx_word)
{
    node_t* node = (node_t*) calloc (1, sizeof(node_t));
    Assert(node == nullptr);
    
    node->elem = word;
    node->avx_elem = avx_word;
    
    return node;
}

void HTableCtor(htab_t* hashtable, size_t size, size_t (*HashFunc)(const char * word))
{
    list_t* temp_ptr = (list_t*) calloc (size, sizeof(list_t));

    Assert(temp_ptr == nullptr);

    hashtable->table = temp_ptr;
    hashtable->size  = size;
    hashtable->HashFunc = HashFunc;
}

void HTableDtor(htab_t* hashtable)
{
    Assert(hashtable == nullptr);
    Assert(hashtable->table == nullptr);

    node_t* next_node = nullptr;

    for (size_t i = 0; i < hashtable->size; i++)
    {
        node_t* node = hashtable->table[i].head;
        for (size_t list_i = 0; list_i < hashtable->table[i].size; list_i++)
        {
            next_node = node->next;
            free(node);
            node = next_node;
        }
    }

    free(hashtable->table);
}

void HTableDump(htab_t* hashtable)
{
    Assert(hashtable == nullptr);

    log("\nHashTable Dump:\n");

    log("hashtable size: %ld\n", hashtable->size);

    for (size_t i = 0; i < hashtable->size; i++)
    {
        log("[%d] (%d elems): ", i, hashtable->table[i].size);
        node_t* node = hashtable->table[i].head;
        for (size_t list_i = 0; list_i < hashtable->table[i].size; list_i++)
        {
            log("%s ", node->elem);
            node = node->next;
        }
        log("\n");
    }
}

void HTableSaveStats(htab_t* hashtable, const char* hashfunc_name)
{
    char filepath[50] = {};
    strcat(strcat(strcpy(filepath, RES_DIR), hashfunc_name), RES_EXT);

    FILE* csv_file = OpenWriteFile(filepath);

    Assert(hashtable == nullptr);

    log("\nHashTable Write\n");

    fprintf(csv_file, "Hash value, %s\n", hashfunc_name);

    for (size_t i = 0; i < hashtable->size; i++)
    {
        fprintf(csv_file, "%ld, %ld\n", i, hashtable->table[i].size);
    }

    fclose(csv_file);
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

void FillHTable(htab_t* hashtable, wordsbuf_t* words_buf, avx_wordsbuf_t* avx_wordsbuf)
{
    log("realloc size table: %d\n", HASHTABLE_SIZE);

    HTableResize(hashtable, HASHTABLE_SIZE);

    for (size_t i = 0; i < hashtable->size; i++)
    {
        hashtable->table[i].size = 0;                           // make nulls (as calloc does)
    }

    for (size_t i = 0; i < words_buf->size; i++)
    {
        Assert(words_buf->buf[i] == nullptr);
        InsertWord(hashtable, words_buf->buf[i], avx_wordsbuf->buf + i);
    }
}

void HTableResize(htab_t* hashtable, size_t new_size)
{
    list_t* temp_ptr_table = (list_t*) realloc (hashtable->table, new_size * sizeof(list_t));
    Assert(temp_ptr_table == nullptr);

    hashtable->table = temp_ptr_table;
    hashtable->size = new_size;
}


int avx_strcmp(__m256i* word1, __m256i* word2)
{
    __m256i cmp_mask = _mm256_cmpeq_epi8(*word1, *word2);

    if (_mm256_movemask_epi8(cmp_mask) == -1)    
    {
        return 0;
    }

    return -1;
}