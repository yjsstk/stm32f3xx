
/** @brief    PIN12根据RSSI值选择视频输出
 *  @file     pin12_ctrl.h
 *  @author   yjsstk@163.com
 *  @version  v1.0
 *  @date     2020/04/17
 *  @note     视频输出由软件控制时可选择V1或V2做为输出
 */

#ifndef _PIN12_CTRL_H_
#define _PIN12_CTRL_H_

#include "config.h"

typedef enum
{
	PIN12_SEL_VI1_TOP,
	PIN12_SEL_VI2_BOT,
}PIN12_VIDEO_SEL_T;

/** @brief   设置视频选择控制
 *  @param   sel[in] @see PIN12_VIDEO_SEL_T
 *  @return  无 
 *  @note    PIN12视频控制选择
 */
extern void pin12_set_video_ctrl(PIN12_VIDEO_SEL_T sel);

/** @brief   该模块的应用初始化函数 
 *  @param   无 
 *  @return  返回值 @see CONFIG_RESULT_T
 *  @note    使用该模块的其他功能前调用以使初始化模块
 */
extern CONFIG_RESULT_T pin12_ctrl_init(void);

#endif
