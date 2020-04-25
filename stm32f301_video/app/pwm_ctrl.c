
/** @brief    PWM控制模块
 *  @file     pwm_ctrl.c
 *  @author   yjsstk@163.com
 *  @version  v1.0
 *  @date     2020/04/19
 *  @note     PIN18,PIN19,PIN21的输出波形用PWM控制输出
 */
#include "config.h"
#if (CONFIG_DEBUG_EN == 1)
#define DEBUG_INFO_EN       1
#define DEBUG_MODULE_NAME   "PWM"
#endif
#include "debug.h"

#include "pwm_ctrl.h"
#include "stm32f3xx_hal.h"
#include "stm32f3xx_hal_gpio.h"
#include "stm32f3xx_hal_tim.h"

static ppwm_interrupt_cb_t pwm_interrupt_cb = NULL;
static TIM_HandleTypeDef   TimHandle;
static uint32_t pwm_cycle_ns       = 64000;
static uint32_t pwm_pin18_pulse_ns = 4700;
static uint32_t pwm_pin19_pulse_ns = 10000;
static uint32_t pwm_pin21_pulse_ns = 4700;

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

/** @brief   PWM中断处理
 *  @param   无
 *  @return  无
 */ 
inline void pwm_interrupt_handler(void)
{
	if (__HAL_TIM_GET_IT_SOURCE(&TimHandle, TIM_IT_CC1) == SET)
	{
		__HAL_TIM_CLEAR_IT(&TimHandle, TIM_IT_CC1);
		if (pwm_interrupt_cb != NULL)
		{
			pwm_interrupt_cb(NULL);
		}
	}
	if (__HAL_TIM_GET_IT_SOURCE(&TimHandle, TIM_IT_CC2) == SET)
	{
		__HAL_TIM_CLEAR_IT(&TimHandle, TIM_IT_CC2);
	}
	if (__HAL_TIM_GET_IT_SOURCE(&TimHandle, TIM_IT_CC3) == SET)
	{
		__HAL_TIM_CLEAR_IT(&TimHandle, TIM_IT_CC3);
	}
	if (__HAL_TIM_GET_IT_SOURCE(&TimHandle, TIM_IT_CC4) == SET)
	{
		__HAL_TIM_CLEAR_IT(&TimHandle, TIM_IT_CC4);
	}
}

/** @brief   PIN18,PIN19停止PWM输出
 *  @param   无
 *  @return  无
 */
void pwm_pin18_pin19_stop(void)
{

}

/** @brief   PIN18,PIN19开始PWM输出
 *  @param   cycle_ns[in] ：PWM周期
 *  @param   pin18_pulse_ns[in] ：高电平时间
 *  @param   pin19_pulse_ns[in] ：高电平时间
 *  @return  无
 */
void pwm_pin18_pin19_start(uint32_t cycle_ns, uint32_t pin18_pulse_ns, uint32_t pin19_pulse_ns)
{

}

/** @brief   PIN21开始PWM输出
 *  @param   cycle_ns[in] ：PWM周期
 *  @param   pulse_ns[in] ：低电平时间
 *  @return  无
 */
void pwm_pin21_start(uint32_t cycle_ns, uint32_t pulse_ns)
{

}

/** @brief   GIPO初始化
 *  @param   无 
 *  @return  无 
 */
void pwm_gpio_init(void)
{
  GPIO_InitTypeDef   GPIO_InitStruct;

  __HAL_RCC_GPIOA_CLK_ENABLE();

  /* Common configuration for all channels */
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;

  GPIO_InitStruct.Alternate = GPIO_AF2_TIM1;
  GPIO_InitStruct.Pin = GPIO_PIN_8;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  GPIO_InitStruct.Alternate = GPIO_AF2_TIM1;
  GPIO_InitStruct.Pin = GPIO_PIN_9;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  GPIO_InitStruct.Alternate = GPIO_AF2_TIM1;
  GPIO_InitStruct.Pin = GPIO_PIN_11;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}

/** @brief   该模块的应用初始化函数 
 *  @param   无 
 *  @return  返回值 @see CONFIG_RESULT_T
 *  @note    使用该模块的其他功能前调用以使初始化模块
 */
CONFIG_RESULT_T pwm_ctrl_init(void)
{
	pwm_gpio_init();
	
	HAL_NVIC_SetPriority(TIM1_CC_IRQn, 2, 0U);
	HAL_NVIC_EnableIRQ(TIM1_CC_IRQn);
	
	__HAL_RCC_TIM1_CLK_ENABLE();
	
	TimHandle.Instance = TIM1;

	TimHandle.Init.Prescaler         = 1000;
	TimHandle.Init.Period            = 71;
	TimHandle.Init.ClockDivision     = 0;
	TimHandle.Init.CounterMode       = TIM_COUNTERMODE_UP;
	TimHandle.Init.RepetitionCounter = 0;
	TimHandle.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;

	if (HAL_TIM_PWM_Init(&TimHandle) != HAL_OK)
	{
		return RESULT_ERROR;
	}
	
	__HAL_TIM_ENABLE_IT(&TimHandle, TIM1_CC_IRQn);
	
	TIM_OC_InitTypeDef sConfig;
	
	sConfig.OCMode       = TIM_OCMODE_PWM1;
	sConfig.OCPolarity   = TIM_OCPOLARITY_HIGH;
	sConfig.OCFastMode   = TIM_OCFAST_ENABLE;
	sConfig.OCNPolarity  = TIM_OCNPOLARITY_LOW;
	sConfig.OCNIdleState = TIM_OCNIDLESTATE_RESET;

	sConfig.OCIdleState  = TIM_OCIDLESTATE_RESET;

	sConfig.Pulse = 500;
	if (HAL_TIM_PWM_ConfigChannel(&TimHandle, &sConfig, TIM_CHANNEL_1) != HAL_OK)
	{
		return RESULT_ERROR;
	}

	/* Set the pulse value for channel 2 */
	sConfig.Pulse = 500;
	if (HAL_TIM_PWM_ConfigChannel(&TimHandle, &sConfig, TIM_CHANNEL_2) != HAL_OK)
	{
		return RESULT_ERROR;
	}
	
	/*##-3- Start PWM signals generation #######################################*/
	/* Start channel 1 */
	if (HAL_TIM_PWM_Start_IT(&TimHandle, TIM_CHANNEL_1) != HAL_OK)
	{
		return RESULT_ERROR;
	}
	/* Start channel 2 */
	if (HAL_TIM_PWM_Start_IT(&TimHandle, TIM_CHANNEL_2) != HAL_OK)
	{
		return RESULT_ERROR;
	}
  
	return RESULT_SUCCESS;
}
