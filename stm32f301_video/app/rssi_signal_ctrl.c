
/** @brief    跟据RSSI信号选择视频信号
 *  @file     rssi_signal_ctrl.c
 *  @author   yjsstk@163.com
 *  @version  v1.0
 *  @date     2020/04/16
 *  @note     通过对两路RSSI信号的强度来选择控制视频信号的输出
 */

#include "config.h"
#if (CONFIG_DEBUG_EN == 1)
#define DEBUG_INFO_EN       0
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
#include "pin25_ctrl.h"
#include "systick.h"

static uint8_t  rssi_buff_in    = 0;             // 数据存入数组索引号
static uint8_t  rssi_rx_number  = 0;             // 当前数组中的数据个 
static uint16_t rssi_switch_time_cnt = 0;        // 视频切换计时
static uint16_t rssi_top_buff[RSSI_BUFFER_LEN];  // RSSI_TOP检测到的数据
static uint16_t rssi_bot_buff[RSSI_BUFFER_LEN];  // RSSI_BOT检测到的数据

static uint8_t  rssi_ctrl_by_sw_state = 0xFF;       

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

/** @brief   PIN22延时控制
 *  @param   无 
 *  @return  无
 *  @note    
 */
static void rssi_pin22_delay_ctrl(void)
{
//	PIN12_VIDEO_SEL_T sw_sel = rssi_pin12_cur_status;
	
	uint16_t top_standard_adc = rssi_status.rssi_top_min + RSSI_VCC_TO_ADC(RSSI_NORMAL_UP_VCC);
	uint16_t bot_standard_adc = rssi_status.rssi_bot_min + RSSI_VCC_TO_ADC(RSSI_NORMAL_UP_VCC);
	if (rssi_status.rssi_top_avg >= top_standard_adc) 
	{
		pin22_output_ctrl(GPIO_PIN_RESET);
	}
	else if (rssi_status.rssi_bot_avg >= bot_standard_adc) 
	{
		pin22_output_ctrl(GPIO_PIN_RESET);
	}
	else
	{
		pin22_output_ctrl(GPIO_PIN_SET);
	}
}


/** @brief   视频软件选择控制处理
 *  @param   无 
 *  @return  无
 *  @note    软件在VI1与VI2之间选择一个信号相对较好的做为输出
 */
static void rssi_video_sw_ctrl_deal(void)
{
	PIN12_VIDEO_SEL_T sw_sel = rssi_pin12_cur_status;
	
	uint16_t top_standard_adc = rssi_status.rssi_top_min + RSSI_VCC_TO_ADC(RSSI_NORMAL_UP_VCC);
	uint16_t bot_standard_adc = rssi_status.rssi_bot_min + RSSI_VCC_TO_ADC(RSSI_NORMAL_UP_VCC);
	if (rssi_status.rssi_top_avg >= top_standard_adc) 
	{
		sw_sel = PIN12_SEL_VI1_TOP;
		
//		pin22_output_ctrl(GPIO_PIN_RESET);
		dac_set_output_vcc(DAC_OUTPUT_VCC);
	}
	else if (rssi_status.rssi_bot_avg >= bot_standard_adc) 
	{
		sw_sel = PIN12_SEL_VI2_BOT;
		
//		pin22_output_ctrl(GPIO_PIN_RESET);
		dac_set_output_vcc(DAC_OUTPUT_VCC);
	}
	else
	{
//		pin22_output_ctrl(GPIO_PIN_SET);
		dac_set_output_vcc(0);
		if (rssi_status.rssi_top_avg <= rssi_status.rssi_top_min)
		{
			sw_sel = PIN12_SEL_VI2_BOT;
		}
		else if (rssi_status.rssi_bot_avg <= rssi_status.rssi_bot_min)
		{
			sw_sel = PIN12_SEL_VI1_TOP;
		}
		else
		{
			uint16_t top_val = rssi_status.rssi_top_avg - rssi_status.rssi_top_min;
			uint16_t bot_val = rssi_status.rssi_bot_avg - rssi_status.rssi_bot_min;
			if (top_val > bot_val)
			{
				sw_sel = PIN12_SEL_VI1_TOP;
			}
			else if (top_val < bot_val)
			{
				sw_sel = PIN12_SEL_VI2_BOT;
			}
		}
	}
	
	if (sw_sel != rssi_pin12_cur_status)
	{
		DEBUG_INFO("video ctrl by sw, sel: %s", 
		            (sw_sel == PIN12_SEL_VI2_BOT) ? "BOT" : "TOP");
		rssi_pin12_cur_status = sw_sel;
		pin12_set_video_ctrl(sw_sel);
	}
}

/** @brief   是否须要进行软件切换判断
 *  @param   无 
 *  @return  
 *  @note    
 */
