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

//#define CONFIG_MOBILE_INTERNET_DETECT 1


#endif // USE_KCONFIG

#endif // __KCONFIG_H_INCLUDED__
