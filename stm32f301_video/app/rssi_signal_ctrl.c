
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
#include "dac.h"
#include "pin22_ctrl.h"
#include "systick.h"

static bool     rssi_both_weak  = false;         // 两个RSSI信号都小于0.9V
static uint8_t  rssi_buff_in    = 0;             // 数据存入数组索引号
static uint8_t  rssi_rx_number  = 0;             // 当前数组中的数据个 
static uint16_t rssi_switch_time_cnt = 0;        // 视频切换计时
static uint16_t rssi_top_buff[RSSI_BUFFER_LEN];  // RSSI_TOP检测到的数据
static uint16_t rssi_bot_buff[RSSI_BUFFER_LEN];  // RSSI_BOT检测到的数据

PIN30_VIDEO_CTRL_T rssi_pin30_cur_status  = PIN30_VIDEO_CTRL_UNKNOWN;
PIN30_VIDEO_CTRL_T rssi_pin30_next_status = PIN30_VIDEO_CTRL_UNKNOWN;
PIN12_VIDEO_SEL_T  rssi_pin12_cur_status  = PIN12_SEL_UNKNOWN;

typedef struct
{
	uint16_t rssi_top_adc;                       // ADC采样值
	uint16_t rssi_bot_adc;                       // ADC采样值
}rssi_scheduler_msg_t;
 
typedef struct
{
	uint16_t rssi_top_avg;                       // RSSI_TOP平均值
	uint16_t rssi_bot_avg;                       // RSSI_BOT平均值
	uint16_t rssi_top_min;                       // RSSI_TOP最小值
	uint16_t rssi_bot_min;                       // RSSI_BOT最小值
}rssi_status_t;

static volatile rssi_status_t rssi_status;

/** @brief   视频软件选择控制处理
 *  @param   无 
 *  @return  无
 *  @note    软件在VI1与VI2之间选择一个信号相对较好的做为输出
 */
static void rssi_video_sw_ctrl_deal(void)
{
	PIN12_VIDEO_SEL_T sw_sel = rssi_pin12_cur_status;
	
	if (rssi_status.rssi_top_avg >= RSSI_TOP_ADC_VALUE) 
	{
		sw_sel = PIN12_SEL_VI1_TOP;
		
		pin22_output_ctrl(GPIO_PIN_RESET);
		dac_set_output_vcc(DAC_OUTPUT_VCC);
	}
	else if (rssi_status.rssi_bot_avg >= RSSI_BOT_ADC_VALUE) 
	{
		sw_sel = PIN12_SEL_VI2_BOT;
		
		pin22_output_ctrl(GPIO_PIN_RESET);
		dac_set_output_vcc(DAC_OUTPUT_VCC);
	}
	else
	{
		uint16_t top_val = rssi_status.rssi_top_avg - rssi_status.rssi_top_min;
		uint16_t bot_val = rssi_status.rssi_bot_avg - rssi_status.rssi_bot_min;
		
//		DEBUG_INFO("top_vag: %d, top_min: %d",rssi_status.rssi_top_avg, rssi_status.rssi_top_min);
//		DEBUG_INFO("bot_val: %d, bot_min: %d",rssi_status.rssi_bot_avg, rssi_status.rssi_bot_min);
//		DEBUG_INFO("top_val: %d, bot_val: %d", top_val, bot_val);
		if (top_val > bot_val)
		{
			sw_sel = PIN12_SEL_VI1_TOP;
		}
		else if (top_val < bot_val)
		{
			sw_sel = PIN12_SEL_VI2_BOT;
		}
		
		pin22_output_ctrl(GPIO_PIN_SET);
		dac_set_output_vcc(0);
	}
	
	if (sw_sel != rssi_pin12_cur_status)
	{
		DEBUG_INFO("video ctrl by sw, sel: %s", 
		            (sw_sel == PIN12_SEL_VI2_BOT) ? "BOT" : "TOP");
		rssi_pin12_cur_status = sw_sel;
		pin12_set_video_ctrl(sw_sel);
	}
}

/** @brief   获取RSSI信号判断状态
 *  @param   无 
 *  @return  true  ：两路RSSI信号都低于RSSI_1V_VALUE
 *  @return  false ：至少有一路RSSI信号高于RSSI_1V_VALUE
 */
bool rssi_get_is_both_weak(void)
{
	return rssi_both_weak;
}

/** @brief   RSSI信号处理
 *  @param   parg[in] 
 *  @param   arg_size[in]
 *  @return  无
 *  @note    
 */
