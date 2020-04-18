
/** @brief    跟据PIN25及RSSI的状态控制PIN24的输出
 *  @file     pin25_ctrl.c
 *  @author   yjsstk@163.com
 *  @version  v1.0
 *  @date     2020/04/17
 *  @note     当MCU_25收到500MS低位时，而且两个RSSI电压小于0.9V时，MCU_24输出500MS高位
 *  @note     有其中一个RSSI电压大于0.9V时，MCU_24不能输出500MS高位
 */

#ifndef _PIN25_CTRL_H_
#define _PIN25_CTRL_H_

#include "config.h"

#define  PIN25_MS_COUNT      500    // MS计时值  
#define  PIN25_COUNT_ERR     10     // MS计时误差

/** @brief   中断回调函数
 *  @param   pcontent[in] 
 *  @return  无
 *  @note    
 */
extern void pin25_exti15_interrupt_handler(void *pcontent);

/** @brief   该模块的应用初始化函数 
 *  @param   无 
 *  @return  返回值 @see CONFIG_RESULT_T
 *  @note    使用该模块的其他功能前调用以使初始化模块
 */
extern CONFIG_RESULT_T pin25_ctrl_init(void);

#endif
