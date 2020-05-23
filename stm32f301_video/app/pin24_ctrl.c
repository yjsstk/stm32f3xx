
/** @brief    PIN24高低电位输出
 *  @file     pin24_ctrl.c
 *  @author   yjsstk@163.com
 *  @version  v1.0
 *  @date     2020/04/17
 *  @note     当MCU_25收到500MS低位时，而且两个RSSI电压小于0.9V时，MCU_24输出500MS高位
 *  @note     有其中一个RSSI电压大于0.9V时，MCU_24不能输出500MS高位
 */

#include "pin24_ctrl.h"
#include "systick.h"
#include "app_scheduler.h"

static uint16_t pin24_time_ms_cnt;

/** @brief   GPIO输出控制
 *  @param   state[in] @see GPIO_PinState
 *  @return  无
 */
inline void pin24_output_ctrl(GPIO_PinState pin_state)
{
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_14, pin_state);
}

/** @brief   GPIO置为输出模式
 *  @param   state[in] @see GPIO_PinState
 *  @return  无
 */
static void pin24_scheduler_event_handle(void *parg, uint16_t arg_size)
{
	GPIO_InitTypeDef  GPIO_InitStruct;
	
	__HAL_RCC_GPIOA_CLK_ENABLE();

	GPIO_InitStruct.Pin   = GPIO_PIN_14;
	GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull  = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;

	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct); 
	
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_14, GPIO_PIN_RESET);
}


/** @brief   1MS回调函数
 *  @param   pcontent[in] 
 *  @return  无
 *  @note    
 */
static void pin24_1ms_callback(void *pcontent)
{
	if (pin24_time_ms_cnt < UINT16_MAX)
	{
		pin24_time_ms_cnt++;
		if (pin24_time_ms_cnt == 10000)
		{
			app_scheduler_put(pin24_scheduler_event_handle, NULL, 0);
		}
	}
}

/** @brief   该模块的应用初始化函数 
 *  @param   无 
 *  @return  返回值 @see CONFIG_RESULT_T
 *  @note    使用该模块的其他功能前调用以使初始化模块
 */
CONFIG_RESULT_T pin24_ctrl_init(void)
{
//	GPIO_InitTypeDef  GPIO_InitStruct;
//	
//	__HAL_RCC_GPIOA_CLK_ENABLE();

//	GPIO_InitStruct.Pin   = GPIO_PIN_14;
//	GPIO_InitStruct.Mode  = GPIO_MODE_AF_OD;
//	GPIO_InitStruct.Pull  = GPIO_PULLUP;
//	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
//	GPIO_InitStruct.Alternate = GPIO_AF0_SWJ;

//	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct); 
	
	
	pin24_time_ms_cnt = 0;
	systick_1ms_cb_reg(pin24_1ms_callback);
	
	return RESULT_SUCCESS;
}



