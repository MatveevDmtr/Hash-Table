#ifndef LOG_NAME

    #define LOG_NAME "log.txt"

#endif


#ifndef LOGGING_H_INCLUDED
#define LOGGING_H_INCLUDED

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>


enum LOG_MODES
{
    SIMPLE           = 0,
    FRAMED           = 1,
    N_LINE           = 2,
    FUNC_N_LINE      = 3,
    FILE_FUNC_N_LINE = 4
};

#ifdef LOGGING
#define Assert(condition)                                  \
{                                                          \
    if(condition)                                          \
    {                                                      \
        print_log(FRAMED, "Assertion ERROR");              \
        log("In line %d in \n%s \nFound (%s).\n",          \
                __LINE__, __FILE__, #condition);           \
    }                                                      \
}
#else
    #define Assert(condition)
#endif

#define LogError(errcode)                                         \
    {                                                             \
        LogCritError(errcode, __PRETTY_FUNCTION__, __LINE__);     \
    }

#define print_crit_errors(text, func, line)                       \
    {                                                             \
        fprintf_log(FRAMED, text, func, line);                    \
    }

#define print_log(mode, text)                                       \
{                                                                   \
    switch (mode)                                                   \
    {                                                               \
        case SIMPLE:                                                \
            fprintf_log(mode, text);                                \
            break;                                                  \
                                                                    \
        case FRAMED:                                                \
            fprintf_log(mode, text, __PRETTY_FUNCTION__, __LINE__); \
            break;                                                  \
                                                                    \
        case N_LINE:                                                \
            fprintf_log(mode, text, __LINE__);                      \
            break;                                                  \
                                                                    \
        case FUNC_N_LINE:                                           \
            fprintf_log(mode, text, __PRETTY_FUNCTION__,            \
                                  __LINE__);                        \
            break;                                                  \
                                                                    \
        case FILE_FUNC_N_LINE:                                      \
            fprintf_log(mode, text, __FILE__,                       \
                                  __PRETTY_FUNCTION__,              \
                                  __LINE__);                        \
            break;                                                  \
                                                                    \
        default:                                                    \
            fprintf_log(FRAMED,                                     \
                      "LOGERROR: Invalid Mode of logging\n",        \
                      __PRETTY_FUNCTION__);                         \
    }                                                               \
}

//start prototypes
FILE* open_log();

void close_log();

void fprintf_log(size_t mode, const char* text, ...);

void PrintFatalError(const char* func, int line, const char* text);

void log(const char* format, ...);
//end prototypes
//#define LOGGING
//#ifndef LOGGING
    //#define log(format, ...)
//#elseif
    //#define log(format, ...) log_()
//#endif

#endif //guard