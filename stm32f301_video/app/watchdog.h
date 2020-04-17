
/** @brief    看门狗功能模块
 *  @file     watchdog.h
 *  @author   yjsstk@163.com
 *  @version  v1.0
 *  @date     2020/04/17
 */
 
#ifndef _WATCHDOG_H_
#define _WATCHDOG_H_

#include "config.h"

#if (CONFIG_WATCHDOG_EN == 1)

/** @brief   喂狗
 *  @param   无 
 *  @return  无
 *  @note    
 */
extern inline void watchdog_feed(void);

/** @brief   该模块的应用初始化函数 
 *  @param   无 
 *  @return  返回值 @see CONFIG_RESULT_T
 *  @note    使用该模块的其他功能前调用以使初始化模块
 */
extern CONFIG_RESULT_T watchdog_init(void);

#endif /* #if (CONFIG_WATCHDOG_EN == 1) */

#endif
