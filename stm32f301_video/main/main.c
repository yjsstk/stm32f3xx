
/** @brief    视频信号处理项目
 *  @file     main.c
 *  @author   yjsstk@163.com
 *  @version  v1.0
 *  @date     2020/04/16
 *  @note     KEIL 5.28开发平台，STM32F301主控，72MHz
 */
 
#include "config.h"
#if (CONFIG_DEBUG_EN == 1)
#define DEBUG_INFO_EN       1
#define DEBUG_MODULE_NAME   "main"
#endif
#include "debug.h"

#include "stm32f3xx_hal.h"
#include "watchdog.h"
#include "rssi_signal_ctrl.h"
#include "sync_head_ctrl.h"
#include "pin30_ctrl.h"
#include "pin12_ctrl.h"
#include "pin25_ctrl.h"
#include "pin24_ctrl.h"
#include "pwm_ctrl.h"
#include "adc_rssi.h"
#include "pin27_ctrl.h"
#include "pin22_ctrl.h"
#include "systick.h"

/**
  * @brief  System Clock Configuration
  *         The system Clock is configured as follow : 
  *            System Clock source            = PLL (HSE)
  *            SYSCLK(Hz)                     = 72000000
  *            HCLK(Hz)                       = 72000000
  *            AHB Prescaler                  = 1
  *            APB1 Prescaler                 = 2
  *            APB2 Prescaler                 = 1
  *            HSE Frequency(Hz)              = 8000000
  *            HSE PREDIV                     = 1
  *            PLLMUL                         = RCC_PLL_MUL9 (9)
  *            Flash Latency(WS)              = 2
  * @param  None
  * @retval None
  */
void SystemClock_Config(void)
{
	RCC_ClkInitTypeDef RCC_ClkInitStruct;
	RCC_OscInitTypeDef RCC_OscInitStruct;

	/* Enable HSE Oscillator and activate PLL with HSE as source */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
	RCC_OscInitStruct.HSEState = RCC_HSE_ON;
	RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
	RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL3;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct)!= HAL_OK)
	{
		/* Initialization Error */
		while(1); 
	}
		
	/* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2 
	 clocks dividers */
	RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;  
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2)!= HAL_OK)
	{
		/* Initialization Error */
		while(1); 
	}
}

static uint16_t main_run_count=0;
void systic_test(void *pcontent)
{
	main_run_count++;
}

int main(void)
{
	HAL_Init();
	
	/* Configure the system clock to 72 MHz */
//    SystemClock_Config();
	
	#if (CONFIG_DEBUG_EN == 1)
	debug_init();
	#endif
	
	DEBUG_INFO("***** main run *****");
	DEBUG_INFO("project name: %s", CONFIG_PROJECT_NAME);
	DEBUG_INFO("version: %d", CONFIG_VERSION);
	
	pin12_ctrl_init();
	pin30_ctrl_init();
	pin25_ctrl_init();
	pin24_ctrl_init();
	pin27_ctrl_init();
	pin22_ctrl_init();
	rssi_signal_ctrl_init();
	sync_head_ctrl_init();
	pwm_ctrl_init();
	adc_init();
	
	#if (CONFIG_WATCHDOG_EN == 1)
	watchdog_init();
	#endif
	
	systick_1ms_cb_reg(systic_test);
	
	while (1)
	{
		rssi_event_loop();
		
		#if (CONFIG_WATCHDOG_EN == 1)
		watchdog_feed();
		#endif
		
		if (main_run_count >= 1000)
		{
			main_run_count = 0;
			DEBUG_PRINTF(".");
		}
	}
}











