
/** @brief    跟据RSSI信号选择视频信号
 *  @file     rssi_signal_ctrl.h
 *  @author   yjsstk@163.com
 *  @version  v1.0
 *  @date     2020/04/16
 *  @note     通过对两路RSSI信号的强度来选择控制视频信号的输出
 */
 
#ifndef _RSSI_SIGNAL_CTRL_H_
#define _RSSI_SIGNAL_CTRL_H_

#include "config.h"
#include <stdbool.h>

#define RSSI_BUFFER_LEN     5        // MAX 255
#define RSSI_1V_VALUE       1000     // 1V对应的值

/** @brief   获取RSSI信号判断状态
 *  @param   无 
 *  @return  true  ：两路RSSI信号都低于RSSI_1V_VALUE
 *  @return  false ：至少有一路RSSI信号高于RSSI_1V_VALUE
 */
extern bool rssi_get_is_both_weak(void);

/** @brief   该模块的应用初始化函数 
 *  @param   无 
 *  @return  返回值 @see CONFIG_RESULT_T
 *  @note    使用该模块的其他功能前调用以使初始化模块
 */
extern CONFIG_RESULT_T rssi_signal_ctrl_init(void);

/** @brief   RSSI事件循环处理函数
 *  @param   无 
 *  @return  无
 *  @note    函数放在MAIN的主循环中调用
 */
extern void rssi_event_loop(void);

#endif


