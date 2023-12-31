#ifndef __KCONFIG_H_INCLUDED__
#define __KCONFIG_H_INCLUDED__

#ifdef USE_KCONFIG

#include "config.h"

#else
/*
默认参数(无Kconfig时的默认参数)
*/

#ifndef CONFIG_APP_NAME
#define CONFIG_APP_NAME "Air780E"
#endif // CONFIG_APP_NAME

#ifndef CONFIG_UART_APP_DEBUG
#define CONFIG_UART_APP_DEBUG 1
#endif // CONFIG_UART_APP_DEBUG

#ifndef CONFIG_WATCHDOG_TIMEOUT
#define CONFIG_WATCHDOG_TIMEOUT 5
#endif // CONFIG_WATCHDOG_TIMEOUT

#ifndef CONFIG_MOBILE_WATCHDOG_TIMEOUT
#define CONFIG_MOBILE_WATCHDOG_TIMEOUT 120
#endif // CONFIG_MOBILE_WATCHDOG_TIMEOUT

//#define CONFIG_MOBILE_INTERNET_DETECT 1

#if CONFIG_MOBILE_INTERNET_DETECT

#ifndef CONFIG_MOBILE_INTERNET_DETECT_URL
#define CONFIG_MOBILE_INTERNET_DETECT_URL "http://example.com/index.html"
#endif // CONFIG_MOBILE_INTERNET_DETECT_URL

#endif // CONFIG_MOBILE_INTERNET_DETECT

#ifndef CONFIG_MQTT_WATCHDOG_TIMEOUT
#define CONFIG_MQTT_WATCHDOG_TIMEOUT 300
#endif // CONFIG_MQTT_WATCHDOG_TIMEOUT

//#define CONFIG_MQTT_STACK_HELLO 1

#if CONFIG_MQTT_STACK_HELLO

#ifndef CONFIG_MQTT_STACK_HELLO_HOST
#define CONFIG_MQTT_STACK_HELLO_HOST "lbsmqtt.airm2m.com"
#endif // CONFIG_MQTT_STACK_HELLO_HOST

#ifndef CONFIG_MQTT_STACK_HELLO_PORT
#define CONFIG_MQTT_STACK_HELLO_PORT 1884
#endif // CONFIG_MQTT_STACK_HELLO_PORT

#endif // CONFIG_MQTT_STACK_HELLO

#endif // USE_KCONFIG

#endif // __KCONFIG_H_INCLUDED__
