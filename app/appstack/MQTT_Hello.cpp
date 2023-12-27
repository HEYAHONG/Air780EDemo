#include "mqttmanager.h"

/*
 Hello MQTT协议栈,用于MQTT测试。
 主题为2层,分别为目的地址与源地址。接收到消息直接调转目的地址与源地址原样回复。
*/


#if CONFIG_MQTT_STACK_HELLO

//默认目标地址
#define DEFAULT_DEST_ADDR "Air780E_Hello"


class mqtthello:public mqttclient
{
public:
    mqtthello()
    {

    }
    virtual ~mqtthello()
    {

    }
    //MQTT初始化,一般在此函数中连接MQTT
    virtual void on_init() override
    {
        main_debug_print("MQTT Hello:init!\r\n");
        connect_opts &opts=get_connect_opts();
        opts.host=CONFIG_MQTT_STACK_HELLO_HOST;
        opts.port=CONFIG_MQTT_STACK_HELLO_PORT;
        opts.id=main_get_imei();

        {
            //遗嘱消息
            opts.will_qos = 0;
            opts.will_retain = false;
            opts.will_payload = "Offline";
            opts.will_topic = (std::string(DEFAULT_DEST_ADDR) + "/" + opts.id);

        }

        {
            //认证信息
            opts.username=opts.id;
            opts.password=opts.id;

        }

        while(true)
        {
            if(connect())
            {
                main_debug_print("MQTT Hello:start connect!\r\n");
                break;
            }
            else
            {
                main_debug_print("MQTT Hello:start connect failed!\r\n");
                disconnect();
            }
            main_task_sleep(3000);
        }
    }
    //MQTT连接,一般在此处订阅主题，发布上线消息
    virtual void on_connect() override
    {
        main_debug_print("MQTT Hello:connect!\r\n");
        //订阅
        subscribe((get_connect_opts().id+"/#"),0);
        //发送上线消息
        publish(get_connect_opts().will_topic,"Online");
    }
    //MQTT连接，当is_close为真时可尝试手动重连
    virtual void on_disconnect(bool is_close) override
    {
        main_debug_print("MQTT Hello:%s!\r\n",is_close?"disconnect":"close");
        while(is_close)
        {
            if(connect())
            {
                main_debug_print("MQTT Hello:start reconnect!\r\n");
                break;
            }
            else
            {
                main_debug_print("MQTT Hello:start reconnect failed!\r\n");
                disconnect();
            }
            main_task_sleep(3000);
        }
    }

    //MQTT数据，接收到的数据在此处处理
    virtual void on_data(std::string topic,std::string payload) override
    {
        main_debug_print("MQTT Hello:%s(%d)!\r\n",topic.c_str(),(int)payload.length());
        //修改主题后原样回复
        size_t pos = 0;
        if ((pos = topic.find("/")) != std::string::npos)
        {
            std::string srcaddr =    topic.substr(pos + 1);

            if (srcaddr.empty())
            {
                return;
            }
            topic = (srcaddr + "/" +get_connect_opts().id);
        }
        publish(topic,payload);
    }

};
static mqtthello g_mqtthello;

#endif // CONFIG_MQTT_STACK_HELLO
