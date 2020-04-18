
/** @brief    PIN24高低电位输出
 *  @file     pin24_ctrl.c
 *  @author   yjsstk@163.com
 *  @version  v1.0
 *  @date     2020/04/17
 *  @note     当MCU_25收到500MS低位时，而且两个RSSI电压小于0.9V时，MCU_24输出500MS高位
 *  @note     有其中一个RSSI电压大于0.9V时，MCU_24不能输出500MS高位
 */

#ifndef _PIN24_CTRL_H_
#define _PIN24_CTRL_H_

#include "config.h"
#include "stm32f3xx_hal.h"
#include "stm32f3xx_hal_gpio.h"

/** @brief   GPIO输出控制
 *  @param   state[in] @see GPIO_PinState
 *  @return  无
 */
extern inline void pin24_output_ctrl(GPIO_PinState pin_state);

/** @brief   该模块的应用初始化函数 
 *  @param   无 
 *  @return  返回值 @see CONFIG_RESULT_T
 *  @note    使用该模块的其他功能前调用以使初始化模块
 */
extern CONFIG_RESULT_T pin24_ctrl_init(void);

#endif
