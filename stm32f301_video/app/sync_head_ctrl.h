
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

// 当两个RSSI都低于1.0V时,PIN18,PIN19保持同步信号输出时间
#define  SYNC_KEEP_OUTPUT_MS              (5 * 1000)      

#define  SYNC_PIN19_PULSE_NS               10000     // ns，行同步信号脉冲

#define  SYNC_ROW_CYCLE_NS                 64000    // ns，行同步信号周期
#define  SYNC_ROW_PULSE_NS                 4700     // ns，行同步信号脉冲
#define  SYNC_SLOT_PULSE                   6        // 开槽脉冲个数

// PAL制视频每场共有312.5个行同步脉冲。在处理时分奇偶场处理，奇场312， 偶场313
#define	 SYNC_PAL_ROW_PULSE                312

// NTSC制视频每场共有262.5个行同步脉冲。在处理时分奇偶场处理，奇场262， 偶场263
#define	 SYNC_NTSC_ROW_PULSE               262

typedef enum
{
	SYNC_VIDEO_UNKNOWN,
	SYNC_VIDEO_PAL,
	SYNC_VIDEO_NTSC,
}sync_video_type_t;

typedef enum
{
	SYNC_FIELD_UNKNOWN,
	SYNC_FIELD_ODD,        // 奇场
	SYNC_FIELD_EVEN,       // 偶场
}sync_field_type_t;

/** @brief   该模块的应用初始化函数 
 *  @param   无 
 *  @return  返回值 @see CONFIG_RESULT_T
 *  @note    使用该模块的其他功能前调用以使初始化模块
 */
extern CONFIG_RESULT_T sync_head_ctrl_init(void);

#endif





