#ifndef __MQTTMANAGER_H__
#define __MQTTMANAGER_H__

#include "main.h"
#include "mobile.h"
#include "watchdog.h"

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

/*
    防止__FORCEINLINE与__STATIC_INLINE同时使用时报错
*/
#ifdef __FORCEINLINE
//取消__FORCEINLINE
#undef __FORCEINLINE
#define __FORCEINLINE __attribute__((unused))
#endif // __FORCEINLINE

#include "luat_network_adapter.h"
#include "libemqtt.h"
#include "luat_mqtt.h"
#include "net_lwip.h"
#include "RC.h"

#ifdef __cplusplus
}
#endif // __cplusplus

#ifdef __cplusplus

#include <functional>
#include <string>

/*
    MQTT客户端，注意:需要在网络初始化后使用,若在网络初始化前使用，需确保不会被析构。一般情况下，不要使用局部变量实例化MQTT客户端，可使用全局变量
*/
class mqttclient
{
public:
    class connect_opts
    {
    public:
        //连接参数
        std::string host;
        int port;

        //TLS连接参数
        bool is_ssl;
        std::string ca;
        std::string cert;
        std::string key;

        //客户端id
        std::string id;

        //认证
        std::string username;
        std::string password;

        //遗嘱消息
        std::string will_topic;
        std::string will_payload;
        uint8_t will_qos;
        uint8_t will_retain;

        //其他参数
        bool clean_session;

        uint32_t keepalive;

        connect_opts():host(),port(1883),is_ssl(false),ca(),cert(),key(),id(),username(),password(),will_topic(),will_payload(),will_qos(0),will_retain(0),clean_session(true),keepalive(240)
        {

        }
        ~connect_opts()
        {

        }
    };
private:
    luat_mqtt_ctrl_t  m_mqtt;
    uint16_t publish_message_id;
    uint16_t subscribe_message_id;
    connect_opts m_conenct_opts;
    void init();
    bool is_init()
    {
        return m_mqtt.userdata != NULL;
    }
    void create()
    {
        if(!is_init())
        {
            if(luat_mqtt_init(&m_mqtt,NW_ADAPTER_INDEX_LWIP_GPRS)==0)
            {
                m_mqtt.ip_addr.type=0xff;
                m_mqtt.userdata=this;
            }

        }
    }
    void destroy()
    {
        if(is_init())
        {
            luat_mqtt_close_socket(&m_mqtt);
            luat_mqtt_release_socket(&m_mqtt);
            memset(&m_mqtt,0,sizeof(m_mqtt));
            m_mqtt.userdata = NULL;
        }
    }
    static void luat_mqtt_cb(luat_mqtt_ctrl_t *luat_mqtt_ctrl, uint16_t event);
    friend int luat_mqtt_set_cb(luat_mqtt_ctrl_t *mqtt_ctrl, luat_mqtt_cb_t mqtt_cb);
    friend class mqttmanager;
public:
    mqttclient():m_mqtt(),publish_message_id(0),subscribe_message_id(0),m_conenct_opts()
    {
        memset(&m_mqtt,0,sizeof(m_mqtt));
        m_mqtt.userdata=NULL;
        init();
    }
    virtual ~mqttclient()
    {
        destroy();
    }
    mqttclient(mqttclient &oths) = delete;
    mqttclient &operator =(mqttclient &oths) = delete;

    connect_opts &get_connect_opts()
    {
        return m_conenct_opts;
    }

