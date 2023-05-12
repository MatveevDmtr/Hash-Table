#include "includes/text_buffers.hpp"
#include "includes/logging/logging.hpp"

const int DEFAULT_WORDS_NUM = 100;

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