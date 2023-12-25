#include "main.h"
#include "common_api.h"
#include "plat_config.h"
#include "driver_uart.h"
#include "driver_gpio.h"
#include "printf.h"
#include "hringbuf.h"

#if CONFIG_UART_APP_DEBUG
/*
    本文件主要将uart0作为APP日志输出口,而不是作为C-SDK日志输出。
*/
#define LOG_BAUD 115200
static uint8_t Tx_Buff[2048]= {0};
static int32_t uart0_rx_callback(void *pData, void *pParam)
{
    uint32_t uartid = (uint32_t)pData;
    uint32_t State = (uint32_t)pParam;
    switch (State)
    {
    case UART_CB_TX_BUFFER_DONE:
    {

    }
    break;
    case UART_CB_TX_ALL_DONE:
    {

    }
    break;
    case UART_CB_RX_NEW:
    {
        Uart_RxBufferClear(uartid);//清理接收缓冲
    }
    break;
    case UART_CB_RX_TIMEOUT:
    {
        Uart_RxBufferClear(uartid);//清理接收缓冲
    }
    break;
    case UART_CB_RX_BUFFER_FULL:
    {
        Uart_RxBufferClear(uartid);//清理接收缓冲
    }
    break;
    case UART_CB_ERROR:
    {

    }
    break;
    case UART_CB_CONNECTED:
    {

    }
    break;
    default:

        break;
    }
    return 0;
}

/*
    设置波特率
*/
void soc_get_unilog_br(uint32_t *baudrate)
{
    *baudrate =  LOG_BAUD;
}

bool soc_init_unilog_uart(uint8_t port, uint32_t baudrate, bool startRecv)
{
    (void)startRecv;
    soc_get_unilog_br(&baudrate);
    GPIO_IomuxEC618(GPIO_ToPadEC618(HAL_GPIO_14, 0), 3, 0, 0);
    GPIO_IomuxEC618(GPIO_ToPadEC618(HAL_GPIO_15, 0), 3, 0, 0);
    GPIO_PullConfig(GPIO_ToPadEC618(HAL_GPIO_14, 0), 1, 1);
    //初始化串口
    Uart_BaseInitEx(port,  LOG_BAUD, 2048, 256, UART_DATA_BIT8, UART_PARITY_NONE, UART_STOP_BIT1, uart0_rx_callback);
    return true;
}


/*
在任务中处理的操作
*/
static void main_init_in_task(void*usr,heventloop_t*loop)
{

    (void)usr;
    (void)loop;
    {
        //设置log到usb输出
        BSP_SetPlatConfigItemValue(PLAT_CONFIG_ITEM_LOG_PORT_SEL,PLAT_CFG_ULG_PORT_USB);
        //保存设置
        BSP_SavePlatConfigToFs();
    }
}


void main_app_log_output(char character, void* arg)
{
    (void)character;
    (void)arg;
    hringbuf_t *buff=hringbuf_get(Tx_Buff,sizeof(Tx_Buff));
    hringbuf_input(buff,(const uint8_t *)&character,1);
}

static void log_slot(void *signal,void *usr)
{
    (void)signal;
    (void)usr;
    hringbuf_t *buff=hringbuf_get(Tx_Buff,sizeof(Tx_Buff));
    char buffer[2048]= {0};
    size_t len=hringbuf_output(buff,(uint8_t *)buffer,sizeof(buffer));
    if(len > 0)
    {
        Uart_TxTaskSafe(0,buffer,len);
    }
}

static void app_log_init()
{
    //将需要在任务中初始化的工作添加至主任务
    main_add_event(NULL,main_init_in_task,NULL);

    {
        extern void soc_uart0_set_log_off(uint8_t isoff);
        //关闭uart0_log
        soc_uart0_set_log_off(1);
    }

    soc_init_unilog_uart(0,LOG_BAUD,false);

    main_register_mainloop_slot(NULL,log_slot,NULL);

}



INIT_TASK_EXPORT(app_log_init,"1");
#else
void main_app_log_output(char character, void* arg)
{
    (void)character;
    (void)arg;
}
#endif // CONFIG_UART_APP_DEBUG
