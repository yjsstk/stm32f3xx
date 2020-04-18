
/** @brief    同步头信号处理
 *  @file     sync_head_ctrl.c
 *  @author   yjsstk@163.com
 *  @version  v1.0
 *  @date     2020/04/17
 *  @note     跟据检测的PIN14引脚信息来控制视频同步头输出
 */

#if (CONFIG_DEBUG_EN == 1)
#define DEBUG_INFO_EN       1
#define DEBUG_MODULE_NAME   "sync_head"
#endif
#include "debug.h"

#include <stdint.h>
#include "sync_head_ctrl.h"
#include "systick.h"

static uint16_t sync_keep_output_ms;

/** @brief   检测到同步头信号回调
 *  @param   pcontent[in] 
 *  @return  无
 *  @note    
 */
static void sync_detect_callbacks(void *pcontent)
{
}

/** @brief   1MS回调函数
 *  @param   pcontent[in] 
 *  @return  无
 *  @note    
 */
static void sync_1ms_callback(void *pcontent)
{
	if (sync_keep_output_ms < UINT16_MAX)
	{
		sync_keep_output_ms++;
	}
	
	// 停止同步头信号输出
	if (sync_keep_output_ms == SYNC_KEEP_OUTPUT_MS)
	{
	}
}
 
/** @brief   该模块的应用初始化函数 
 *  @param   无 
 *  @return  返回值 @see CONFIG_RESULT_T
 *  @note    使用该模块的其他功能前调用以使初始化模块
 */
CONFIG_RESULT_T sync_head_ctrl_init(void)
{
	systick_1ms_func_reg(sync_1ms_callback);
	
	return RESULT_SUCCESS;
}
