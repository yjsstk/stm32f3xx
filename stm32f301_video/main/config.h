
/** @brief    项目配置文件
 *  @file     config.h
 *  @author   yjsstk@163.com
 *  @version  v1.0
 *  @date     2020/04/16
 */
 
#ifndef _CONFIG_H_
#define _CONFIG_H_
 
#include <stdint.h> 
 
#define CONFIG_VERSION            1     // 版本号

#define CONFIG_DEBUG_EN           0     // DEBUG调试
 

#ifndef NULL
#define NULL 0
#endif
 
typedef enum
{
	RESULT_SUCCESS,
	RESULT_NULL,
}CONFIG_RESULT_T;

#endif
