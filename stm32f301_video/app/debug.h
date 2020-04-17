
/** @brief    DEBUG串口调试功能
 *  @file     debug.h
 *  @author   yjsstk@163.com
 *  @version  v1.0
 *  @date     2020/04/17
 *  @note     使用串口输出做DEBUG调试功能
 */

#ifndef _DEBUG_H_
#define _DEBUG_H_

#include "config.h"
#include <stdio.h>

#if ((CONFIG_DEBUG_EN == 1) && (DEBUG_INFO_EN == 1))
	#define DEBUG_INFO(format, ...)     printf("[%s][line=%d]:"format"\r\n", DEBUG_MODULE_NAME, __LINE__, ##__VA_ARGS__)
	#define DEBUG_PRINTF(format, ...)   printf(format, ##__VA_ARGS__)
#else
	#define DEBUG_INFO(format, ...)
	#define DEBUG_PRINTF(format, ...)
#endif

#if (CONFIG_DEBUG_EN == 1)
/** @brief   该模块的应用初始化函数 
 *  @param   无 
 *  @return  返回值 @see CONFIG_RESULT_T
 *  @note    使用该模块的其他功能前调用以使初始化模块
 */
extern CONFIG_RESULT_T debug_init(void);
#endif 

#endif


