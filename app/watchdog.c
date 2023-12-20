#include "watchdog.h"
#include "common_api.h"
#include "main.h"
#include "luat_rtos.h"
#include "luat_wdt.h"
#include "reset.h"


static void sys_reset()
{
    ResetECSystemReset();
    //若NVIC复位未实现，使用死循环通过硬件看门狗复位
    while(true)
    {

    }
}


static uint32_t tick_ms_up=0;//高32位
static uint64_t last_ms_tick=0;
static uint64_t sys_tick_ms()
{
    uint64_t current_tick=((uint64_t)tick_ms_up<<32)+main_task_gettick_ms();
    while(current_tick < last_ms_tick)
    {
        tick_ms_up++;
        current_tick=((uint64_t)tick_ms_up<<32)+main_task_gettick_ms();
    }
    last_ms_tick=current_tick;
    return current_tick;
}


static void hw_feed()
{
    luat_wdt_feed();
}

/*
    槽,由主循环发送信号(调用)
*/
static void mainloop_slot(void *signal,void *usr)
{
    (void)signal;
    (void)usr;
    //喂狗
    hwatchdog_feed();
}

static void watchdog_init(void)
{
    //设置好看门狗
    hwatchdog_set_hardware_dog_feed(hw_feed);
    hwatchdog_setup_software_dog(sys_reset,sys_tick_ms);


    //在主循环中喂狗
    heventslots_register_slot(main_get_mainloop_slot(),NULL,mainloop_slot,NULL);
}

INIT_TASK_EXPORT(watchdog_init, "9");
