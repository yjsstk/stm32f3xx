
/** @brief    视频输出信号由软件或硬件控制处理
 *  @file     pin30_ctrl.c
 *  @author   yjsstk@163.com
 *  @version  v1.0
 *  @date     2020/04/17
 *  @note     PIN30输出低，视频输出信息由硬件控制处理
 *  @note     PIN30输出高，视频输出信息由软件控制处理 
 *  @note     软件控制由PIN12控制
 */

#include "pin30_ctrl.h"


/** @brief   该模块的应用初始化函数 
 *  @param   无 
 *  @return  返回值 @see CONFIG_RESULT_T
 *  @note    使用该模块的其他功能前调用以使初始化模块
 */
CONFIG_RESULT_T pin30_ctrl_init(void)
{
	return RESULT_SUCCESS;
}



