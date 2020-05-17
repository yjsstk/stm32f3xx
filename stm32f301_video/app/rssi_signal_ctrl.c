
/** @brief    跟据RSSI信号选择视频信号
 *  @file     rssi_signal_ctrl.c
 *  @author   yjsstk@163.com
 *  @version  v1.0
 *  @date     2020/04/16
 *  @note     通过对两路RSSI信号的强度来选择控制视频信号的输出
 */

#include "config.h"
#if (CONFIG_DEBUG_EN == 1)
#define DEBUG_INFO_EN       1
#define DEBUG_MODULE_NAME   "rssi"
#endif
#include "debug.h"

#include <stdint.h>
#include <string.h>
#include "rssi_signal_ctrl.h"
#include "pin30_ctrl.h"
#include "pin12_ctrl.h"
#include "app_scheduler.h"
#include "adc_rssi.h"

static bool     rssi_buff_lock  = false;         // 数据接收数组锁定
static uint8_t  rssi_lost_time  = 0;             // 因锁定接收而错过的数据次数
static uint8_t  rssi_buff_in    = 0;             // 数据存入数组索引号
static uint8_t  rssi_rx_number  = 0;             // 当前数组中的数据个数
static uint16_t rssi_top_buff[RSSI_BUFFER_LEN];  // RSSI_TOP检测到的数据
static uint16_t rssi_bot_buff[RSSI_BUFFER_LEN];  // RSSI_BOT检测到的数据

PIN30_VIDEO_CTRL_T rssi_pin30_status = PIN30_VIDEO_CTRL_UNKNOWN;
PIN12_VIDEO_SEL_T  rssi_pin12_status = PIN12_SEL_UNKNOWN;
 
typedef struct
{
	uint16_t rssi_top_avg;                       // RSSI_TOP平均值
	uint16_t rssi_bot_avg;                       // RSSI_BOT平均值
	uint16_t rssi_top_min;                       // RSSI_TOP最小值
	uint16_t rssi_bot_min;                       // RSSI_BOT最小值
}rssi_status_t;

static rssi_status_t rssi_status;

/** @brief   RSSI信号平均值计算
 *  @param   无
 *  @return  无 
 */
static void rssi_signal_avg_calc(void)
{
	uint32_t top_val=0, bot_val=0;
	
	rssi_buff_lock = true;
	uint8_t  number = rssi_rx_number;
	for (uint16_t i=0; i<number; i++)
	{
		top_val += rssi_top_buff[i];
		bot_val += rssi_bot_buff[i];
	}
	rssi_buff_lock  = false;
	
	rssi_status.rssi_top_avg = top_val / number;
	rssi_status.rssi_bot_avg = bot_val / number;
	
	DEBUG_INFO("numb: %d, loss: %d, top_avg: %d, bot_avg: %d", 
	            rssi_rx_number, rssi_lost_time, rssi_status.rssi_top_avg, rssi_status.rssi_bot_avg);
	
	if (rssi_status.rssi_top_min > top_val)
	{
		rssi_status.rssi_top_min = top_val;
		DEBUG_INFO("rssi_top_min: %d", top_val);
	}
	if (rssi_status.rssi_bot_min > bot_val)
	{
		rssi_status.rssi_bot_min = bot_val;
		DEBUG_INFO("rssi_bot_min: %d", bot_val);
	}
}

/** @brief   视频软件选择控制处理
 *  @param   无 
 *  @return  无
 *  @note    软件在VI1与VI2之间选择一个信号相对较好的做为输出
 */
