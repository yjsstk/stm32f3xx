
/** @brief    视频输出信号由软件或硬件控制处理
 *  @file     pin30_ctrl.h
 *  @author   yjsstk@163.com
 *  @version  v1.0
 *  @date     2020/04/17
 *  @note     PIN30输出低，视频输出信息由硬件控制处理
 *  @note     PIN30输出高，视频输出信息由软件控制处理 
 *  @note     软件控制由PIN12控制
 */

#ifndef _PIN30_CTRL_H_
#define _PIN30_CTRL_H_

#include "config.h"

typedef enum
{
	PIN30_VIDEO_CTRL_BY_HW,
	PIN30_VIDEO_CTRL_BY_SW,
}PIN30_VIDEO_CTRL_T;

/** @brief   视频处理控制
 *  @param   ctrl[in] @see PIN30_VIDEO_CTRL_T
 *  @return  无 
 *  @note    PIN30视频处理选择
 */
extern void pin30_set_video_ctrl(PIN30_VIDEO_CTRL_T ctrl);
	
/** @brief   该模块的应用初始化函数 
 *  @param   无 
 *  @return  返回值 @see CONFIG_RESULT_T
 *  @note    使用该模块的其他功能前调用以使初始化模块
 */
extern CONFIG_RESULT_T pin30_ctrl_init(void);

#endif