static uint8_t rssi_video_sw_switch_detect(void)
{	
	static uint8_t switch_bk=0xFF;
	uint8_t sw_sitch=0xFF;
	
	uint16_t top_standard_adc = rssi_status.rssi_top_min + RSSI_VCC_TO_ADC(RSSI_NORMAL_UP_VCC);
	uint16_t bot_standard_adc = rssi_status.rssi_bot_min + RSSI_VCC_TO_ADC(RSSI_NORMAL_UP_VCC);
	if (rssi_status.rssi_top_avg >= top_standard_adc) 
	{
		sw_sitch = 1;
	}
	else if (rssi_status.rssi_bot_avg >= bot_standard_adc) 
	{
		sw_sitch = 2;
	}
	else
	{
		if (rssi_status.rssi_top_avg <= rssi_status.rssi_top_min)
		{
			sw_sitch = 3;
		}
		else if (rssi_status.rssi_bot_avg <= rssi_status.rssi_bot_min)
		{
			sw_sitch = 4;
		}
		else
		{
			uint16_t top_val = rssi_status.rssi_top_avg - rssi_status.rssi_top_min;
			uint16_t bot_val = rssi_status.rssi_bot_avg - rssi_status.rssi_bot_min;
			if (top_val > bot_val)
			{
				sw_sitch = 5;
			}
			else if (top_val < bot_val)
			{
				sw_sitch = 6;
			}
		}
	}
	if (switch_bk != sw_sitch)
	{
		switch_bk = sw_sitch;
		DEBUG_INFO("Ti: %d, Bi: %d, Ts: %d, Bs: %d", 
		    rssi_status.rssi_top_min, rssi_status.rssi_bot_min,
		    top_standard_adc, bot_standard_adc);
		DEBUG_INFO("Ta: %d, Ba: %d, sw: %d", 
	            rssi_status.rssi_top_avg, rssi_status.rssi_bot_avg, sw_sitch);
	}
	
	return sw_sitch;
}

/** @brief   判断两路RSSI是否都小于指定的VCC值
 *  @param   VCC[in] : 判断值
 *  @return  true  ：两路RSSI信号都低于VCC
 *  @return  false ：至少有一路RSSI信号高于VCC
 */
bool rssi_get_is_both_weak(float VCC)
{
	if ((rssi_status.rssi_top_avg >= RSSI_VCC_TO_ADC(VCC)) ||
		(rssi_status.rssi_bot_avg >= RSSI_VCC_TO_ADC(VCC)))
	{
		return false;
	}
	else
	{
		return true;
	}
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
	
	// 控制状态
	uint16_t top_standard_adc = rssi_status.rssi_top_min + RSSI_VCC_TO_ADC(RSSI_NORMAL_UP_VCC);
	uint16_t bot_standard_adc = rssi_status.rssi_bot_min + RSSI_VCC_TO_ADC(RSSI_NORMAL_UP_VCC);
	if ((rssi_status.rssi_top_avg >= top_standard_adc) 
	 && (rssi_status.rssi_bot_avg >= bot_standard_adc))
	{
		rssi_pin30_next_status = PIN30_VIDEO_CTRL_BY_HW;
//		DEBUG_INFO("next_status: HW");
	}
	else
	{
		rssi_pin30_next_status = PIN30_VIDEO_CTRL_BY_SW;
//		DEBUG_INFO("next_status: SW");
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

/** @brief   RSSI信号校准通知回调函数
 *  @param   无
 *  @return  无 
 */
static void rssi_calibration_notify_cb(void)
{	
	rssi_status.rssi_top_min = rssi_status.rssi_top_avg;
	rssi_status.rssi_bot_min = rssi_status.rssi_bot_avg;
	DEBUG_INFO("top_min: %d, bot_min: %d", rssi_status.rssi_top_min, rssi_status.rssi_bot_min);
}

/** @brief   1ms回调函数
 *  @param   无 
 *  @return  
 */
static void rssi_time_1ms_cb(void *pcontent)
{
	rssi_switch_time_cnt++;
	if (PIN30_VIDEO_CTRL_BY_HW == rssi_pin30_next_status)
	{
		if (rssi_pin30_cur_status != rssi_pin30_next_status)
		{			
//			if (rssi_switch_time_cnt >= RSSI_VIDEO_DELAY_SWITCH_MS)
			{
				DEBUG_INFO("Ta: %d, Ba: %d", rssi_status.rssi_top_avg, rssi_status.rssi_bot_avg);
				rssi_pin30_cur_status = PIN30_VIDEO_CTRL_BY_HW;
				DEBUG_INFO("video switch by hw");
				dac_set_output_vcc(DAC_OUTPUT_VCC);
				pin30_set_video_ctrl(PIN30_VIDEO_CTRL_BY_HW);
				pin12_set_video_ctrl(PIN12_SEL_VI1_TOP);
				pin22_output_ctrl(GPIO_PIN_RESET);
				rssi_ctrl_by_sw_state = 0xff;
			}
		}
		else
		{
			rssi_switch_time_cnt = 0;
		}
	}
	else if (rssi_pin30_next_status == PIN30_VIDEO_CTRL_BY_SW)
	{
		uint8_t sw_temp = rssi_video_sw_switch_detect();
		if (sw_temp != rssi_ctrl_by_sw_state)
		{
			if (rssi_pin30_cur_status != PIN30_VIDEO_CTRL_BY_SW)
			{
				rssi_pin30_cur_status = PIN30_VIDEO_CTRL_BY_SW;
				pin30_set_video_ctrl(PIN30_VIDEO_CTRL_BY_SW);
			}
			rssi_ctrl_by_sw_state = 0xff;
			rssi_video_sw_ctrl_deal();
			if (rssi_switch_time_cnt >= RSSI_VIDEO_DELAY_SWITCH_MS)
			{
				rssi_pin22_delay_ctrl();
				rssi_ctrl_by_sw_state = sw_temp;
			}
		}
		else
		{
			rssi_switch_time_cnt = 0;
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
	rssi_status.rssi_top_min = RSSI_VCC_TO_ADC(RSSI_DEFAULT_VCC);
	rssi_status.rssi_bot_min = RSSI_VCC_TO_ADC(RSSI_DEFAULT_VCC);
	rssi_ctrl_by_sw_state = 0xFF; 
	
	systick_1ms_cb_reg(rssi_time_1ms_cb);
	adc_value_change_reg(rssi_detect_callbacks);
	pin25_calibration_cb_reg(rssi_calibration_notify_cb);
	
	return RESULT_SUCCESS;
}

