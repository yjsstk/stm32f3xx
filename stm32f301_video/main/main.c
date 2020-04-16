
/** @brief    视频信号处理项目
 *  @file     main.c
 *  @author   yjsstk@163.com
 *  @version  v1.0
 *  @date     2020/04/16
 *  @note     KEIL 5.28开发平台，STM32F301主控，72MHz
 */
 
#include "stm32f301x8.h"
#include "config.h"
#include "rssi_signal_ctrl.h"

int main(void)
{
	
	rssi_signal_ctrl_init();
	
	while (1)
	{
	}
}











