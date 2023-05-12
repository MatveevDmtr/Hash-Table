#include "includes/hash_functions.hpp"

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
        value = Ror(value, 1) ^ word[i];
        i++;
    }
    return value;
}

size_t Ror(size_t value, int shift)
{
    return (value >> shift) | (value << (sizeof (int) - shift));
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
        a *= b;
        i++;
    }

    return value;
}