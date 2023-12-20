#include "luat_network_adapter.h"
#include "common_api.h"
#include "luat_debug.h"
#include "luat_rtos.h"
#include "luat_mobile.h"
#include "net_lwip.h"
#include "luat_mem.h"
#include "luat_uart.h"
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"



void main_task_sleep(uint32_t timeout_ms)
{
    luat_rtos_task_sleep(timeout_ms);
}

uint32_t main_task_gettick_ms()
{
    return xTaskGetTickCount() * (1000/configTICK_RATE_HZ);
}

static int32_t critical_nested=0;
static uint32_t critical_data=0;
void main_enter_critical()
{
    if(critical_nested==0)
    {
        critical_data=luat_rtos_entry_critical();
    }
    critical_nested++;
}

void main_exit_critical()
{
    critical_nested--;
    if(critical_nested==0)
    {
        critical_data=luat_rtos_entry_critical();
    }
}

void * main_malloc(size_t bytes)
{
    return luat_heap_malloc(bytes);
}

void main_free(void *ptr)
{
    luat_heap_free(ptr);
}

void main_meminfo(size_t *total,size_t *used,size_t *max_used)
{
    luat_meminfo_sys(total,used,max_used);
}

int main_debug_print(const char * fmt,...)
{
    va_list ap;
    va_start(ap, fmt);
    soc_vsprintf(0, fmt, ap);
    va_end(ap);
    return 0;
}

static luat_rtos_task_handle main_task_handle;

void main_init(void)
{

    main_debug_print("main init start!");

    {
        //初始化C++
        typedef void(*pfunc)();
        extern pfunc __ctors_start__[];
        extern pfunc __ctors_end__[];
        pfunc *p=NULL;
        for (p = __ctors_start__; p < __ctors_end__; p++)
        {
            if(p!=NULL)
            {
                (*p)();
            }
        }
    }

    //初始化蜂窝网络
    net_lwip_init();
    net_lwip_register_adapter(NW_ADAPTER_INDEX_LWIP_GPRS);
    network_register_set_default(NW_ADAPTER_INDEX_LWIP_GPRS);

    //创建主任务
    luat_rtos_task_create(&main_task_handle,8192,10,"main",main_task,NULL,16);

}

INIT_TASK_EXPORT(main_init, "1");
