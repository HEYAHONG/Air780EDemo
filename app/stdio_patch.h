#ifndef __STDIO_PATCH_H_INCLUDED__
#define __STDIO_PATCH_H_INCLUDED__
#ifdef __cplusplus
/*
    此文件用于g++编译器头文件的报错进行修复，仅对C++生效,包含C++头文件前必须包含此文件(直接或间接)。
*/
#include "stdio.h"
#include "stdarg.h"

/*
    仿照stdio.h中，对可能缺少的函数进行声明。
*/
#undef sprintf
#undef snprintf
#undef vsnprintf
int sprintf(char* buffer, const char* format, ...);
int snprintf(char* buffer, size_t count, const char* format, ...);
int vsnprintf(char* buffer, size_t count, const char* format, va_list va);

#endif // __cplusplus
#endif // __STDIO_PATCH_H_INCLUDED__
