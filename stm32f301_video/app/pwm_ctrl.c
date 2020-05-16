
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

#define  PWM_NS_TO_72MHZ_CNT(ns)   ((ns) * 72 / 1000)

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

/** @brief   TIM1更新中断
 *  @param   无
 *  @return  无
 */ 
inline void pwm_tim1_up_interrupt_handler(void)
{
	if (__HAL_TIM_GET_IT_SOURCE(&TimHandle, TIM_IT_UPDATE) == SET)
	{
		__HAL_TIM_CLEAR_IT(&TimHandle, TIM_IT_UPDATE);
		
		if (pwm_interrupt_cb != NULL)
		{
			pwm_interrupt_cb(NULL);
		}
	}
}

/** @brief   PIN18,PIN19停止PWM输出
 *  @param   无
 *  @return  无
 */
void pwm_pin18_pin19_stop(void)
{
	TimHandle.Instance->CCR1 = 0;
	TimHandle.Instance->CCR2 = 0;
}

/** @brief   PIN18,PIN19开始PWM输出
 *  @param   cycle_ns[in] ：PWM周期
 *  @param   pin18_pulse_ns[in] ：高电平时间
 *  @param   pin19_pulse_ns[in] ：高电平时间
 *  @param   pin21_pulse_ns[in] ：低电平时间
 *  @return  无
 */
void pwm_set_output(
	uint32_t cycle_ns, 
	uint32_t pin18_pulse_ns, 
	uint32_t pin19_pulse_ns, 
	uint32_t pin21_pulse_ns)
{
	__HAL_TIM_SET_AUTORELOAD(&TimHandle, PWM_NS_TO_72MHZ_CNT(cycle_ns) - 1);
//	TimHandle.Instance->ARR  = PWM_NS_TO_72MHZ_CNT(cycle_ns) - 1;
	TimHandle.Instance->CCR1 = PWM_NS_TO_72MHZ_CNT(pin18_pulse_ns);
	TimHandle.Instance->CCR2 = PWM_NS_TO_72MHZ_CNT(pin19_pulse_ns);
	TimHandle.Instance->CCR4 = PWM_NS_TO_72MHZ_CNT(pin21_pulse_ns);
}

/** @brief   设置ARR自动重装的状态
 *  @param   status[in] 启用或禁用自动重装功能
 *  @return  无
 */
void pwm_set_auto_reload_preload(bool status)
{
	if (status == true)
	{
		TimHandle.Instance->CR1   |= 0x0080;
//		TimHandle.Instance->CCMR1 |= TIM_CCMR1_OC1PE;
	}
	else
	{
		TimHandle.Instance->CR1   &= ~(0x0080);
//		TimHandle.Instance->CCMR1 &= ~TIM_CCMR1_OC1PE;
	}
}

/** @brief   GIPO初始化
 *  @param   无 
 *  @return  无 
 */
void pwm_gpio_init(void)
{
	GPIO_InitTypeDef   GPIO_InitStruct;

	/* Enable all GPIO Channels Clock requested */
	__HAL_RCC_GPIOA_CLK_ENABLE();

	/* Common configuration for all channels */
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;

	GPIO_InitStruct.Alternate = GPIO_AF6_TIM1;
	GPIO_InitStruct.Pin = GPIO_PIN_8;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	GPIO_InitStruct.Alternate = GPIO_AF6_TIM1;
	GPIO_InitStruct.Pin = GPIO_PIN_9;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	GPIO_InitStruct.Alternate = GPIO_AF11_TIM1;
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
	
	HAL_NVIC_SetPriority(TIM1_UP_TIM16_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(TIM1_UP_TIM16_IRQn);
	
	__HAL_RCC_TIM1_CLK_ENABLE();
	
	TimHandle.Instance = TIM1;

	TimHandle.Init.Period            = PWM_NS_TO_72MHZ_CNT(pwm_cycle_ns) - 1;
	TimHandle.Init.Prescaler         = 0;
	TimHandle.Init.ClockDivision     = 0;
	TimHandle.Init.CounterMode       = TIM_COUNTERMODE_UP;
	TimHandle.Init.RepetitionCounter = 0;
	TimHandle.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;

	if (HAL_TIM_Base_Init(&TimHandle) != HAL_OK)
	{
		return RESULT_ERROR;
	}

	
	if (HAL_TIM_PWM_Init(&TimHandle) != HAL_OK)
	{
		return RESULT_ERROR;
	}
	
	HAL_TIM_Base_Start_IT(&TimHandle);

//	TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig;
//	sBreakDeadTimeConfig.OffStateRunMode = TIM_OSSR_ENABLE;
//	sBreakDeadTimeConfig.OffStateIDLEMode = TIM_OSSI_ENABLE;
//	sBreakDeadTimeConfig.LockLevel = TIM_LOCKLEVEL_OFF;
//	sBreakDeadTimeConfig.DeadTime = 0xFF;
//	sBreakDeadTimeConfig.BreakState = TIM_BREAK_DISABLE;
//	sBreakDeadTimeConfig.BreakPolarity = TIM_BREAKPOLARITY_HIGH;
//	sBreakDeadTimeConfig.AutomaticOutput = TIM_AUTOMATICOUTPUT_ENABLE;
//	if (HAL_TIMEx_ConfigBreakDeadTime(&TimHandle, &sBreakDeadTimeConfig) != HAL_OK)
//	{
//		return RESULT_ERROR;
//	}
  
	/*##-2- Configure the PWM channels #########################################*/
	/* Common configuration for all channels */
	TIM_OC_InitTypeDef sConfig;

	sConfig.OCMode       = TIM_OCMODE_PWM1;
	sConfig.OCPolarity   = TIM_OCPOLARITY_HIGH;
	sConfig.OCFastMode   = TIM_OCFAST_DISABLE;
	sConfig.OCNPolarity  = TIM_OCNPOLARITY_HIGH;
	sConfig.OCNIdleState = TIM_OCNIDLESTATE_RESET;
	sConfig.OCIdleState  = TIM_OCIDLESTATE_RESET;

	/* Set the pulse value for channel 1 */
	sConfig.Pulse = 0;
	if (HAL_TIM_PWM_ConfigChannel(&TimHandle, &sConfig, TIM_CHANNEL_1) != HAL_OK)
	{
		return RESULT_ERROR;
	}

	/* Set the pulse value for channel 2 */
	sConfig.Pulse = 0;
	if (HAL_TIM_PWM_ConfigChannel(&TimHandle, &sConfig, TIM_CHANNEL_2) != HAL_OK)
	{
		return RESULT_ERROR;
	}
	
	sConfig.OCMode = TIM_OCMODE_PWM2;
	sConfig.Pulse  = PWM_NS_TO_72MHZ_CNT(pwm_pin21_pulse_ns);
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
//	if (HAL_TIMEx_PWMN_Start(&TimHandle, TIM_CHANNEL_1)!= HAL_OK)
//	{
//		return RESULT_ERROR;
//	}
	if (HAL_TIM_PWM_Start(&TimHandle, TIM_CHANNEL_2) != HAL_OK)
	{
		return RESULT_ERROR;
	}
	if (HAL_TIM_PWM_Start(&TimHandle, TIM_CHANNEL_4) != HAL_OK)
	{
		return RESULT_ERROR;
	}

	return RESULT_SUCCESS;
}
