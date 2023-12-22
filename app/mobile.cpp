#include "mobile.h"
#include "heventloop.h"
#include "watchdog.h"

typedef struct
{
    LUAT_MOBILE_EVENT_E event;
    uint8_t index;
    uint8_t status;
} luat_mobile_event_callback_data_t;
/*
    此文件主要对mobile相关事件进行处理，（主要是创建一个线程转发事件,即在任务中执行事件处理函数）。
    单例模式
*/
static class mobile
{
    static luat_rtos_task_handle task_handle;
    heventloop_t *loop;
    void run();
    void process_callback(luat_mobile_event_callback_data_t &data);
public:
    static void luat_mobile_event_callback(LUAT_MOBILE_EVENT_E event, uint8_t index, uint8_t status);
    static void task_entry(void *param)
    {
        if(param!=NULL)
        {
            ((mobile*)param)->run();
        }
    }
    mobile():loop(NULL)
    {
        loop=heventloop_new(this);
        luat_rtos_task_create(&task_handle,8192,10,"mobile",task_entry,this,16);
    }
    mobile(mobile &oths) = delete;
    mobile &operator =(mobile &oths) = delete;
    ~mobile()
    {

    }
    bool add_event(void *event_usr,void(*event_process)(void *,heventloop_t *),void(*event_onfree)(void *,heventloop_t *))
    {
        return heventloop_add_event_ex1(loop,event_usr,event_process,event_onfree);
    }

} g_mobile;

luat_rtos_task_handle mobile::task_handle=NULL;

void mobile::process_callback(luat_mobile_event_callback_data_t &data)
{
    //此处处理事件(注意:开机时的事件可能会丢失)
    main_debug_print("mobile_event:%d,%d,%d",(int)data.event,(int)data.index,(int)data.status);
    {
        LUAT_MOBILE_EVENT event=data.event;
        uint8_t index=data.index;
        uint8_t status=data.status;
        luat_mobile_cell_info_t cell_info;
        luat_mobile_signal_strength_info_t signal_info;
        uint8_t csq, i;
        char imsi[20];
        char iccid[24] = {0};
        char apn[32] = {0};
        ip_addr_t ipv4;
        ip_addr_t ipv6;
        switch(event)
        {
        case LUAT_MOBILE_EVENT_CFUN:
            main_debug_print("CFUN消息，status %d", status);
            break;
        case LUAT_MOBILE_EVENT_SIM:
            if (status != LUAT_MOBILE_SIM_NUMBER)
            {
                main_debug_print("SIM卡消息，卡槽%d", index);
            }
            switch(status)
            {
            case LUAT_MOBILE_SIM_READY:
                main_debug_print("SIM卡正常工作");
                luat_mobile_get_iccid(index, iccid, sizeof(iccid));
                main_debug_print("ICCID %s", iccid);
                luat_mobile_get_imsi(index, imsi, sizeof(imsi));
                main_debug_print("IMSI %s", imsi);
                break;
            case LUAT_MOBILE_NO_SIM:
                main_debug_print("SIM卡不存在");
                break;
            case LUAT_MOBILE_SIM_NEED_PIN:
                main_debug_print("SIM卡需要输入PIN码");
                break;
            }
            break;
        case LUAT_MOBILE_EVENT_REGISTER_STATUS:
            main_debug_print("移动网络服务状态变更，当前为%d", status);
            break;
        case LUAT_MOBILE_EVENT_CELL_INFO:
            switch(status)
            {
            case LUAT_MOBILE_CELL_INFO_UPDATE:
                main_debug_print("周期性搜索小区信息完成一次");
                luat_mobile_get_last_notify_cell_info(&cell_info);
                if (cell_info.lte_service_info.cid)
                {
                    main_debug_print("服务小区信息 mcc %x mnc %x cellid %u band %d tac %u pci %u earfcn %u is_tdd %d rsrp %d rsrq %d snr %d rssi %d",
                                     cell_info.lte_service_info.mcc, cell_info.lte_service_info.mnc, cell_info.lte_service_info.cid,
                                     cell_info.lte_service_info.band, cell_info.lte_service_info.tac, cell_info.lte_service_info.pci, cell_info.lte_service_info.earfcn,
                                     cell_info.lte_service_info.is_tdd, cell_info.lte_service_info.rsrp, cell_info.lte_service_info.rsrq,
                                     cell_info.lte_service_info.snr, cell_info.lte_service_info.rssi);
                }
                for (i = 0; i < cell_info.lte_neighbor_info_num; i++)
                {
                    if (cell_info.lte_info[i].cid)
                    {
                        main_debug_print("邻小区 %d mcc %x mnc %x cellid %u tac %u pci %u", i + 1, cell_info.lte_info[i].mcc,
                                         cell_info.lte_info[i].mnc, cell_info.lte_info[i].cid, cell_info.lte_info[i].tac, cell_info.lte_info[i].pci);
                        main_debug_print("邻小区 %d earfcn %u rsrp %d rsrq %d snr %d", i + 1, cell_info.lte_info[i].earfcn, cell_info.lte_info[i].rsrp,
                                         cell_info.lte_info[i].rsrq, cell_info.lte_info[i].snr);
                    }
                }
                break;
            case LUAT_MOBILE_SIGNAL_UPDATE:
                main_debug_print("服务小区信号状态变更");
                luat_mobile_get_last_notify_signal_strength_info(&signal_info);
                luat_mobile_get_last_notify_signal_strength(&csq);
                if (signal_info.luat_mobile_lte_signal_strength_vaild)
                {
                    main_debug_print("rsrp %d, rsrq %d, snr %d, rssi %d, csq %d %d", signal_info.lte_signal_strength.rsrp,
                                     signal_info.lte_signal_strength.rsrq, signal_info.lte_signal_strength.snr,
                                     signal_info.lte_signal_strength.rssi, csq, luat_mobile_rssi_to_csq(signal_info.lte_signal_strength.rssi));
                }

                break;
            }
            break;
        case LUAT_MOBILE_EVENT_PDP:
            main_debug_print("CID %d PDP激活状态变更为 %d", index, status);
            break;
        case LUAT_MOBILE_EVENT_NETIF:
            main_debug_print("internet工作状态变更为 %d,cause %d", status,index);
            switch (status)
            {
            case LUAT_MOBILE_NETIF_LINK_ON:
                main_debug_print("可以上网");
                if (luat_mobile_get_apn(0, 0, apn, sizeof(apn)))
                {
                    main_debug_print("默认apn %s", apn);
                }
                luat_mobile_get_local_ip(0, 1, &ipv4, &ipv6);
                if (ipv4.type != 0xff)
                {
                    main_debug_print("IPV4 %s", ip4addr_ntoa(&ipv4.u_addr.ip4));
                }
                if (ipv6.type != 0xff)
                {
                    main_debug_print("IPV6 %s", ip6addr_ntoa(&ipv4.u_addr.ip6));
                }
                break;
            default:
                main_debug_print("不能上网");
                break;
            }
            break;
        case LUAT_MOBILE_EVENT_TIME_SYNC:
            main_debug_print("通过移动网络同步了UTC时间");
            break;
        case LUAT_MOBILE_EVENT_CSCON:
            main_debug_print("RRC状态 %d", status);
            break;
        case LUAT_MOBILE_EVENT_NAS_ERROR:
            main_debug_print("NAS异常类型 %d，拒绝原因 %d", index, status);
            break;
        case LUAT_MOBILE_EVENT_FATAL_ERROR:
            main_debug_print("网络需要严重故障，建议在5秒后重启协议栈");
            break;
        default:
            break;
        }

    }
}

