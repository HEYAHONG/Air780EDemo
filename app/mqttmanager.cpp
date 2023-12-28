#include "mqttmanager.h"

/*
    此文件主要管理MQTT相关事件。可将MQTT的回调转入MQTT管理器所在线程执行
    单例模式
*/

static class mqttmanager
{
    luat_rtos_task_handle m_task_handle;
    luat_rtos_mutex_t m_lock;
    void lock()
    {
        if(m_lock==NULL)
        {
            return;
        }
        luat_rtos_mutex_lock(m_lock,LUAT_WAIT_FOREVER);
    }
    void unlock()
    {
        if(m_lock==NULL)
        {
            return;
        }
        luat_rtos_mutex_unlock(m_lock);
    }
    heventloop_t *loop;
    void run();
public:
    static void task_entry(void *param)
    {
        if(param!=NULL)
        {
            ((mqttmanager*)param)->run();
        }
    }
    mqttmanager():m_task_handle(NULL),m_lock(NULL),loop(NULL)
    {
        luat_rtos_mutex_create(&m_lock);
        luat_rtos_task_create(&m_task_handle,8192,10,"mqttmanager",task_entry,this,16);

    }
    virtual ~mqttmanager()
    {

    }
    mqttmanager(mqttmanager &oths) = delete;
    mqttmanager & operator =(mqttmanager &oths) = delete;
    bool add_event(void *event_usr,void(*event_process)(void *,heventloop_t *),void(*event_onfree)(void *,heventloop_t *))
    {
        if(loop == NULL)
        {
            loop=heventloop_new(this);
        }
        return heventloop_add_event_ex1(loop,event_usr,event_process,event_onfree);
    }
    bool add_event(std::function<void()> event)
    {
        std::function<void()> *event_ptr=new std::function<void()>();
        (*event_ptr)=event;
        bool  ret=add_event(event_ptr,[](void *usr,heventloop_t *loop)
        {
            if(usr !=NULL && loop != NULL)
            {
                mqttmanager *m_mqttmanager=(mqttmanager *)heventloop_get_usr_ptr(loop);
                if(m_mqttmanager != NULL)
                {
                    std::function<void()> &m_event=*(std::function<void()> *)usr;
                    if(m_event!=NULL)
                    {
                        m_event();
                    }
                }
            }
        },
        [](void *usr,heventloop_t *loop)
        {
            (void)loop;
            if(usr != NULL)
            {
                std::function<void()> *m_event_ptr=(std::function<void()> *)usr;
                delete m_event_ptr;
            }
        }
                           );
        return ret;
    }
} g_manager;


void mqttmanager::run()
{
    main_debug_print("mqttmanager thread started!\r\n");
    uint32_t loop_start_tick=main_task_gettick_ms();
    {
        //通过看门狗检查循环起始tick
        HWATCHDOG_ADD_WATCH([](hwatchdog_watch_info_t* info) -> bool
        {
            if(info!=NULL && info->usr !=NULL)
            {
                uint32_t loop_start_tick=*(uint32_t *)info->usr;
                if(main_task_gettick_ms()-loop_start_tick > (CONFIG_MQTT_WATCHDOG_TIMEOUT * 1000))//检查是否超时
                {
                    main_debug_print("mqttmanager watchdog reset!\r\n");
                    return false;
                }
            }
            return true;
        },0,&loop_start_tick);
        main_debug_print("mqttmanager watchdog init!\r\n");
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

void mqttclient::init()
{
    auto cb = [&,this]()
    {
        if(mobile_is_netif_ok())
        {
            //需要网络准备好，才能开启MQTT
            create();
            if(is_init())
            {
                on_init();
            }
        }
        else
        {
            init();
        }
    };
    g_manager.add_event(cb);
}

void mqttclient::luat_mqtt_cb(luat_mqtt_ctrl_t *luat_mqtt_ctrl, uint16_t event)
{
    if(luat_mqtt_ctrl->userdata == NULL)
    {
        return;
    }
    mqttclient *client=(mqttclient*)luat_mqtt_ctrl->userdata;
    switch(event)
    {
    case MQTT_MSG_CONNACK:
    {
        g_manager.add_event([=]()
        {
            client->on_connect();
        });

    }
    break;
    case MQTT_MSG_PUBLISH :
    {
        std::string topic;
        {
            const uint8_t *ptr=NULL;
            size_t len=mqtt_parse_pub_topic_ptr(client->m_mqtt.mqtt_packet_buffer,&ptr);
            if(ptr != NULL && len != 0)
            {
                topic=std::string((char *)ptr,len);
            }
        }
        std::string payload;
        {
            const uint8_t *ptr=NULL;
            size_t len=mqtt_parse_pub_msg_ptr(client->m_mqtt.mqtt_packet_buffer,&ptr);
            if(ptr != NULL && len != 0)
            {
                payload=std::string((char *)ptr,len);
            }
        }
        g_manager.add_event([=]()
        {
            client->on_data(topic,payload);
        });
    }
    break;
    case MQTT_MSG_PUBACK :
    case MQTT_MSG_PUBCOMP :
    {
        client->publish_message_id = mqtt_parse_msg_id(client->m_mqtt.mqtt_packet_buffer);

    }
    break;
    case MQTT_MSG_RELEASE :
    {

    }
    break;
    case MQTT_MSG_DISCONNECT :
    {
        g_manager.add_event([=]()
        {
            client->on_disconnect(false);
        });
    }
    break;
    case MQTT_MSG_CLOSE :
    {
        g_manager.add_event([=]()
        {
            client->on_disconnect(true);
        });
    }
    break;
    default:
        break;

    }

}
