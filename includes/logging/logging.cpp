#include "logging.hpp"

static FILE* LOG_FILE = open_log();

//start describing functions
FILE* open_log()
{
    FILE* log_file = fopen(LOG_NAME, "w");

    if (log_file == NULL)
    {
        fprintf(stderr, "ERROR: log file not found while opening\n");
        return NULL;
    }

    fprintf(log_file, "\n\n$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\n"
                          "--------------------Start program--------------------\n");
    atexit(close_log);

    return log_file;
}

void close_log()
{
    if (LOG_FILE == NULL)
    {
        fprintf(stderr, "ERROR: log file not found while closing\n");
    }
    else
    {
        fprintf(LOG_FILE, "\n$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\n"
                            "--------------------Finish program--------------------");

        fclose(LOG_FILE);

        LOG_FILE = NULL;
    }
}

void fprintf_log(size_t mode, const char* text, ...)
{
    va_list params;

    va_start(params, text);

    const char* func = "";

    int line = 0;

    switch (mode)
    {
        case SIMPLE:
            log("%s\n", text);
            break;

        case FRAMED:
            func = va_arg(params, char*);
            line = va_arg(params, int);
            PrintFatalError(func, line, text);
            break;

        case FILE_FUNC_N_LINE:
            log("\nIn file %s: ", va_arg(params, char*));
            break;

        default:
            ;
    }

    if (mode >= FUNC_N_LINE)
    {
        log("\n\tIn function: %s:", va_arg(params, char*));
    }

    if (mode >= N_LINE)
    {
        log("\n\t\tline %d: %s\n", va_arg(params, int), text);
    }

    va_end(params);
}

void PrintFatalError(const char* func, int line, const char* text)
{
    int len_text = strlen(text);

    fprintf(LOG_FILE, "\nIn %s ", func);

    fprintf(LOG_FILE, "(line %d):\n", line);

    for (size_t i = 0; i < len_text + 4; i++)
    {
        fputc('-', LOG_FILE);
    }

    fputc('\n', LOG_FILE);

    fprintf(LOG_FILE, "| %s |\n", text);

    for (size_t i = 0; i < len_text + 4; i++)
    {
        fputc('-', LOG_FILE);
    }

    fputc('\n', LOG_FILE);

    fflush(LOG_FILE);
}

#undef log
void log(const char* format, ...)
{
#ifdef LOGGING
    va_list args;

    va_start (args, format);

    vfprintf(LOG_FILE, format, args);

    fflush(LOG_FILE);

    va_end(args);
#endif
}