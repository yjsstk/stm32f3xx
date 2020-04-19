
/** @brief    PWM控制模块
 *  @file     pwm_ctrl.h
 *  @author   yjsstk@163.com
 *  @version  v1.0
 *  @date     2020/04/19
 *  @note     PIN18,PIN19,PIN21的输出波形用PWM控制输出
 */

#include "config.h"

typedef void (*ppwm_interrupt_cb_t)(void *pcontent);

/** @brief   PWM中断回调函数注册
 *  @param   func[in] 回调函数
 *  @return  返回值 @see CONFIG_RESULT_T
 */
extern CONFIG_RESULT_T pwm_interrupt_cb_reg(ppwm_interrupt_cb_t func);

/** @brief   该模块的应用初始化函数 
 *  @param   无 
 *  @return  返回值 @see CONFIG_RESULT_T
 *  @note    使用该模块的其他功能前调用以使初始化模块
 */
extern CONFIG_RESULT_T pwm_ctrl_init(void);
