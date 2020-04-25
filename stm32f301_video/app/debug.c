
/** @brief    DEBUG串口调试功能
 *  @file     debug.c
 *  @author   yjsstk@163.com
 *  @version  v1.0
 *  @date     2020/04/17
 *  @note     使用串口输出做DEBUG调试功能
 */

#include "debug.h"

#if (CONFIG_DEBUG_EN == 1)

#include "stm32f3xx_hal.h"
#include "stm32f3xx_hal_usart.h"

static UART_HandleTypeDef UartHandle;

/** @brief   发送数据
 *  @param   
 *  @return  
 */
int fputc(int ch, FILE *f)
{
	UartHandle.Instance->TDR = ch;
    while (__HAL_UART_GET_FLAG(&UartHandle, UART_FLAG_TXE) == RESET); 
	return ch;
}

/** @brief   串口所对应使用的GPIO口初始化配置
 *  @param   无 
 *  @return  无
 */
static void debug_uart_gpio_init(void)
{  
	GPIO_InitTypeDef  GPIO_InitStruct;
  
	/*##-1- Enable peripherals and GPIO Clocks #################################*/
	/* Enable GPIO TX/RX clock */
	__HAL_RCC_GPIOB_CLK_ENABLE();
  
	/*##-2- Configure peripheral GPIO ##########################################*/  
	/* UART TX GPIO pin configuration  */
	GPIO_InitStruct.Pin       = GPIO_PIN_6;
	GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull      = GPIO_PULLUP;
	GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_HIGH;
	GPIO_InitStruct.Alternate = GPIO_AF7_USART1;

	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}

/** @brief   该模块的应用初始化函数 
 *  @param   无 
 *  @return  返回值 @see CONFIG_RESULT_T
 *  @note    使用该模块的其他功能前调用以使初始化模块
 */
CONFIG_RESULT_T debug_init(void)
{
	debug_uart_gpio_init();
	
	__HAL_RCC_USART1_CLK_ENABLE();
	
	UartHandle.Instance          = USART1;

	UartHandle.Init.BaudRate     = 115200;
	UartHandle.Init.WordLength   = UART_WORDLENGTH_8B;
	UartHandle.Init.StopBits     = UART_STOPBITS_1;
	UartHandle.Init.Parity       = UART_PARITY_NONE;
	UartHandle.Init.HwFlowCtl    = UART_HWCONTROL_NONE;
	UartHandle.Init.Mode         = UART_MODE_TX;
	UartHandle.Init.OverSampling = UART_OVERSAMPLING_16;
	UartHandle.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;

	if(HAL_UART_DeInit(&UartHandle) != HAL_OK)
	{
	}  
	if(HAL_UART_Init(&UartHandle) != HAL_OK)
	{
	}
	
	__HAL_UART_ENABLE(&UartHandle);
	
	return RESULT_SUCCESS;
}


#endif /* #if (CONFIG_DEBUG_EN == 1) */





