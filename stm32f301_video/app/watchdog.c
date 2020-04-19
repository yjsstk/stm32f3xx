
/** @brief    看门狗功能模块
 *  @file     watchdog.c
 *  @author   yjsstk@163.com
 *  @version  v1.0
 *  @date     2020/04/17
 */
 
#include "watchdog.h"

#if (CONFIG_WATCHDOG_EN == 1)

#include "stm32f3xx_hal.h"
#include "stm32f3xx_hal_iwdg.h"

static IWDG_HandleTypeDef   IwdgHandle;


/** @brief   喂狗
 *  @param   无 
 *  @return  无
 *  @note    
 */
inline void watchdog_feed(void)
{
	if(HAL_IWDG_Refresh(&IwdgHandle) != HAL_OK)
    {
    }
}

/** @brief   该模块的应用初始化函数 
 *  @param   无 
 *  @return  返回值 @see CONFIG_RESULT_T
 *  @note    使用该模块的其他功能前调用以使初始化模块
 */
CONFIG_RESULT_T watchdog_init(void)
{
	/*##-1- Check if the system has resumed from IWDG reset ####################*/
	if (__HAL_RCC_GET_FLAG(RCC_FLAG_IWDGRST) != RESET)
	{
	}

	/* Clear reset flags in any cases */
	__HAL_RCC_CLEAR_RESET_FLAGS();
	
	IwdgHandle.Instance = IWDG;
	IwdgHandle.Init.Prescaler = IWDG_PRESCALER_32;
	IwdgHandle.Init.Reload = 3000;
	IwdgHandle.Init.Window = IWDG_WINDOW_DISABLE;
	
	if(HAL_IWDG_Init(&IwdgHandle) != HAL_OK)
	{
		return RESULT_ERROR;
	}
  
	return RESULT_SUCCESS;
}


#endif /* #if (CONFIG_WATCHDOG_EN == 1) */ 
