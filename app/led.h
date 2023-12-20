#ifndef __LED_H_INCLUDED__
#define __LED_H_INCLUDED__
#include "stdint.h"
#include "inttypes.h"
#include "stdlib.h"
#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus
/*
    本文件中LED指NET_STATUS灯(GPIO27)
*/
#define LED_PIN 27 /**< LED引脚编号 */

/** \brief LED获取亮灭表
 *
 * \return uint64_t 亮灭表，每bits表示100ms，共64bits
 *
 */
uint64_t led_get_table();

/** \brief LED设置亮灭表
 *
 * \param table uint64_t 亮灭表，每bits表示100ms，共64bits
 *
 */
void led_set_table(uint64_t table);

/** \brief LED初始化(必须在任务中调用，且只能调用一次)
 *
 *
 */
void led_init(void);

#ifdef __cplusplus
}
#endif // __cplusplus


#endif // __LED_H_INCLUDED__
