#ifndef TEXT_BUFFERS_H
#define TEXT_BUFFERS_H

#include <stdio.h>
#include <ctype.h>
#include <immintrin.h>

#include "logging/logging.hpp"

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

typedef struct TextBuf
{
    char* buf;
    size_t size;
} textbuf_t;

void FreeTextBuf(textbuf_t* textbuf);
void FreeWordsBuf(wordsbuf_t* words_buf);

void WordsBufCtor(wordsbuf_t* words_buf);
void FillWordsBuf(textbuf_t* textbuf, wordsbuf_t* words_buf);
void WordToBuf(wordsbuf_t* words_buf, const char* word);

int TextToBuffer(FILE* file, textbuf_t* textbuf);
FILE* OpenReadFile(const char* filename);

#endif //guard TEXT_BUFFERS_H