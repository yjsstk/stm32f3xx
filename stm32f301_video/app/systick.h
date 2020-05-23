
/** @brief    SYSTICK定时器
 *  @file     systick.h
 *  @author   yjsstk@163.com
 *  @version  v1.0
 *  @date     2020/04/18
 */

#ifndef _SYSTICK_H_
#define _SYSTICK_H_

#include "config.h"

#define SYSTICK_MAX_FUNC_REG   8

typedef void (*psystick_1ms_cb_t)(void *pcontent);

/** @brief   SYSTICK中断调用函数
 *  @param   无
 *  @return  无
 */
extern inline void systick_interrupt_callback(void);

/** @brief   1MS定时回调注册
 *  @param   func[in] 回调函数
 *  @return  返回值 @see CONFIG_RESULT_T
 */
extern CONFIG_RESULT_T systick_1ms_cb_reg(psystick_1ms_cb_t func);

#endif
