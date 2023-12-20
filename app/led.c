#include "led.h"
#include "luat_rtos.h"
#include "luat_gpio.h"
#include "platform_define.h"


static uint64_t led_table=0x5555555555555555;

uint64_t led_get_table()
{
    return led_table;
}

void led_set_table(uint64_t table)
{
    led_table=table;
}

#ifdef LED_PIN

static uint8_t led_index=0;
static luat_rtos_timer_t led_timer=NULL;

static void led_time_callback(void *param)
{
    (void)param;
    luat_gpio_set(LED_PIN,(led_table&(1ULL << led_index))!=0?1:0);
    led_index++;
    if(led_index > 63)
    {
        led_index=0;
    }
}

void led_init(void)
{
    luat_gpio_cfg_t gpio_cfg;
    luat_gpio_set_default_cfg(&gpio_cfg);
    gpio_cfg.pin = LED_PIN;
    luat_gpio_open(&gpio_cfg);

    luat_rtos_timer_create(&led_timer);
    if(led_timer!=NULL)
    {
        luat_rtos_timer_start(led_timer,100,1,led_time_callback,NULL);
    }
}

#endif // LED_PIN


