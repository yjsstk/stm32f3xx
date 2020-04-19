
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
	}
  
	return RESULT_SUCCESS;
}
