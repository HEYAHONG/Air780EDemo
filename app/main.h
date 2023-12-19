#ifndef __MAIN_H__
#define __MAIN_H__

#include "stdint.h"
#include "stdlib.h"
#include "inttypes.h"

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus


void main_task(void *param);

void main_task_sleep(uint32_t timeout_ms);

void * main_malloc(size_t bytes);

void main_free(void *ptr);

int main_debug_print(const char * fmt,...);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // __MAIN_H__




