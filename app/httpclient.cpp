#include "httpclient.h"

void httpclient::luat_http_callback(int status, void *data, uint32_t data_len, void *user_param)
{
    if(user_param==NULL)
    {
        return;
    }
    httpclient *client=(httpclient *)user_param;
    client->http_callback(status,data,data_len);
}


std::string http_get(const char *url,std::string ca_crt,std::string client_crt,std::string key,std::string key_pass)
{
    return http_post(url,std::string(),ca_crt,client_crt,key,key_pass);
}


std::string http_post(const char *url,std::string post,std::string ca_crt,std::string client_crt,std::string key,std::string key_pass)
{
    std::string data;
    if(url==NULL || strlen(url) < 5)
    {
        return data;
    }
    httpclient client;
    if(url[4]!='s' && url[4]!='S')
    {
        //http
        if(!client.base())
        {
            //由于库函数错误（未返回0），此处不处理
            //return data;
        }
    }
    else
    {
        //https
        if(!client.ssl(ca_crt,client_crt,key,key_pass))
        {
            return data;
        }
    }
    bool is_end=false;
    client.set_callback(
        [&](int status)
    {
        (void)status;
        is_end=true;
    },
    [&](void *_data,uint32_t _data_len)
    {
        if(_data!=NULL && _data_len != 0)
        {
            data+=std::string((char *)_data,_data_len);
        }
        else
        {
            is_end=true;
        }
    },
    [&](void *_data,uint32_t _data_len)
    {
        (void)_data;
        (void)_data_len;
    },
    [&]()->std::string
    {
        //每次发送256字节
        std::string ret=post.substr(0,256);
        post=post.substr(256);
        return ret;
    });
    if(!client.start(url,post.empty()?0:1))
    {
        return data;
    }
    while(!is_end)
    {
        luat_rtos_task_sleep(1000);
    }
    return data;
}
