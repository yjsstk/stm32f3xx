
/** @brief    PWM控制模块
 *  @file     pwm_ctrl.c
 *  @author   yjsstk@163.com
 *  @version  v1.0
 *  @date     2020/04/19
 *  @note     PIN18,PIN19,PIN21的输出波形用PWM控制输出
 */

#include "pwm_ctrl.h"
#include "stm32f3xx_hal.h"
#include "stm32f3xx_hal_gpio.h"
#include "stm32f3xx_hal_tim.h"

static ppwm_interrupt_cb_t pwm_interrupt_cb=NULL;

static TIM_HandleTypeDef  TimHandle;

/** @brief   PWM中断回调函数注册
 *  @param   func[in] 回调函数
 *  @return  返回值 @see CONFIG_RESULT_T
 */
CONFIG_RESULT_T pwm_interrupt_cb_reg(ppwm_interrupt_cb_t func)
{
	if (pwm_interrupt_cb == NULL)
	{
		pwm_interrupt_cb = func;
		return RESULT_SUCCESS;
	}
	return RESULT_ERROR;
}

/** @brief   该模块的应用初始化函数 
 *  @param   无 
 *  @return  返回值 @see CONFIG_RESULT_T
 *  @note    使用该模块的其他功能前调用以使初始化模块
 */
CONFIG_RESULT_T pwm_ctrl_init(void)
{
	TimHandle.Instance = TIM1;

	TimHandle.Init.Prescaler         = 1000;
	TimHandle.Init.Period            = 1000;
	TimHandle.Init.ClockDivision     = 0;
	TimHandle.Init.CounterMode       = TIM_COUNTERMODE_UP;
	TimHandle.Init.RepetitionCounter = 0;
	TimHandle.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;

	if (HAL_TIM_PWM_Init(&TimHandle) != HAL_OK)
	{
		return RESULT_ERROR;
	}
	
	TIM_OC_InitTypeDef sConfig;
	
	sConfig.OCMode       = TIM_OCMODE_PWM1;
	sConfig.OCPolarity   = TIM_OCPOLARITY_HIGH;
	sConfig.OCFastMode   = TIM_OCFAST_DISABLE;
	sConfig.OCNPolarity  = TIM_OCNPOLARITY_HIGH;
	sConfig.OCNIdleState = TIM_OCNIDLESTATE_RESET;

	sConfig.OCIdleState  = TIM_OCIDLESTATE_RESET;

	sConfig.Pulse = 100;
	if (HAL_TIM_PWM_ConfigChannel(&TimHandle, &sConfig, TIM_CHANNEL_1) != HAL_OK)
	{
		return RESULT_ERROR;
	}

	/* Set the pulse value for channel 2 */
	sConfig.Pulse = 100;
	if (HAL_TIM_PWM_ConfigChannel(&TimHandle, &sConfig, TIM_CHANNEL_2) != HAL_OK)
	{
		return RESULT_ERROR;
	}

	/* Set the pulse value for channel 3 */
	sConfig.Pulse = 100;
	if (HAL_TIM_PWM_ConfigChannel(&TimHandle, &sConfig, TIM_CHANNEL_3) != HAL_OK)
	{
		return RESULT_ERROR;
	}

	/* Set the pulse value for channel 4 */
	sConfig.Pulse = 100;
	if (HAL_TIM_PWM_ConfigChannel(&TimHandle, &sConfig, TIM_CHANNEL_4) != HAL_OK)
	{
		return RESULT_ERROR;
	}

	/*##-3- Start PWM signals generation #######################################*/
	/* Start channel 1 */
	if (HAL_TIM_PWM_Start(&TimHandle, TIM_CHANNEL_1) != HAL_OK)
	{
		return RESULT_ERROR;
	}
	/* Start channel 2 */
	if (HAL_TIM_PWM_Start(&TimHandle, TIM_CHANNEL_2) != HAL_OK)
	{
		return RESULT_ERROR;
	}
	/* Start channel 3 */
	if (HAL_TIM_PWM_Start(&TimHandle, TIM_CHANNEL_3) != HAL_OK)
	{
		return RESULT_ERROR;
	}
	/* Start channel 4 */
	if (HAL_TIM_PWM_Start(&TimHandle, TIM_CHANNEL_4) != HAL_OK)
	{
		return RESULT_ERROR;
	}
  
	return RESULT_SUCCESS;
}
