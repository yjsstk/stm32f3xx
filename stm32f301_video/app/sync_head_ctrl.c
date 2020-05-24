
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
#include <string.h>
#include <stdbool.h>
#include "sync_head_ctrl.h"
#include "systick.h"
#include "pwm_ctrl.h"
#include "pin22_ctrl.h"
#include "rssi_signal_ctrl.h"
#include "app_scheduler.h"
#include "tim.h"

typedef struct
{
	bool     stop_output;
	uint16_t pulse_count;      // 脉冲计数
	uint16_t field_count;      // 
	uint16_t max_pulse;        //
	uint16_t keep_output_ms;
	sync_field_type_t field_type;
	sync_video_type_t video_type;
}sync_ctrl_t;

static sync_ctrl_t sync_pal_ctrl=
{
	.stop_output  = false,
	.pulse_count  = 0,
	.field_count  = 0,
	.max_pulse    = SYNC_PAL_ROW_PULSE,
	.field_type   = SYNC_FIELD_ODD,
	.video_type   = SYNC_VIDEO_PAL,
};
static sync_ctrl_t *psync_ctrl = &sync_pal_ctrl;

/** @brief   PWM事件处理
 *  @param   parg[in] 
 *  @param   arg_size[in]
 *  @return  无
 *  @note    
 */
