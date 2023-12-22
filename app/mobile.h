#ifndef __MOBILE_H__
#define __MOBILE_H__
#include "main.h"
#include "heventloop.h"
#include "heventchain.h"
#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

/** \brief 添加事件到事件循环(mobile)
 *
 * \param  event_usr void* 事件中的用户参数
 * \param  event_process 事件处理,第一个参数为用户参数,第二个参数为heventloop_t指针。
 * \param  event_onfree  事件释放回调,第一个参数为用户参数,第二个参数为heventloop_t指针。通常用于释放用户参数。
 * \return bool 是否添加成功,当内存不足或者到达最大事件数时将添加失败。
 *
 */
bool mobile_add_event(void *event_usr,void(*event_process)(void *,heventloop_t *),void(*event_onfree)(void *,heventloop_t *));

/** \brief 是否同步时间
 *
 * \return bool 是否同步时间
 *
 */
bool mobile_is_time_sync_ok();

/** \brief 是否NETIF准备好
 *
 * \return bool 是否NETIF准备好
 *
 */
bool mobile_is_netif_ok();

typedef struct
{
    LUAT_MOBILE_EVENT_E event;
    uint8_t index;
    uint8_t status;
} luat_mobile_event_callback_data_t; /**< 事件数据 */

/** \brief 安装事件钩子
 *
 * \param priority uint32_t 优先级,数值越小优先级越高,同一优先级后安装的优先级高。
 * \param hook_usr void*钩子中的用户参数
 * \param hook 钩子函数,第一个参数为传入参数(luat_mobile_event_callback_data_t *),第二个参数为用户参数,返回值为是否成功处理,通常返回false以传递到后面的钩子。
 * \param onfree 槽释放回调,第一个参数为用户参数。通常在卸载钩子时调用
 * \return uint32_t 注册id,为0表示失败
 *
 */

uint32_t mobile_install_hook(uint32_t priority,void *hook_usr,bool (*hook)(void *,void *),void (*onfree)(void *));

/** \brief 卸载事件钩子
 *
 * \param id uint32_t 注册id,由安装时返回
 *
 */
void mobile_uninstall_hook(uint32_t id);


#ifdef __cplusplus
}
#endif // __cplusplus

#endif // __MOBILE_H__
