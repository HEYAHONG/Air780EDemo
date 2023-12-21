#include "stdio_patch.h"
/*
    采用printf.h中的函数
*/
#include "printf.h"


int sprintf(char* buffer, const char* format, ...)
{
    va_list va;
    va_start(va, format);
    const int ret = vsnprintf_( buffer, (size_t)-1, format, va);
    va_end(va);
    return ret;
}



int snprintf(char* buffer, size_t count, const char* format, ...)
{
    va_list va;
    va_start(va, format);
    const int ret = vsnprintf_( buffer, count, format, va);
    va_end(va);
    return ret;
}

int vsnprintf(char* buffer, size_t count, const char* format, va_list va)
{
    return vsnprintf_( buffer, count, format, va);
}
