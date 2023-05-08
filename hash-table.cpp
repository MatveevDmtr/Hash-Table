#include "includes/hash-table.hpp"

//#define HASHFUNC Hash_6

//#define OPT_CMP
//#define OPT_RS
//#define OPT_HASHROL
//#define OPT_ROL
#define OPT_CRC32

extern "C" size_t Mul378551(size_t num);
extern "C" size_t asm_HashROL(const char* word);
extern "C" size_t asm_crc32(__m256i* avx_word);

const char* READ_FILE_NAME = "texts/Fahrenheit_451.txt";
const char* CSV_FILE_NAME = "result/table_stats.csv";
const char* RES_DIR = "result/statistics/";
const char* RES_EXT = ".csv";
const int DEFAULT_WORDS_NUM = 100;
const int ELEMS_IN_LIST = 99;
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

    // TestHashFunc(&words_buf, Hash_Always1, "hash_always1");
    // TestHashFunc(&words_buf, Hash_FirstASCII, "hash_firstASCII");
    // TestHashFunc(&words_buf, Hash_Strlen, "hash_strlen");
    // TestHashFunc(&words_buf, Hash_SumASCII, "hash_sumASCII");
    // TestHashFunc(&words_buf, Hash_ROL, "hash_rol");
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
    Assert(hashtable == nullptr);
    Assert(avx_word == nullptr);

    size_t index = hashtable->HashFunc(word) % hashtable->size;

    node_t* node = hashtable->table[index].head;

    for (size_t i = 0; i < hashtable->table[index].size; i++)
    {
        if (!strcmp(word, node->elem))
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
#ifdef OPT_CRC32
    size_t index = asm_crc32(avx_word) % hashtable->size;
#else
    size_t index = hashtable->HashFunc(word) % hashtable->size;
#endif

    int res = SearchInList(&(hashtable->table[index]), word, avx_word);
    return res;
}

