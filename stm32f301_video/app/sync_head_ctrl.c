
/** @brief    同步头信号处理
 *  @file     sync_head_ctrl.c
 *  @author   yjsstk@163.com
 *  @version  v1.0
 *  @date     2020/04/17
 *  @note     跟据检测的PIN14引脚信息来控制视频同步头输出
 */

#include "config.h"
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
#include "pin22_ctrl.h"
#include "rssi_signal_ctrl.h"
#include "app_scheduler.h"

static uint16_t sync_keep_output_ms;

typedef struct
{
	bool     pin18_19_stop;
	bool     field_even;       // 偶数场标志
	uint8_t  slot_step;
	uint16_t pulse_count;      // 脉冲计数
	uint16_t field_count;
	uint16_t total_row_pulse;  //
	uint16_t total_field_pulse;  //
}sync_ctrl_t;

static sync_ctrl_t sync_pal_ctrl=
{
	.pin18_19_stop    = false,
	.field_even       = false,
	.slot_step        = 0,
	.pulse_count      = 0,
	.field_count      = 0,
	.total_row_pulse  = SYNC_PAL_ROW_PULSE,
	.total_field_pulse= SYNC_PAL_FIELD_PULSE,
};

static sync_ctrl_t sync_ntsc_ctrl=
{
	.pin18_19_stop    = false,
	.field_even       = false,
	.slot_step        = 0,
	.pulse_count      = 0,
	.field_count      = 0,
	.total_row_pulse  = SYNC_NTSC_ROW_PULSE,
	.total_field_pulse= SYNC_NTSC_FIELD_PULSE,
};

static sync_ctrl_t *psync_ctrl = &sync_ntsc_ctrl;

/** @brief   PWM事件处理
 *  @param   parg[in] 
 *  @param   arg_size[in]
 *  @return  无
 *  @note    
 */
static void sync_pwm_event_handle(void *parg, uint16_t arg_size)
{
	psync_ctrl->pulse_count++;
	if (psync_ctrl->pulse_count == psync_ctrl->total_row_pulse)
	{
		if (psync_ctrl->pin18_19_stop == false)
		{
			pwm_set_output(SYNC_ROW_CYCLE_NS / 2, SYNC_ROW_PULSE_NS / 2, SYNC_PIN19_PULSE_NS, SYNC_ROW_PULSE_NS / 2);
		}
		else
		{
			pwm_set_output(SYNC_ROW_CYCLE_NS / 2, 0, 0, SYNC_ROW_PULSE_NS / 2);
		}
		psync_ctrl->field_count = 0;
		psync_ctrl->slot_step   = 0;
	}
	else if (psync_ctrl->pulse_count > psync_ctrl->total_row_pulse)
	{
		psync_ctrl->field_count++;
		if (psync_ctrl->field_count == SYNC_SLOT_PULSE && psync_ctrl->slot_step == 0)
		{
			uint32_t pin18_pulse_ns = SYNC_ROW_CYCLE_NS / 2 - SYNC_ROW_PULSE_NS;
			uint32_t pin19_pulse_ns = SYNC_ROW_CYCLE_NS / 2 - 1750;
			
			if (psync_ctrl->pin18_19_stop == false)
			{
				pwm_set_output(SYNC_ROW_CYCLE_NS / 2, pin18_pulse_ns, pin19_pulse_ns, pin18_pulse_ns);
			}
			else
			{
				pwm_set_output(SYNC_ROW_CYCLE_NS / 2, 0, 0, pin18_pulse_ns);
			}
			psync_ctrl->slot_step   = 1;
			psync_ctrl->field_count = 0;
		}
		else if (psync_ctrl->field_count == SYNC_SLOT_PULSE && psync_ctrl->slot_step == 1)
		{
			if (psync_ctrl->pin18_19_stop == false)
			{
				pwm_set_output(SYNC_ROW_CYCLE_NS / 2, SYNC_ROW_PULSE_NS / 2, SYNC_PIN19_PULSE_NS, SYNC_ROW_PULSE_NS / 2);
			}
			else
			{
				pwm_set_output(SYNC_ROW_CYCLE_NS / 2, 0, 0, SYNC_ROW_PULSE_NS / 2);
			}
			psync_ctrl->slot_step   = 2;
			psync_ctrl->field_count = 0;
		}
		else if (psync_ctrl->field_count == SYNC_SLOT_PULSE && psync_ctrl->slot_step == 2)
		{
			if (psync_ctrl->pin18_19_stop == false)
			{
				pwm_set_output(SYNC_ROW_CYCLE_NS, SYNC_ROW_PULSE_NS, SYNC_PIN19_PULSE_NS, SYNC_ROW_PULSE_NS);
			}
			else
			{
				pwm_set_output(SYNC_ROW_CYCLE_NS, 0, 0, SYNC_ROW_PULSE_NS);
			}
			psync_ctrl->slot_step   = 3;
			psync_ctrl->field_count = 0;
		}
		else if (psync_ctrl->slot_step == 3)
		{
			uint16_t last_pulse = psync_ctrl->total_field_pulse - 8;
			if (psync_ctrl->field_even == true)
			{
				last_pulse += 1;
			}
			if (psync_ctrl->field_count >= last_pulse)
			{
				psync_ctrl->field_even = !psync_ctrl->field_even;
				psync_ctrl->pulse_count = 0;
			}
		}
	}
}

/** @brief   PWM中断回调函数
 *  @param   pcontent[in] 
 *  @return  无
 *  @note    
 */
static void sync_pwm_interrupt_callback(void *pcontent)
{
	app_scheduler_put(sync_pwm_event_handle, NULL, 0);
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
	if (rssi_get_is_both_weak() == false)
	{
		sync_keep_output_ms = 0;
		return;
	}
	
	if (sync_keep_output_ms < UINT16_MAX)
	{
		sync_keep_output_ms++;
	}
	
	// 停止同步头信号输出
	if (sync_keep_output_ms == SYNC_KEEP_OUTPUT_MS)
	{
		psync_ctrl->pin18_19_stop = true;
		pwm_pin18_pin19_stop();
		pin22_output_ctrl(GPIO_PIN_SET);
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
