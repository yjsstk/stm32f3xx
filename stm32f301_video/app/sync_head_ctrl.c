
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
#include <stdbool.h>
#include "sync_head_ctrl.h"
#include "systick.h"
#include "pwm_ctrl.h"

static uint16_t sync_keep_output_ms;

typedef struct
{
	bool     field_even;       // 偶数场标志
	uint16_t pulse_count;      // 脉冲计数
	uint16_t max_row_pulse;    // 行脉冲总数
	uint16_t max_solt_pulse;   // 槽脉冲个数
}sync_ctrl_t;

static sync_ctrl_t sync_pal_ctrl=
{
	.field_even     = false,
	.pulse_count    = 0,
	.max_row_pulse  = SYNC_PAL_FIELD_ROW,
	.max_solt_pulse = SYNC_PAL_SLOT_PULSE,
};

static sync_ctrl_t sync_ntsc_ctrl=
{
	.field_even     = false,
	.pulse_count    = 0,
	.max_row_pulse  = SYNC_NTSC_FIELD_ROW,
	.max_solt_pulse = SYNC_NTSC_SLOT_PULSE,
};

static sync_ctrl_t *psync_ctrl = &sync_ntsc_ctrl;

/** @brief   PWM中断回调函数
 *  @param   pcontent[in] 
 *  @return  无
 *  @note    
 */
static void sync_pwm_interrupt_callback(void *pcontent)
{
}

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
	pwm_interrupt_cb_reg(sync_pwm_interrupt_callback);
	systick_1ms_cb_reg(sync_1ms_callback);
	
	return RESULT_SUCCESS;
}
