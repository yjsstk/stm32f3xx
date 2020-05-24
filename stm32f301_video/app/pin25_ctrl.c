
/** @brief    跟据PIN25配置为外啊中断输入
 *  @file     pin25_ctrl.c
 *  @author   yjsstk@163.com
 *  @version  v1.0
 *  @date     2020/04/17
 *  @note     当MCU_25收到500MS低位时，而且两个RSSI电压小于0.9V时，MCU_24输出500MS高位
 *  @note     有其中一个RSSI电压大于0.9V时，MCU_24不能输出500MS高位
 */

#include "config.h"
#if (CONFIG_DEBUG_EN == 1)
#define DEBUG_INFO_EN       0
#define DEBUG_MODULE_NAME   "PIN25"
#endif
#include "debug.h"
#include "pin25_ctrl.h"
#include "stm32f3xx_hal.h"
#include "stm32f3xx_hal_gpio.h"
#include "rssi_signal_ctrl.h"
#include "pin24_ctrl.h"
#include "systick.h"

static volatile uint16_t pin25_ms_count    = 0;
static volatile uint16_t pin25_ctrl_output = 0;

ppin25_calibration_cb_t pin25_calibration_cb=NULL;

/** @brief   校准通知回调函数注册
 *  @param   func[in] 回调函数
 *  @return  无
 *  @note    
 */
void pin25_calibration_cb_reg(ppin25_calibration_cb_t func)
{
	pin25_calibration_cb = func;
}

/** @brief   1MS回调函数
 *  @param   pcontent[in] 
 *  @return  无
 *  @note    
 */
static void pin25_1ms_callback(void *pcontent)
{
	pin25_ms_count++;
	
	if (pin25_ctrl_output < UINT16_MAX)
	{
		pin25_ctrl_output++;
		if (pin25_ctrl_output == PIN25_MS_COUNT)
		{
			pin24_output_ctrl(GPIO_PIN_RESET);
		}
	}
}

/** @brief   中断调用函数
 *  @param   pcontent[in] 
 *  @return  无
 *  @note    
 */
void pin25_exti15_interrupt_handler(void *pcontent)
{
	if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_15) == GPIO_PIN_RESET)
	{
		pin25_ms_count = 0;
		DEBUG_INFO("GPIO_PIN_25 is reset");
	}
	else 
	{
		if ((pin25_ms_count >= PIN25_MS_COUNT - PIN25_COUNT_ERR) 
		 && (pin25_ms_count <= PIN25_MS_COUNT + PIN25_COUNT_ERR))
		{
			if (rssi_get_is_both_weak(RSSI_MAX_LIMIT_VCC) == true)
			{
				DEBUG_INFO("GPIO_PIN_25 is set");
				pin25_ctrl_output = 0;
				pin24_output_ctrl(GPIO_PIN_SET);
				if (pin25_calibration_cb != NULL)
				{
					pin25_calibration_cb();
				}
			}
		}
	}
}

/** @brief   该模块的应用初始化函数 
 *  @param   无 
 *  @return  返回值 @see CONFIG_RESULT_T
 *  @note    使用该模块的其他功能前调用以使初始化模块
 */
CONFIG_RESULT_T pin25_ctrl_init(void)
{
	GPIO_InitTypeDef   GPIO_InitStructure;

	__HAL_RCC_GPIOA_CLK_ENABLE();

	GPIO_InitStructure.Pin = GPIO_PIN_15;
	GPIO_InitStructure.Mode = GPIO_MODE_IT_RISING_FALLING;
	GPIO_InitStructure.Pull = GPIO_PULLUP;
	GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);

	HAL_NVIC_SetPriority(EXTI15_10_IRQn, 2, 0);
	HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
	
	systick_1ms_cb_reg(pin25_1ms_callback);
	
	return RESULT_SUCCESS;
}