static void rssi_scheduler_event_handle(void *parg, uint16_t arg_size)
{
	rssi_scheduler_msg_t *pmsg = (rssi_scheduler_msg_t *)parg;
	
	// 保存数据
	rssi_top_buff[rssi_buff_in] = pmsg->rssi_top_adc;
	rssi_bot_buff[rssi_buff_in] = pmsg->rssi_bot_adc;
	
	// 调整保存数据位置
	rssi_buff_in = (rssi_buff_in + 1) % RSSI_BUFFER_LEN;
	if (rssi_rx_number < RSSI_BUFFER_LEN)
	{
		rssi_rx_number++;
	}
	
	// 计算平均值
	uint32_t top_val=0, bot_val=0;
	for (uint16_t i=0; i<rssi_rx_number; i++)
	{
		top_val += rssi_top_buff[i];
		bot_val += rssi_bot_buff[i];
	}
	rssi_status.rssi_top_avg = top_val / rssi_rx_number;
	rssi_status.rssi_bot_avg = bot_val / rssi_rx_number;
	
//	DEBUG_INFO("numb: %d, top_avg: %d, bot_avg: %d", 
//	            rssi_rx_number, rssi_status.rssi_top_avg, rssi_status.rssi_bot_avg);
	
	// 记录最小值
	if (rssi_status.rssi_top_min > rssi_status.rssi_top_avg)
	{
		rssi_status.rssi_top_min = rssi_status.rssi_top_avg;
		DEBUG_INFO("rssi_top_min: %d", rssi_status.rssi_top_avg);
	}
	// 记录最小值
	if (rssi_status.rssi_bot_min > rssi_status.rssi_bot_avg)
	{
		rssi_status.rssi_bot_min = rssi_status.rssi_bot_avg;
		DEBUG_INFO("rssi_bot_min: %d", rssi_status.rssi_bot_avg);
	}
	
	// 控制状态
	if ((rssi_status.rssi_top_avg >= RSSI_TOP_ADC_VALUE) 
	 && (rssi_status.rssi_bot_avg >= RSSI_BOT_ADC_VALUE))
	{
		rssi_pin30_next_status = PIN30_VIDEO_CTRL_BY_HW;
		DEBUG_INFO("next_status: HW");
	}
	else
	{
		rssi_pin30_next_status = PIN30_VIDEO_CTRL_BY_SW;
		DEBUG_INFO("next_status: SW");
	}
}

/** @brief   RSSI信号检测到数据的回调函数
 *  @param   rssi_top[in] ：RSSI_TOP检测到的数据
 *  @param   rssi_bot[in] ：RSSI_BOT检测到的数据
 *  @return  无 
 */
static void rssi_detect_callbacks(uint16_t rssi_bot, uint16_t rssi_top)
{	
	rssi_scheduler_msg_t msg;
	msg.rssi_bot_adc = rssi_bot;
	msg.rssi_top_adc = rssi_top;
	app_scheduler_put(rssi_scheduler_event_handle, (void *)&msg, sizeof(msg));
}

/** @brief   1ms回调函数
 *  @param   无 
 *  @return  
 */
static void rssi_time_1ms_cb(void *pcontent)
{
	rssi_switch_time_cnt++;
	if (rssi_pin30_cur_status == rssi_pin30_next_status)
	{
		rssi_switch_time_cnt = 0;
	}
	else if (rssi_switch_time_cnt >= RSSI_VIDEO_DELAY_SWITCH_MS)
	{
		rssi_pin30_cur_status = rssi_pin30_next_status;
		if (rssi_pin30_next_status == PIN30_VIDEO_CTRL_BY_HW)
		{
			DEBUG_INFO("video switch by hw");
			dac_set_output_vcc(DAC_OUTPUT_VCC);
			pin30_set_video_ctrl(PIN30_VIDEO_CTRL_BY_HW);
			pin12_set_video_ctrl(PIN12_SEL_VI1_TOP);
			pin22_output_ctrl(GPIO_PIN_RESET);
		}
		else if (rssi_pin30_next_status == PIN30_VIDEO_CTRL_BY_SW)
		{
			DEBUG_INFO("video switch by sw");
			pin30_set_video_ctrl(PIN30_VIDEO_CTRL_BY_SW);
			rssi_video_sw_ctrl_deal();
		}
		
		if ((rssi_status.rssi_top_avg >= RSSI_TOP_ADC_VALUE) 
		 || (rssi_status.rssi_bot_avg >= RSSI_BOT_ADC_VALUE))
		{
			rssi_both_weak = false;
		}
		else
		{
			rssi_both_weak = true;
		}
	}
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
	
	systick_1ms_cb_reg(rssi_time_1ms_cb);
	adc_value_change_reg(rssi_detect_callbacks);
	
	return RESULT_SUCCESS;
}

