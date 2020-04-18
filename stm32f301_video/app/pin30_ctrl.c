
/** @brief    视频输出信号由软件或硬件控制处理
 *  @file     pin30_ctrl.c
 *  @author   yjsstk@163.com
 *  @version  v1.0
 *  @date     2020/04/17
 *  @note     PIN30输出低，视频输出信号由硬件控制处理
 *  @note     PIN30输出高，视频输出信号由软件控制处理 
 *  @note     软件控制由PIN12控制
 */

#include "pin30_ctrl.h"
#include "stm32f3xx_hal.h"
#include "stm32f3xx_hal_gpio.h"

/** @brief   视频处理控制
 *  @param   ctrl[in] @see PIN30_VIDEO_CTRL_T
 *  @return  无 
 *  @note    PIN30输出低：视频输出信号由硬件控制处理
 *  @note    PIN30输出高：视频输出信号由软件控制处理 
 */
void pin30_set_video_ctrl(PIN30_VIDEO_CTRL_T ctrl)
{
	if (ctrl == PIN30_VIDEO_CTRL_BY_HW)
	{
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_RESET);
	}
	else if (ctrl == PIN30_VIDEO_CTRL_BY_SW)
	{
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_SET);
	}
}

/** @brief   该模块的应用初始化函数 
 *  @param   无 
 *  @return  返回值 @see CONFIG_RESULT_T
 *  @note    使用该模块的其他功能前调用以使初始化模块
 */
CONFIG_RESULT_T pin30_ctrl_init(void)
{
	GPIO_InitTypeDef  GPIO_InitStruct;
	
	__HAL_RCC_GPIOB_CLK_ENABLE();

	GPIO_InitStruct.Pin   = GPIO_PIN_7;
	GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull  = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;

	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct); 
	
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_RESET);
	
	return RESULT_SUCCESS;
}