void mobile::luat_mobile_event_callback(LUAT_MOBILE_EVENT_E event, uint8_t index, uint8_t status)
{
    luat_mobile_event_callback_data_t *data=new luat_mobile_event_callback_data_t;
    data->event=event;
    data->index=index;
    data->status=status;
    mobile_add_event(data,[](void *usr,heventloop_t *loop)
    {
        if(usr !=NULL)
        {
            luat_mobile_event_callback_data_t *data=(luat_mobile_event_callback_data_t *)usr;
            mobile *m_mobile_ptr=(mobile*)heventloop_get_usr_ptr(loop);
            if(m_mobile_ptr!=NULL)
            {
                m_mobile_ptr->process_callback(*data);
            }
        }
    },
    [](void *usr,heventloop_t *loop)
    {
        (void)loop;
        if(usr !=NULL)
        {
            luat_mobile_event_callback_data_t *data=(luat_mobile_event_callback_data_t *)usr;
            delete data;
        }
    }
                    );
}

void mobile::run()
{
    main_debug_print("mobile thread started!");
    luat_mobile_event_register_handler(luat_mobile_event_callback);
    uint32_t loop_start_tick=main_task_gettick_ms();
    {
        //通过看门狗检查循环起始tick
        HWATCHDOG_ADD_WATCH([](hwatchdog_watch_info_t* info) -> bool
        {
            if(info!=NULL && info->usr !=NULL)
            {
                uint32_t loop_start_tick=*(uint32_t *)info->usr;
                if(main_task_gettick_ms()-loop_start_tick > 120000)//检查是否超时
                {
                    main_debug_print("mobile watchdog reset!");
                    return false;
                }
            }
            return true;
        },0,&loop_start_tick);
        main_debug_print("mobile watchdog init!");
    }
    while(true)
    {
        {
            //更新循环起始节拍
            main_enter_critical();
            loop_start_tick=main_task_gettick_ms();
            main_exit_critical();
        }
        if(heventloop_has_events(loop)!=0)
        {
            heventloop_process_event(loop);
            main_task_sleep(1);
        }
        else
        {
            main_task_sleep(200);
        }

    }
}

/** \brief 添加事件到事件循环(mobile)
 *
 * \param  event_usr void* 事件中的用户参数
 * \param  event_process 事件处理,第一个参数为用户参数,第二个参数为heventloop_t指针。
 * \param  event_onfree  事件释放回调,第一个参数为用户参数,第二个参数为heventloop_t指针。通常用于释放用户参数。
 * \return bool 是否添加成功,当内存不足或者到达最大事件数时将添加失败。
 *
 */
bool mobile_add_event(void *event_usr,void(*event_process)(void *,heventloop_t *),void(*event_onfree)(void *,heventloop_t *))
{
    return g_mobile.add_event(event_usr,event_process,event_onfree);
}
