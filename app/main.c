#include "luat_network_adapter.h"
#include "common_api.h"
#include "net_lwip.h"
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "led.h"
#include "watchdog.h"



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
        luat_rtos_exit_critical(critical_data);
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


const char *main_get_imei()
{
    static char imei[20]= {0};
    if(strlen(imei) < 10)
    {
        luat_mobile_get_imei(0, imei, sizeof(imei));
    }
    return imei;
}

static heventloop_t *main_loop=NULL;
heventloop_t *main_get_eventloop()
{
    if(main_loop==NULL)
    {
        main_loop=heventloop_new(NULL);
    }
    return main_loop;
}

bool main_add_event(void *event_usr,void(*event_process)(void *,heventloop_t *),void(*event_onfree)(void *,heventloop_t *))
{
    return heventloop_add_event_ex1(main_get_eventloop(),event_usr,event_process,event_onfree);
}

static heventslots_t *main_loop_slot=NULL;
heventslots_t *main_get_mainloop_slot()
{
    if(main_loop_slot==NULL)
    {
        main_loop_slot=heventslots_new(NULL);
    }
    return main_loop_slot;
}


uint32_t main_register_mainloop_slot(void *slot_usr,void (*slot)(void *,void *),void (*onfree)(void *))
{
    return heventslots_register_slot(main_get_mainloop_slot(),slot_usr,slot,onfree);
}

void main_unregister_mainloop_slot(uint32_t id)
{
    heventslots_unregister_slot(main_get_mainloop_slot(),id);
}

static luat_rtos_task_handle main_task_handle;

static bool main_memory_check(hwatchdog_watch_info_t* info)
{
    (void)info;
    {
        size_t total=0,used=0,max_used=0;
        static bool is_memory_low=false;
        main_meminfo(&total,&used,&max_used);
        if(total-used < 10*1024)
        {
            if(!is_memory_low)
            {
                main_debug_print("memory is low!\r\n");
            }
            is_memory_low=true;
            return false;
        }
        else
        {
            is_memory_low=false;
        }
    }
    return true;
}

/*
在任务中处理的操作
*/
static void main_init_in_task(void*usr,heventloop_t*loop)
{

    (void)usr;
    (void)loop;

    main_debug_print("main init start!poweron reason=%d!\r\n",luat_pm_get_poweron_reason());

    {
        main_debug_print("init filesystem!\r\n");
        luat_fs_init();
    }

    {
        //初始化KV存储
        main_debug_print("init kv!\r\n");
        luat_kv_init();
    }

    {
        //初始化蜂窝网络
        main_debug_print("init lwip!\r\n");
        net_lwip_init();
        net_lwip_register_adapter(NW_ADAPTER_INDEX_LWIP_GPRS);
        network_register_set_default(NW_ADAPTER_INDEX_LWIP_GPRS);
    }

    {
        //初始化LED
        main_debug_print("init led!\r\n");
        led_init();
    }



    {
        //初始化C++
        main_debug_print("init cpp environment!\r\n");
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

    {
        //通过看门狗检查内存大小
        HWATCHDOG_ADD_WATCH(main_memory_check,30000,NULL);
        main_debug_print("memory watchdog init!\r\n");
    }

}

static void main_init(void)
{

    //将需要在任务中初始化的工作添加至主任务
    main_add_event(NULL,main_init_in_task,NULL);

    //创建主任务
    luat_rtos_task_create(&main_task_handle,8192,0,"main",main_task,NULL,16);

}

INIT_TASK_EXPORT(main_init, "9");
