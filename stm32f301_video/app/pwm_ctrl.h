
/** @brief    PWM控制模块
 *  @file     pwm_ctrl.h
 *  @author   yjsstk@163.com
 *  @version  v1.0
 *  @date     2020/04/19
 *  @note     PIN18,PIN19,PIN21的输出波形用PWM控制输出
 */

#include "config.h"
#include <stdint.h>

typedef void (*ppwm_interrupt_cb_t)(void *pcontent);

/** @brief   PWM中断回调函数注册
 *  @param   func[in] 回调函数
 *  @return  返回值 @see CONFIG_RESULT_T
 */
extern CONFIG_RESULT_T pwm_interrupt_cb_reg(ppwm_interrupt_cb_t func);

/** @brief   TIM1更新中断
 *  @param   无
 *  @return  无
 */ 
extern inline void pwm_tim1_up_interrupt_handler(void);

/** @brief   PIN18,PIN19停止PWM输出
 *  @param   无
 *  @return  无
 */
extern void pwm_pin18_pin19_stop(void);

/** @brief   PIN18,PIN19开始PWM输出
 *  @param   cycle_ns[in] ：PWM周期
 *  @param   pin18_pulse_ns[in] ：高电平时间
 *  @param   pin19_pulse_ns[in] ：高电平时间
 *  @param   pin21_pulse_ns[in] ：低电平时间
 *  @return  无
 */
extern void pwm_set_output(
	uint32_t cycle_ns, 
	uint32_t pin18_pulse_ns, 
	uint32_t pin19_pulse_ns, 
	uint32_t pin21_pulse_ns);
	
/** @brief   该模块的应用初始化函数 
 *  @param   无 
 *  @return  返回值 @see CONFIG_RESULT_T
 *  @note    使用该模块的其他功能前调用以使初始化模块
 */
extern CONFIG_RESULT_T pwm_ctrl_init(void);
