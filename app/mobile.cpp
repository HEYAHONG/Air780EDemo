#include "mobile.h"
#include "heventloop.h"

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
    while(true)
    {
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
