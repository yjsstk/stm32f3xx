
/** @brief    PIN12根据RSSI值选择视频输出
 *  @file     pin12_ctrl.h
 *  @author   yjsstk@163.com
 *  @version  v1.0
 *  @date     2020/04/17
 *  @note     视频输出由软件控制时可选择V1或V2做为输出
 */

#include "pin12_ctrl.h"
#include "stm32f3xx_hal.h"
#include "stm32f3xx_hal_gpio.h"

/** @brief   设置视频选择控制
 *  @param   sel[in] @see PIN12_VIDEO_SEL_T
 *  @return  无 
 *  @note    PIN12输出低：PIN12_SEL_VI1_TOP
 *  @note    PIN12输出高：PIN12_SEL_VI2_BOT
 */
void pin12_set_video_ctrl(PIN12_VIDEO_SEL_T sel)
{
	if (sel == PIN12_SEL_VI1_TOP)
	{
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);
	}
	else
	{
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET);
	}
}

/** @brief   该模块的应用初始化函数 
 *  @param   无 
 *  @return  返回值 @see CONFIG_RESULT_T
 *  @note    使用该模块的其他功能前调用以使初始化模块
 */
CONFIG_RESULT_T pin12_ctrl_init(void)
{
	GPIO_InitTypeDef  GPIO_InitStruct;
	
	__HAL_RCC_GPIOA_CLK_ENABLE();

	GPIO_InitStruct.Pin   = GPIO_PIN_5;
	GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull  = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;

	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct); 
	
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);
	
	return RESULT_SUCCESS;
}



