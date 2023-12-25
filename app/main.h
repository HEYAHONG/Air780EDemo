#ifndef __MAIN_H__
#define __MAIN_H__

#include "Kconfig.h"
#include "stdint.h"
#include "stdlib.h"
#include "inttypes.h"
#include "string.h"
#include "stdio_patch.h"

#include "heventloop.h"
#include "heventslots.h"

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

/*
    luat接口
*/
#include "luat_adc.h"
#include "luat_adc_legacy.h"
#include "luat_air153C_wtd.h"
#include "luat_camera.h"
#include "luat_crypto.h"
#include "luat_debug.h"
#include "luat_flash.h"
#include "luat_fs.h"
#include "luat_gpio.h"
#include "luat_gpio_legacy.h"
#include "luat_i2c.h"
#include "luat_i2c_legacy.h"
#include "luat_iconv.h"
#include "luat_irq.h"
#include "luat_kv.h"
#include "luat_mcu.h"
#include "luat_mem.h"
#include "luat_mobile.h"
#include "luat_otp.h"
#include "luat_pm.h"
#include "luat_pwm.h"
#include "luat_rtc.h"
#include "luat_rtos.h"
#include "luat_rtos_legacy.h"
#include "luat_sms.h"
#include "luat_spi.h"
#include "luat_spi_legacy.h"
#include "luat_uart.h"
#include "luat_uart_legacy.h"
#include "luat_wdt.h"
#include "luat_wifiscan.h"

void main_task(void *param);

void main_task_sleep(uint32_t timeout_ms);

uint32_t main_task_gettick_ms(); /**< 获取当前tick(ms),只能在任务中调用 */

void main_enter_critical();

void main_exit_critical();

void * main_malloc(size_t bytes);

void main_free(void *ptr);

void main_meminfo(size_t *total,size_t *used,size_t *max_used);

#include "printf.h"
void main_app_log_output(char character, void* arg);
#ifndef main_debug_print
#define main_debug_print(fmt,...) {\
                                    luat_debug_print(fmt,##__VA_ARGS__);\
                                    main_enter_critical();\
                                    fctprintf(main_app_log_output,NULL,fmt,##__VA_ARGS__);\
                                    main_exit_critical();\
                                   }
#endif // main_debug_print


const char *main_get_imei();

/** \brief 获取主事件循环
 *
 * \return heventloop_t * 事件循环指针，参考heventloop.h中的描述
 *
 */
heventloop_t *main_get_eventloop();

/** \brief 添加事件到事件循环
 *
 * \param  event_usr void* 事件中的用户参数
 * \param  event_process 事件处理,第一个参数为用户参数,第二个参数为heventloop_t指针。
 * \param  event_onfree  事件释放回调,第一个参数为用户参数,第二个参数为heventloop_t指针。通常用于释放用户参数。
 * \return bool 是否添加成功,当内存不足或者到达最大事件数时将添加失败。
 *
 */
bool main_add_event(void *event_usr,void(*event_process)(void *,heventloop_t *),void(*event_onfree)(void *,heventloop_t *));

/** \brief 获取主循环槽(在主循环中多次调用)
 *
 * \return heventslots_t*主循环槽指针,详情参考heventslots.h
 *
 */
heventslots_t *main_get_mainloop_slot();

/** \brief 注册主循环槽函数（一般用于需要周期调用的函数）
 *
 * \param slot_usr void*槽中的用户参数
 * \param slot 槽函数,第一个参数为信号(一般为空),第二个参数为用户参数。
 * \param onfree 槽释放回调,第一个参数为用户参数。
 * \return uint32_t 注册id,为0表示失败
 *
 */

uint32_t main_register_mainloop_slot(void *slot_usr,void (*slot)(void *,void *),void (*onfree)(void *));


/** \brief 取消注册主循环槽函数
 *
 * \param id uint32_t 注册id,由注册时返回
 *
 */
void main_unregister_mainloop_slot(uint32_t id);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // __MAIN_H__




