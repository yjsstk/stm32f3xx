
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

int main(void)
{
	HAL_Init();
	
	#if (CONFIG_DEBUG_EN == 1)
	debug_init();
	#endif
	
	DEBUG_INFO("*****main run*****");
	DEBUG_INFO("project name: %s", CONFIG_PROJECT_NAME);
	DEBUG_INFO("version: %d", CONFIG_VERSION);
	
	pin12_ctrl_init();
	pin30_ctrl_init();
	pin25_ctrl_init();
	pin24_ctrl_init();
	rssi_signal_ctrl_init();
	sync_head_ctrl_init();
	
	#if (CONFIG_WATCHDOG_EN == 1)
	watchdog_init();
	#endif
	while (1)
	{
		rssi_event_loop();
		
		#if (CONFIG_WATCHDOG_EN == 1)
		watchdog_feed();
		#endif
	}
}











