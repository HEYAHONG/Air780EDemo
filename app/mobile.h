#ifndef __MOBILE_H__
#define __MOBILE_H__
#include "main.h"
#include "heventloop.h"
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

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // __MOBILE_H__
