#include "main.h"
#include "led.h"

void main_task(void *param)
{
    main_debug_print("main start!");

    {
        //初始化LED
        led_init();
    }

    while(true)
    {
        main_task_sleep(1000);
        main_debug_print("main loop!");
    }
}

/*
  重载相关函数
*/
void *operator new(size_t size)
{
    return main_malloc(size);
}

void *operator new[](size_t size)
{
    return main_malloc(size);
}

void operator delete(void *ptr)
{
    main_free(ptr);
}

void operator delete[](void *ptr)
{
    return main_free(ptr);
}

void __cxa_pure_virtual(void)
{
    main_debug_print("Illegal to call a pure virtual function.\n");
}