    bool connect(connect_opts *opts=NULL)
    {
        if(!is_init())
        {
            return false;
        }
        if(luat_mqtt_state_get(&m_mqtt)!=MQTT_STATE_DISCONNECT)
        {
            //已连接
            return false;
        }
        if(opts == NULL)
        {
            opts = &get_connect_opts();
        }
        {
            //连接参数
            if(opts->host.empty())
            {
                return false;
            }
            luat_mqtt_connopts_t c_opts;
            memset(&c_opts,0,sizeof(c_opts));
            c_opts.is_tls = opts->is_ssl?1:0;
            if(opts->is_ssl)
            {
                //加载EMQX CA文件
                if(opts->ca.empty())
                {
                    const char * rc=(char *)RCGetHandle("emqx/cacert.pem");
                    if(rc)
                    {
                        opts->ca=std::string(rc);
                    }
                }
                c_opts.server_cert = opts->ca.c_str();
                c_opts.server_cert_len = opts->ca.length();
                c_opts.client_cert = opts->cert.c_str();
                c_opts.client_cert_len = opts->cert.length();
                c_opts.client_key = opts->key.c_str();
                c_opts.client_key_len = opts->key.length();
            }

            if(opts->port == 0)
            {
                c_opts.port=1883;
            }
            else
            {
                c_opts.port=opts->port;
            }

            c_opts.host=opts->host.c_str();

            if(luat_mqtt_set_connopts(&m_mqtt,&c_opts)!=0)
            {
                return false;
            }
        }

        {
            //id设置
            if(opts->id.empty())
            {
                opts->id = main_get_imei();
            }
            mqtt_init(&(m_mqtt.broker),opts->id.c_str());
        }

        {
            //用户名与密码
            if(!opts->username.empty())
            {
                mqtt_init_auth(&(m_mqtt.broker),opts->username.c_str(),opts->password.c_str() );
            }
        }

        {
            //遗嘱消息
            if(!opts->will_topic.empty())
            {
                luat_mqtt_set_will(&m_mqtt, opts->will_topic.c_str(), opts->will_payload.c_str(),  opts->will_payload.length(), opts->will_qos, opts->will_retain);
            }

        }

        {
            //其它参数
            if(opts->clean_session)
            {
                m_mqtt.broker.clean_session = 1;
            }
            else
            {
                m_mqtt.broker.clean_session = 0;
            }

            m_mqtt.reconnect = 1;
            m_mqtt.reconnect_time = 3000;

            m_mqtt.keepalive=opts->keepalive;
        }

        luat_mqtt_set_cb(&m_mqtt,luat_mqtt_cb);

        if(luat_mqtt_connect(&m_mqtt)!=0)
        {
            return false;
        }

        return true;
    }

    bool disconnect()
    {
        if(!is_init())
        {
            return false;
        }
        luat_mqtt_close_socket(&m_mqtt);
        return true;

    }

    bool reconnect()
    {
        if(!is_init())
        {
            return false;
        }
        return luat_mqtt_reconnect(&m_mqtt)==0;
    }

    bool is_connected()
    {
        if(!is_init())
        {
            return false;
        }
        return luat_mqtt_state_get(&m_mqtt)==MQTT_STATE_READY;
    }

    //发布消息
    bool publish(std::string topic,std::string payload,uint8_t qos=0,uint8_t retain=0)
    {
        if(!is_connected())
        {
            return false;
        }
        if(topic.empty())
        {
            return false;
        }
        return mqtt_publish_with_qos(&(m_mqtt.broker),topic.c_str(),payload.c_str(),payload.length(),retain,qos,&publish_message_id)==0;
    }

    //订阅消息(一般在连接回调订阅)
    bool subscribe(std::string topic,uint8_t qos = 0)
    {
        if(!is_connected())
        {
            return false;
        }
        if(topic.empty())
        {
            return false;
        }
        return mqtt_subscribe(&(m_mqtt.broker), topic.c_str(), &subscribe_message_id,qos) == 0;
    }

    //虚函数(对外接口),子类可通过重载接口实现相应功能

    //MQTT初始化,一般在此函数中连接MQTT
    virtual void on_init() = 0;
    //MQTT连接,一般在此处订阅主题，发布上线消息
    virtual void on_connect() = 0;
    //MQTT连接，当is_close为真时可尝试手动重连
    virtual void on_disconnect(bool is_close) = 0;
    //MQTT数据，接收到的数据在此处处理
    virtual void on_data(std::string topic,std::string payload) = 0;

};

#endif // __cplusplus

#endif // __MQTTMANAGER_H__
