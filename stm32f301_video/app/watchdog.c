
/** @brief    看门狗功能模块
 *  @file     watchdog.c
 *  @author   yjsstk@163.com
 *  @version  v1.0
 *  @date     2020/04/17
 */
 
#include "watchdog.h"
 
#if (CONFIG_WATCHDOG_EN == 1)

/** @brief   喂狗
 *  @param   无 
 *  @return  无
 *  @note    
 */
inline void watchdog_feed(void)
{
}

/** @brief   该模块的应用初始化函数 
 *  @param   无 
 *  @return  返回值 @see CONFIG_RESULT_T
 *  @note    使用该模块的其他功能前调用以使初始化模块
 */
CONFIG_RESULT_T watchdog_init(void)
{
	return RESULT_SUCCESS;
}


#endif /* #if (CONFIG_WATCHDOG_EN == 1) */ 
