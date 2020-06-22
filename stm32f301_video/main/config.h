
/** @brief    项目配置文件
 *  @file     config.h
 *  @author   yjsstk@163.com
 *  @version  v1.0
 *  @date     2020/04/16
 */
 
#ifndef _CONFIG_H_
#define _CONFIG_H_

#define CONFIG_PROJECT_NAME          "VIDEO"    // 项目名称
#define CONFIG_VERSION               1          // 版本号

#define CONFIG_APP_SCHEDULER_EVENT        20 
#define CONFIG_APP_SCHEDULER_EVENT_SIZE   20 

#define CONFIG_DEBUG_EN              0          // DEBUG调试
#define CONFIG_WATCHDOG_EN           0          // 看门狗功能
 
typedef enum
{
	RESULT_SUCCESS,
	RESULT_ERROR,
	RESULT_NULL,
	RESULT_FULL,
	RESULT_EMPTY,
	RESULT_INVALID_SIZE,
}CONFIG_RESULT_T;

#ifndef NULL
#define NULL  0
#endif

#ifndef TRUE
#define TRUE  1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#endif
