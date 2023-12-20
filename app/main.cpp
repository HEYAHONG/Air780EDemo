#include "main.h"
#include "RC.h"

static void displaymeminfo()
{
    size_t total=0,used=0,max_used=0;
    main_meminfo(&total,&used,&max_used);
    main_debug_print("memory total: %" PRIu32  " bytes,max_used: %" PRIu32 " bytes,free: %" PRIu32 " bytes",total,max_used,total-used);
}

static char * global=new char[30];

void main_task(void *param)
{
    main_debug_print("main start!");

    {
        //打印banner
        char * banner=(char *)RCGetHandle("banner");
        if(banner!=NULL)
            main_debug_print("\r\n%s\r\n",banner);
    }

    displaymeminfo();

    uint32_t main_loop_print_tick_ms=main_task_gettick_ms();
    while(true)
    {

        {
            //处理事件循环
            heventloop_process_event(main_get_eventloop());
        }

        {
            //处理槽函数，参数为空
            heventslots_emit_signal(main_get_mainloop_slot(),NULL);
        }

        if(main_task_gettick_ms()-main_loop_print_tick_ms > 60000)
        {
            main_loop_print_tick_ms=main_task_gettick_ms();
            main_debug_print("main loop!");
            displaymeminfo();

        }

        main_task_sleep(1);
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
