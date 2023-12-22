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

int main_debug_print(const char * fmt,...);


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

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // __MAIN_H__




