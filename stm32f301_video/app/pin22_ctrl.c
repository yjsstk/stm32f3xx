
/** @brief    PIN22高低电位输出
 *  @file     pin22_ctrl.c
 *  @author   yjsstk@163.com
 *  @version  v1.0
 *  @date     2020/04/17
 */

#include "pin22_ctrl.h"

/** @brief   GPIO输出控制
 *  @param   state[in] @see GPIO_PinState
 *  @return  无
 */
inline void pin22_output_ctrl(GPIO_PinState pin_state)
{
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12, pin_state);
}

/** @brief   该模块的应用初始化函数 
 *  @param   无 
 *  @return  返回值 @see CONFIG_RESULT_T
 *  @note    使用该模块的其他功能前调用以使初始化模块
 */
CONFIG_RESULT_T pin22_ctrl_init(void)
{
	GPIO_InitTypeDef  GPIO_InitStruct;
	
	__HAL_RCC_GPIOA_CLK_ENABLE();

	GPIO_InitStruct.Pin   = GPIO_PIN_12;
	GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull  = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;

	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct); 
	
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12, GPIO_PIN_RESET);
	
	return RESULT_SUCCESS;
}



