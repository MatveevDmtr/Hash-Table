#ifndef HASH_FUNCTIONS_H
#define HASH_FUNCTIONS_H

#include <stdio.h>
#include <string.h>

#include "logging/logging.hpp"

size_t Hash_Always1(const char* word);
size_t Hash_FirstASCII(const char* word);
size_t Hash_Strlen(const char* word);
size_t Hash_SumASCII(const char* word);
size_t Hash_ROL(const char* word);
size_t Hash_ROR(const char* word);
size_t Hash_Rs(const char * word);

size_t Rol(size_t value, int shift);
size_t Ror(size_t value, int shift);

#endif //guard HASH_FUNCTIONS_H