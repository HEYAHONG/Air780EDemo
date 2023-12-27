#ifndef __HTTPCLIENT_H__
#define __HTTPCLIENT_H__

#include "main.h"

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus
struct network_ctrl_t;
#include "luat_http.h"

#ifdef __cplusplus
}
#endif // __cplusplus


#ifdef __cplusplus
#include <functional>
#include <string>

class httpclient
{
    std::function<void (int, void *, uint32_t)> http_cb;
    luat_http_ctrl_t * client;
    bool pause(uint8_t is_pause=1)
    {
        if(client!=NULL)
        {
            if(luat_http_client_pause(client,is_pause)==0)
            {
                return true;
            }
        }
        return false;
    }

    std::function<void(int)> onerror;//出错
    std::function<void (void *,uint32_t)> onbody;//将被多次调用，每次输入部分数据，当数据为空时结束
    std::function<void (void *,uint32_t)> onhead;//将被多次调用，每次输入部分数据，当数据为空时结束
    std::function<std::string ()> post;//多次调用，可每次返回部分数据，返回空结束

    static void luat_http_callback(int status, void *data, uint32_t data_len, void *user_param);
    friend luat_http_ctrl_t* luat_http_client_create(luat_http_cb cb, void *user_param, int adapter_index);
    void http_callback(int status, void *data, uint32_t data_len)
    {
        if(http_cb!=NULL)
        {
            http_cb(status,data,data_len);
        }
        if(status < 0 )
        {
            //http出错
            if(onerror!=NULL)
            {
                onerror(status);
            }
        }

        switch(status)
        {
        case HTTP_STATE_GET_BODY:
        {
            if(onbody!=NULL)
            {
                onbody(data,data_len);
            }
        }
        break;
        case HTTP_STATE_GET_HEAD:
        {
            if(onhead!=NULL)
            {
                onhead(data,data_len);
            }
        }
        break;
        case HTTP_STATE_SEND_BODY_START:
        {
            if(post!=NULL)
            {
                std::string data=post();
                if(!data.empty())
                {
                    luat_http_client_post_body(client,(void *)data.c_str(),data.length());
                }
            }
        }
        break;
        case HTTP_STATE_SEND_BODY:
        {
            if(post!=NULL)
            {
                std::string data=post();
                if(!data.empty())
                {
                    luat_http_client_post_body(client,(void *)data.c_str(),data.length());
                }
            }
        }
        break;
        default:
            break;
        }

    }
public:
    void create()
    {
        if(client==NULL)
        {
            client=luat_http_client_create(luat_http_callback,this,-1);
        }
    }

    bool base(uint32_t timeout=5000, uint8_t debug_onoff=0, uint8_t retry_cnt=3)
    {
        create();
        if(client!=NULL)
        {
            if(luat_http_client_base_config(client,timeout,debug_onoff,retry_cnt)==0)
            {
                return true;
            }
        }
        return false;
    }
    bool ssl(std::string ca_cert,std::string client_cert=std::string(),std::string client_key=std::string(),std::string client_key_password=std::string(),int mode=0)
    {
        create();
        if(client!=NULL)
        {
            if(luat_http_client_ssl_config(client,mode,ca_cert.c_str(),ca_cert.length(),client_cert.c_str(),client_cert.length(),client_key.c_str(),client_key.length(),client_key_password.c_str(),client_key_password.length())==0)
            {
                return true;
            }
        }
        return false;
    }
    bool clear()
    {
        if(client!=NULL)
        {
            if(luat_http_client_clear(client)==0)
            {
                return true;
            }
        }
        return false;
    }
    bool set_user_head(const char *name,const char *value)
    {
        if(client!=NULL)
        {
            if(luat_http_client_set_user_head(client,name,value)==0)
            {
                return true;
            }
        }
        return false;
    }
    void set_callback(std::function<void(int)> _onerror,std::function<void (void *,uint32_t)> _onbody,std::function<void (void *,uint32_t)> _onhead=NULL,std::function<std::string ()> _post=NULL)
    {
        onerror=_onerror;
        onbody=_onbody;
        onhead=_onhead;
        post=_post;
    }
    bool start(const char *url,uint8_t is_post=0,uint8_t ipv6=0,uint8_t continue_mode=1)
    {
        if(client!=NULL)
        {
            if(luat_http_client_start(client,url,is_post,ipv6,continue_mode)==0)
            {
                return true;
            }
        }
        return false;
    }
    bool stop()
    {
        if(client!=NULL)
        {
            if(luat_http_client_close(client)==0)
            {
                return true;
            }
        }
        return false;
    }
    int get_status_code()
    {
        if(client!=NULL)
        {
            return luat_http_client_get_status_code(client);
        }
        return -1;
    }
    void destroy()
    {
        if(client!=NULL)
        {
            luat_http_client_destroy(&client);
            client=NULL;
        }
    }
    httpclient(std::function<void (int, void *, uint32_t)> _http_cb=NULL):http_cb(_http_cb),client(NULL),onerror(NULL),onbody(NULL),onhead(NULL),post(NULL)
    {
        create();
    }
    httpclient(const httpclient &oths)=delete;
    const httpclient &operator =(const httpclient &oths)=delete;
    virtual ~httpclient()
    {
        destroy();
    }
};





/** \brief 简易HTTP Get
 *
 * \param url const char* URL
 * \param ca std::string CA证书
 * \param client std::string 客户端证书
 * \param key std::string 客户端私钥
 * \param key_pass std::string 客户端私钥密码
 * \return std::string Get获取数据
 *
 */
std::string http_get(const char *url,std::string ca=std::string(),std::string client=std::string(),std::string key=std::string(),std::string key_pass=std::string());

/** \brief 简易HTTP Post
 *
 * \param url const char* URL
 * \param post std::string Post数据
 * \param ca std::string CA证书
 * \param client std::string 客户端证书
 * \param key std::string 客户端私钥
 * \param key_pass std::string 客户端私钥密码
 * \return std::string Get获取数据
 *
 */
std::string http_post(const char *url,std::string post,std::string ca=std::string(),std::string client=std::string(),std::string key=std::string(),std::string key_pass=std::string());

#endif // __cplusplus


#endif // __HTTPCLIENT_H__
