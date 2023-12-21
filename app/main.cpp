#include "main.h"
#include "RC.h"
#include "time.h"
#include <string>
#include <json/value.h>
#include <json/reader.h>
#include <json/writer.h>

static void displaymeminfo()
{
    size_t total=0,used=0,max_used=0;
    main_meminfo(&total,&used,&max_used);
    main_debug_print("memory total: %" PRIu32  " bytes,max_used: %" PRIu32 " bytes,free: %" PRIu32 " bytes",total,max_used,total-used);
}

static char * global=new char[30];

void main_task(void *param)
{
    (void)param;
    main_add_event(NULL,[](void *,heventloop_t*)
    {
        main_debug_print("main start!");

        {
            //打印banner
            char * banner=(char *)RCGetHandle("banner");
            if(banner!=NULL)
                main_debug_print("\r\n%s\r\n",banner);
        }

        //打印IMEI
        main_debug_print("IMEI:%s",main_get_imei());

        {
            //打印文件系统信息
            luat_fs_info_t info= {"",0,0,0,0};
            if(luat_fs_info("/",&info)==0)
            {
                main_debug_print("fsinfo:filesystem= %s ,type= %d ,total= %d bytes,free= %d bytes,block= %d bytes",info.filesystem,(int)info.type,(int)(info.total_block*info.block_size),(int)((info.total_block-info.block_used)*info.block_size),(int)info.block_size);
            }
        }

        {
            /*
            加载资源文件中的app.json,保存一些常用信息
            */
            std::string app_json;
            {
                //加载RC中的资源文件
                const char * temp=(const char *)RCGetHandle("app.json");
                if(temp!=NULL)
                {
                    app_json=temp;
                }
            }

            Json::Reader reader;
            Json::Value json;
            if(reader.parse(app_json,json))
            {
                {
                    //将imei加入json
                    json["imei"]=main_get_imei();
                }
                Json::StyledWriter writer;
                std::string payload=writer.write(json);
                main_debug_print("app.json config\n%s",payload.c_str());
            }
        }

        displaymeminfo();
    },NULL);

    uint32_t main_loop_print_tick_ms=main_task_gettick_ms();
    while(true)
    {

        {
            //处理事件循环
            heventloop_process_event(main_get_eventloop());
        }

        {
            //处理槽函数，参数为空
            heventslots_emit_signal(main_get_mainloop_slot(),NULL);
        }

        if(main_task_gettick_ms()-main_loop_print_tick_ms > 60000)
        {
            main_loop_print_tick_ms=main_task_gettick_ms();
            time_t current_time=(uint32_t)time(NULL);
            char timestr[50]= {0};
            {
                const char * _timestr=asctime(localtime(&current_time));
                memcpy(timestr,_timestr,strlen(_timestr)-1);
            }
            main_debug_print("main loop,tick=%" PRIu32 ",time=%s!",main_task_gettick_ms(),timestr);
            displaymeminfo();
        }

        main_task_sleep(1);
    }
}

/*
  重载相关函数
*/
void *operator new(size_t size)
{
    return main_malloc(size);
}

void *operator new[](size_t size)
{
    return main_malloc(size);
}

void operator delete(void *ptr)
{
    main_free(ptr);
}

void operator delete[](void *ptr)
{
    return main_free(ptr);
}

void __cxa_pure_virtual(void)
{
    main_debug_print("Illegal to call a pure virtual function.\n");
}
