
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

#define RSSI_BUFFER_LEN              2        // MAX 255
#define RSSI_VIDEO_DELAY_SWITCH_MS   1000     // 视频延时切换时间(MS) max: 0xFFFF
#define RSSI_MAX_LIMIT_VCC           0.9      // 设定RSSI最大限定电压，单位V, MAX: 3.3v
#define RSSI_DEFAULT_VCC             0.65     // 设定RSSI默认电压，单位V, MAX: 3.3v
#define RSSI_NORMAL_UP_VCC           0.2      // 比校准值高出的电压。单位V

#define RSSI_VCC_TO_ADC(VCC)         ((VCC) * 4096 / 3.3)


/** @brief   获取RSSI信号判断状态
 *  @param   无 
 *  @return  true  ：两路RSSI信号都低于RSSI_1V_VALUE
 *  @return  false ：至少有一路RSSI信号高于RSSI_1V_VALUE
 */
extern bool rssi_get_is_both_weak(float VCC);

/** @brief   该模块的应用初始化函数 
 *  @param   无 
 *  @return  返回值 @see CONFIG_RESULT_T
 *  @note    使用该模块的其他功能前调用以使初始化模块
 */
extern CONFIG_RESULT_T rssi_signal_ctrl_init(void);

#endif