int SearchInList(list_t* list, const char* word, __m256i* avx_word)
{
    node_t* node = list->head;

    for (size_t list_i = 0; list_i < list->size; list_i++)
    {
#ifdef OPT_CMP
        if (!avx_strcmp(node->avx_elem, avx_word))
        {
            //log("Word \"%s\" found!\n", word);
            return list_i;
        }
#else
        if (!strcmp(node->elem, word))
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

size_t Hash_Always1(const char* word)
{
    return 1;
}

size_t Hash_FirstASCII(const char* word)
{
    return word[0];
}

size_t Hash_Strlen(const char* word)
{
    return strlen(word);
}

size_t Hash_SumASCII(const char* word)
{
    size_t sum = 0;
    int i = 0;
    while (word[i] != '\0')
    {
        sum += word[i];
        i++;
    }
    return sum;
}

size_t Hash_ROL(const char* word)
{
#ifdef OPT_ROL
    size_t rol_value = 0;
#endif

    size_t value = 0;
    int i = 0;
    while (word[i] != '\0')
    {
#ifdef OPT_ROL
    asm (
        ".intel_syntax noprefix\n\t"
        "mov rax, %1\n\t"
        "rol rax, 1\n\t"
        "mov %0, rax\n\t"
        ".att_syntax prefix\n\t" 
        : "=r" (rol_value)
        : "r" (value)
        : "%rax"
    );
    value = rol_value ^ word[i];
    log("counted asm hash rol value\n");
#else
        value = (Rol(value, 1)) ^ word[i];
#endif
        i++;
    }
    return value;
}

size_t Rol(size_t value, int shift)
{
    return (value << shift) | (value >> (sizeof (int) - shift));
}

size_t Hash_ROR(const char* word)
{
    size_t value = 0;
    int i = 0;
    while (word[i] != '\0')
    {
        value = ((value >> 1) | (value << (sizeof (int) - 1))) ^ word[i];
        i++;
    }
    return value;
}

size_t Hash_Rs(const char* word)
{

    static const unsigned int b = 378551;
    unsigned int a = 63689;
    size_t value = 0;

    int i = 0;
    while (word[i] != '\0')
    {
        value = value * a + (unsigned char) word[i];

#ifdef OPT_RS
        //log("%d", a * b);
        a = Mul378551(a);
        //log(" %d\n", a);
#else
        a *= b;
#endif
        i++;
    }

    return value;
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

void FillHTable(htab_t* hashtable, wordsbuf_t* words_buf, avx_wordsbuf_t* avx_wordsbuf)
{
    log("realloc size table: %d\n", (words_buf->size / ELEMS_IN_LIST) + 1);

    HTableResize(hashtable, (words_buf->size / ELEMS_IN_LIST) + 1);

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

void FillWordsBuf(textbuf_t* textbuf, wordsbuf_t* words_buf)
{
    size_t textbuf_i = 0;
    char* word = nullptr;

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
        if (word && *word != '\0')  WordToBuf(words_buf, word);        
    }

    if (words_buf->size < words_buf->capacity - 1)                                                              //decrease buf size to free unused memory
    {
        const char** temp_ptr = (const char**) realloc (words_buf->buf, words_buf->size * sizeof(char*));
        Assert(temp_ptr == nullptr);
        words_buf->buf = temp_ptr;
        words_buf->capacity = words_buf->size;
    }
}

void WordsBufCtor(wordsbuf_t* words_buf)
{
    const char** temp_ptr_wbuf = (const char**) calloc (DEFAULT_WORDS_NUM, sizeof(char*));
    Assert(temp_ptr_wbuf == nullptr);

    words_buf->buf   = temp_ptr_wbuf;
    words_buf->capacity  = DEFAULT_WORDS_NUM,
    words_buf->size = 0;
}

void WordToBuf(wordsbuf_t* words_buf, const char* word)
{
    if (words_buf->size >= words_buf->capacity)
    {
        log("realloc size word buffer: %d\n", 2 * words_buf->capacity);
        const char** temp_ptr = (const char**) realloc (words_buf->buf, 2 * words_buf->capacity * sizeof(char*));
        Assert(temp_ptr == nullptr);
        words_buf->buf = temp_ptr;
        words_buf->capacity *= 2;
    }

    words_buf->buf[words_buf->size] = word;
    words_buf->size++;
}

size_t GetFileSize(FILE* file)
{
    Assert(file == NULL);
    
    fseek(file, 0, SEEK_END);

    size_t size = ftell(file);

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

void FreeTextBuf(textbuf_t* textbuf)
{
    Assert(textbuf      == nullptr);
    Assert(textbuf->buf == nullptr);

    free(textbuf->buf);
}

void FreeWordsBuf(wordsbuf_t* words_buf)
{
    Assert(words_buf      == nullptr);
    Assert(words_buf->buf == nullptr);

    free(words_buf->buf);
}

void FillAVXWordsBuf(avx_wordsbuf_t* avx_wordsbuf, wordsbuf_t* words_buf)
{
    __m256i* temp_ptr = (__m256i*) aligned_alloc(32, words_buf->size * sizeof(__m256i));
    Assert(temp_ptr == nullptr);


    avx_wordsbuf->buf = temp_ptr;
    __m256i* avx_wb_pointer = avx_wordsbuf->buf;                                                 // points the first free elem of buffer

    for (size_t i = 0; i < words_buf->size; i++)
    {
        strcpy((char*) avx_wb_pointer, words_buf->buf[i]);        
        avx_wb_pointer++;
    }

    log("finish filling avx buf\n");

    avx_wordsbuf->buf      = temp_ptr;
    avx_wordsbuf->size     = words_buf->size;
    avx_wordsbuf->capacity = words_buf->capacity;

}

#ifdef OPT_CMP

int avx_strcmp(__m256i* word1, __m256i* word2)
{
    __m256i cmp_mask = _mm256_cmpeq_epi8(*word1, *word2);

    if (_mm256_movemask_epi8(cmp_mask) == -1)    
    {
        return 0;
    }

    return -1;
}
#endif // OPT_CMP