
/** @brief    跟据PIN25配置为外啊中断输入
 *  @file     pin25_ctrl.c
 *  @author   yjsstk@163.com
 *  @version  v1.0
 *  @date     2020/04/17
 *  @note     当MCU_25收到500MS低位时，而且两个RSSI电压小于0.9V时，MCU_24输出500MS高位
 *  @note     有其中一个RSSI电压大于0.9V时，MCU_24不能输出500MS高位
 */

#include "pin25_ctrl.h"
#include "stm32f3xx_hal.h"
#include "stm32f3xx_hal_gpio.h"

/** @brief   中断回调函数
 *  @param   pcontent[in] 
 *  @return  无
 *  @note    
 */
void pin25_exti15_callback(void *pcontent)
{
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
	GPIO_InitStructure.Pull = GPIO_NOPULL;
	GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);

	HAL_NVIC_SetPriority(EXTI15_10_IRQn, 2, 0);
	HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
	
	return RESULT_SUCCESS;
}