static void sync_pwm_event_handle(void *parg, uint16_t arg_size)
{
	#define FIELD_PULSE_NUMBER   (SYNC_SLOT_PULSE * 3 / 2) 
	psync_ctrl->pulse_count++;
	if (psync_ctrl->pulse_count == psync_ctrl->max_pulse - FIELD_PULSE_NUMBER)
	{
		if (psync_ctrl->stop_output == false)
		{
			pwm_set_output(SYNC_ROW_CYCLE_NS / 2, SYNC_ROW_PULSE_NS / 2, SYNC_PIN19_PULSE_NS, SYNC_ROW_PULSE_NS / 2);
		}
		else
		{
			pwm_set_output(SYNC_ROW_CYCLE_NS / 2, 0, 0, SYNC_ROW_PULSE_NS / 2);
		}
		psync_ctrl->field_count = 0;
	}
	else if (psync_ctrl->pulse_count > psync_ctrl->max_pulse - FIELD_PULSE_NUMBER)
	{
		psync_ctrl->field_count++;
		if (psync_ctrl->field_count == SYNC_SLOT_PULSE)
		{
			uint32_t pin18_pulse_ns = SYNC_ROW_CYCLE_NS / 2 - SYNC_ROW_PULSE_NS;
			uint32_t pin19_pulse_ns = SYNC_ROW_CYCLE_NS / 2 - 1750;
			
			if (psync_ctrl->stop_output == false)
			{
				pwm_set_output(SYNC_ROW_CYCLE_NS / 2, pin18_pulse_ns, pin19_pulse_ns, pin18_pulse_ns);
			}
			else
			{
				pwm_set_output(SYNC_ROW_CYCLE_NS / 2, 0, 0, pin18_pulse_ns);
			}
		}
		else if (psync_ctrl->field_count == SYNC_SLOT_PULSE * 2)
		{
			if (psync_ctrl->stop_output == false)
			{
				pwm_set_output(SYNC_ROW_CYCLE_NS / 2, SYNC_ROW_PULSE_NS / 2, SYNC_PIN19_PULSE_NS, SYNC_ROW_PULSE_NS / 2);
			}
			else
			{
				pwm_set_output(SYNC_ROW_CYCLE_NS / 2, 0, 0, SYNC_ROW_PULSE_NS / 2);
			}
		}
		else if (psync_ctrl->field_type == SYNC_FIELD_EVEN)
		{
			if (psync_ctrl->field_count >= SYNC_SLOT_PULSE * 3 + 1)
			{
				psync_ctrl->pulse_count = 0;
				psync_ctrl->field_type = SYNC_FIELD_ODD;
				pwm_set_auto_reload_preload(false);
				if (psync_ctrl->stop_output == false)
				{
					pwm_set_output(SYNC_ROW_CYCLE_NS, SYNC_ROW_PULSE_NS, SYNC_PIN19_PULSE_NS, SYNC_ROW_PULSE_NS);
				}
				else
				{
					pwm_set_output(SYNC_ROW_CYCLE_NS, 0, 0, SYNC_ROW_PULSE_NS);
				}
				if (psync_ctrl->video_type == SYNC_VIDEO_PAL)
				{
					psync_ctrl->max_pulse = SYNC_PAL_ROW_PULSE;
				}
				else if (psync_ctrl->video_type == SYNC_VIDEO_NTSC)
				{
					psync_ctrl->max_pulse = SYNC_NTSC_ROW_PULSE;
				}
			}
		}
		else 
		{
			if (psync_ctrl->field_count >= SYNC_SLOT_PULSE * 3 - 1)
			{
				psync_ctrl->pulse_count = 0;
				psync_ctrl->field_type = SYNC_FIELD_EVEN;
				pwm_set_auto_reload_preload(true);
				if (psync_ctrl->stop_output == false)
				{
					pwm_set_output(SYNC_ROW_CYCLE_NS, SYNC_ROW_PULSE_NS, SYNC_PIN19_PULSE_NS, SYNC_ROW_PULSE_NS);
				}
				else
				{
					pwm_set_output(SYNC_ROW_CYCLE_NS, 0, 0, SYNC_ROW_PULSE_NS);
				}
				
				if (psync_ctrl->video_type == SYNC_VIDEO_PAL)
				{
					psync_ctrl->max_pulse = SYNC_PAL_ROW_PULSE+1;
				}
				else if (psync_ctrl->video_type == SYNC_VIDEO_NTSC)
				{
					psync_ctrl->max_pulse = SYNC_NTSC_ROW_PULSE+1;
				}
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
	sync_pwm_event_handle(NULL, 0);
//	app_scheduler_put(sync_pwm_event_handle, NULL, 0);
}

/** @brief   检测到同步头信号回调
 *  @param   video_type[in] 视频类型
 *  @param   field_type[in] 奇偶场
 *  @return  无
 *  @note    
 */
static void sync_event_ctrl(sync_video_type_t video_type, sync_field_type_t field_type)
{
	if (video_type == SYNC_VIDEO_UNKNOWN || field_type == SYNC_FIELD_UNKNOWN)
	{
		DEBUG_INFO("sync detect err");
		return;
	}
	sync_ctrl_t sync_ctrl;
	
	sync_ctrl.stop_output = false;
	sync_ctrl.video_type  = video_type;
	sync_ctrl.field_type  = field_type;
	sync_ctrl.field_count = 0;
	sync_ctrl.pulse_count = 0;
	
	if (video_type == SYNC_VIDEO_PAL)
	{
		sync_ctrl.max_pulse = SYNC_PAL_ROW_PULSE;
		if (field_type == SYNC_FIELD_ODD)
		{
			sync_ctrl.max_pulse = SYNC_PAL_ROW_PULSE;
		}
		else if (field_type == SYNC_FIELD_EVEN)
		{
			sync_ctrl.max_pulse = SYNC_PAL_ROW_PULSE + 1;
		}
	}
	else if (video_type == SYNC_VIDEO_NTSC)
	{
		sync_ctrl.max_pulse = SYNC_NTSC_ROW_PULSE;
		if (field_type == SYNC_FIELD_ODD)
		{
			sync_ctrl.max_pulse = SYNC_NTSC_ROW_PULSE;
		}
		else if (field_type == SYNC_FIELD_EVEN)
		{
			sync_ctrl.max_pulse = SYNC_NTSC_ROW_PULSE + 1;
		}
	}
	
	__set_PRIMASK(1);
	memcpy(psync_ctrl, &sync_ctrl, sizeof(sync_ctrl));
	__set_PRIMASK(0);
	
	pwm_set_auto_reload_preload(false);
	if (psync_ctrl->stop_output == false)
	{
		pwm_set_output(SYNC_ROW_CYCLE_NS, 
		               SYNC_ROW_PULSE_NS, 
		               SYNC_PIN19_PULSE_NS, 
		               SYNC_ROW_PULSE_NS);
	}
	else
	{
		pwm_set_output(SYNC_ROW_CYCLE_NS, 0, 0, SYNC_ROW_PULSE_NS);
	}
	pwm_set_time_cnt_val(0);
}

/** @brief   1MS回调函数
 *  @param   pcontent[in] 
 *  @return  无
 *  @note    
 */
static void sync_tim_detect_cb(filed_cycle_t filed_cycle)
{
//	DEBUG_INFO("%d", psync_ctrl->pulse_count);
	if (filed_cycle == FILED_CYCLE_50HZ_ODD)
	{
		sync_event_ctrl(SYNC_VIDEO_PAL, SYNC_FIELD_ODD);
	}
	else if (filed_cycle == FILED_CYCLE_50HZ_EVEN)
	{
		sync_event_ctrl(SYNC_VIDEO_PAL, SYNC_FIELD_EVEN);
	}
	else if (filed_cycle == FILED_CYCLE_60HZ_ODD)
	{
		sync_event_ctrl(SYNC_VIDEO_NTSC, SYNC_FIELD_ODD);
	}
	else if (filed_cycle == FILED_CYCLE_60HZ_EVEN)
	{
		sync_event_ctrl(SYNC_VIDEO_NTSC, SYNC_FIELD_EVEN);
	}
//	if (filed_cycle != FILED_CYCLE_UNKNOWN)
//	{
//		HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_4);
//	}
}

/** @brief   1MS回调函数
 *  @param   pcontent[in] 
 *  @return  无
 *  @note    
 */
static void sync_1ms_callback(void *pcontent)
{
	psync_ctrl->keep_output_ms++;
	if (rssi_get_is_both_weak(RSSI_MAX_LIMIT_VCC) == false)
	{
		psync_ctrl->stop_output = false;
		psync_ctrl->keep_output_ms = 0;
		return;
	}
	
	// 停止同步头信号输出
	if ((psync_ctrl->keep_output_ms == SYNC_KEEP_OUTPUT_MS) && 
		(psync_ctrl->stop_output == false))
	{
		psync_ctrl->stop_output = true;
		pwm_pin18_pin19_stop();
//		pin22_output_ctrl(GPIO_PIN_SET);
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
	tim_field_cycle_capture_evt_reg(sync_tim_detect_cb);
	systick_1ms_cb_reg(sync_1ms_callback);
	
	return RESULT_SUCCESS;
}
