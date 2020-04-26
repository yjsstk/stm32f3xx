
/** @brief    SYSTICK定时器
 *  @file     systick.c
 *  @author   yjsstk@163.com
 *  @version  v1.0
 *  @date     2020/04/18
 *  @note     SYSTICK在系统调用HAL_Init()时已默认初始化为1MS的定时中断
 */

#if (CONFIG_DEBUG_EN == 1)
#define DEBUG_INFO_EN       1
#define DEBUG_MODULE_NAME   "systick"
#endif
#include "debug.h"

#include <stdint.h>
#include "systick.h"

static psystick_1ms_cb_t systick_cb_list[SYSTICK_MAX_FUNC_REG]={0};

/** @brief   SYSTICK中断调用函数
 *  @param   无
 *  @return  无
 */
inline void systick_interrupt_callback(void)
{
	for (uint8_t i=0; i<SYSTICK_MAX_FUNC_REG; i++)
	{
		if (systick_cb_list[i] != NULL)
		{
			systick_cb_list[i](NULL);
		}
	}
}

/** @brief   1MS定时回调注册
 *  @param   func[in] 回调函数
 *  @return  返回值 @see CONFIG_RESULT_T
 */
CONFIG_RESULT_T systick_1ms_cb_reg(psystick_1ms_cb_t func)
{
	for (uint8_t i=0; i<SYSTICK_MAX_FUNC_REG; i++)
	{
		if (systick_cb_list[i] == NULL)
		{
			systick_cb_list[i] = func;
			return RESULT_SUCCESS;
		}
	}
	DEBUG_INFO("systick_1ms_cb_reg err");
	return RESULT_FULL;
}

