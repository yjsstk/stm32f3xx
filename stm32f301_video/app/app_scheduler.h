
/** @brief    调度器
 *  @file     app_scheduler.h
 *  @author   yjsstk@163.com
 *  @version  v1.0
 *  @date     2020/04/26
 */

#ifndef _APP_SCHEDULER_H_
#define _APP_SCHEDULER_H_

#include "config.h"
#include <stdint.h>

typedef void (*papp_scheduler_cb_t)(void *parg, uint16_t arg_size);

/** @brief   压入调度器
 *  @param   无
 *  @return  true：空  false：非空
 */
extern CONFIG_RESULT_T app_scheduler_put(
	papp_scheduler_cb_t func, 
	void *parg, 
	uint16_t arg_size
);

/** @brief   调度器执行
 *  @param   无
 *  @return  无
 */
extern void app_scheduler_exe(void);
	
/** @brief   调度器初始化
 *  @param   无
 *  @return  返回值 @see CONFIG_RESULT_T
 */
extern CONFIG_RESULT_T app_scheduler_init(void);


#endif
