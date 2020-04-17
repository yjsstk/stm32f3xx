
/** @brief    DEBUG串口调试功能
 *  @file     debug.c
 *  @author   yjsstk@163.com
 *  @version  v1.0
 *  @date     2020/04/17
 *  @note     使用串口输出做DEBUG调试功能
 */

#include "debug.h"

#if (CONFIG_DEBUG_EN == 1)

/** @brief   该模块的应用初始化函数 
 *  @param   无 
 *  @return  返回值 @see CONFIG_RESULT_T
 *  @note    使用该模块的其他功能前调用以使初始化模块
 */
CONFIG_RESULT_T debug_init(void)
{
	return RESULT_SUCCESS;
}


#endif /* #if (CONFIG_DEBUG_EN == 1) */





