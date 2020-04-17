
/** @brief    同步头信号处理
 *  @file     sync_head_ctrl.h
 *  @author   yjsstk@163.com
 *  @version  v1.0
 *  @date     2020/04/17
 *  @note     跟据检测的PIN14引脚信息来控制视频同步头输出
 */

#ifndef _SYNC_HEAD_CTRL_H_
#define _SYNC_HEAD_CTRL_H_

#include "config.h"

/** @brief   该模块的应用初始化函数 
 *  @param   无 
 *  @return  返回值 @see CONFIG_RESULT_T
 *  @note    使用该模块的其他功能前调用以使初始化模块
 */
extern CONFIG_RESULT_T sync_head_ctrl_init(void);

#endif