static void rssi_video_sw_ctrl_deal(void)
{
	PIN12_VIDEO_SEL_T sw_sel = PIN12_SEL_UNKNOWN;
	
	if (rssi_status.rssi_top_avg >= RSSI_1V_VALUE) 
	{
		sw_sel = PIN12_SEL_VI1_TOP;
	}
	else if (rssi_status.rssi_bot_avg >= RSSI_1V_VALUE) 
	{
		sw_sel = PIN12_SEL_VI2_BOT;
	}
	else if (rssi_status.rssi_top_avg == rssi_status.rssi_top_min)
	{
		sw_sel = PIN12_SEL_VI2_BOT;
	}
	else if (rssi_status.rssi_bot_avg == rssi_status.rssi_bot_min)
	{
		sw_sel = PIN12_SEL_VI1_TOP;
	}
	else if (rssi_status.rssi_top_avg > rssi_status.rssi_bot_avg) 
	{
		sw_sel = PIN12_SEL_VI1_TOP;
	}
	else if (rssi_status.rssi_bot_avg > rssi_status.rssi_top_avg)
	{
		sw_sel = PIN12_SEL_VI2_BOT;
	}
	
	if (sw_sel != rssi_pin12_status)
	{
		DEBUG_INFO("rssi_top_avg: %d, rssi_bot_avg: %d", 
		            rssi_status.rssi_top_avg, rssi_status.rssi_bot_avg);
		DEBUG_INFO("video ctrl by sw, sel: %s", 
		            sw_sel == PIN12_SEL_VI2_BOT ? "BOT" : "TOP");
		rssi_pin12_status = sw_sel;
		pin12_set_video_ctrl(sw_sel);
	}
}

/** @brief   视频选择控制处理
 *  @param   无 
 *  @return  无
 */
static void rssi_video_ctrl_deal(void)
{
	if ((rssi_status.rssi_top_avg >= RSSI_1V_VALUE) 
	 && (rssi_status.rssi_bot_avg >= RSSI_1V_VALUE))
	{
		if (rssi_pin30_status != PIN30_VIDEO_CTRL_BY_HW)
		{
			DEBUG_INFO("video ctrl by hw");
			rssi_pin30_status = PIN30_VIDEO_CTRL_BY_HW;
			pin30_set_video_ctrl(PIN30_VIDEO_CTRL_BY_HW);
		}
	}
	else
	{
		if (rssi_pin30_status != PIN30_VIDEO_CTRL_BY_SW)
		{
			rssi_pin30_status = PIN30_VIDEO_CTRL_BY_SW;
			pin30_set_video_ctrl(PIN30_VIDEO_CTRL_BY_SW);
		}
		rssi_video_sw_ctrl_deal();
	}
}

/** @brief   获取RSSI信号判断状态
 *  @param   无 
 *  @return  true  ：两路RSSI信号都低于RSSI_1V_VALUE
 *  @return  false ：至少有一路RSSI信号高于RSSI_1V_VALUE
 */
bool rssi_get_is_both_weak(void)
{
	if ((rssi_status.rssi_top_avg >= RSSI_1V_VALUE) 
	 || (rssi_status.rssi_bot_avg >= RSSI_1V_VALUE))
	{
		return false;
	}
	else
	{
		return true;
	}
}

/** @brief   PWM事件处理
 *  @param   parg[in] 
 *  @param   arg_size[in]
 *  @return  无
 *  @note    
 */
static void rssi_event_handle(void *parg, uint16_t arg_size)
{
	rssi_signal_avg_calc();
	rssi_video_ctrl_deal();
}

/** @brief   RSSI信号检测到数据的回调函数
 *  @param   rssi_top[in] ：RSSI_TOP检测到的数据
 *  @param   rssi_bot[in] ：RSSI_BOT检测到的数据
 *  @return  无 
 */
static void rssi_detect_callbacks(uint16_t rssi_bot, uint16_t rssi_top)
{
	if (rssi_buff_lock == true)
	{
		if (rssi_lost_time < 0xFF)
		{
			rssi_lost_time += 1;
		}
		return;
	}
	rssi_top_buff[rssi_buff_in] = rssi_top;
	rssi_bot_buff[rssi_buff_in] = rssi_bot;
	
	rssi_buff_in = (rssi_buff_in + 1) % RSSI_BUFFER_LEN;
	if (rssi_rx_number < RSSI_BUFFER_LEN)
	{
		rssi_rx_number++;
	}
	app_scheduler_put(rssi_event_handle, NULL, 0);
}

/** @brief   该模块的应用初始化函数 
 *  @param   无 
 *  @return  返回值 @see CONFIG_RESULT_T
 *  @note    使用该模块的其他功能前调用以使初始化模块
 */
CONFIG_RESULT_T rssi_signal_ctrl_init(void)
{
	rssi_status.rssi_top_min = UINT16_MAX;
	rssi_status.rssi_bot_min = UINT16_MAX;
	
	adc_value_change_reg(rssi_detect_callbacks);
	
	return RESULT_SUCCESS;
}

