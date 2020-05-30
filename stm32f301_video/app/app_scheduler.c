
/** @brief    调度器
 *  @file     app_scheduler.c
 *  @author   yjsstk@163.com
 *  @version  v1.0
 *  @date     2020/04/26
 */

#include "config.h"
#if (CONFIG_DEBUG_EN == 1)
#define DEBUG_INFO_EN       1
#define DEBUG_MODULE_NAME   "app_scheduler"
#endif
#include "debug.h"

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "cmsis_armcc.h"
#include "app_scheduler.h"

typedef struct
{
	papp_scheduler_cb_t pfunc;
	uint16_t arg_size;
	uint8_t  arg_buff[CONFIG_APP_SCHEDULER_EVENT_SIZE];
}app_scheduler_msg_t;

static app_scheduler_msg_t app_scheduler_msg[CONFIG_APP_SCHEDULER_EVENT+1];

static uint16_t app_scheduler_write=0;
static uint16_t app_scheduler_read=0;

/** @brief   获取调度器中事件个数
 *  @param   无
 *  @return  无
 */
static uint16_t app_scheduler_get_event_numb(void) 
{
	uint16_t event_numb, read, write;
	
	read  = app_scheduler_read;
	write = app_scheduler_write;
	
	if (write == read)
	{
		event_numb = 0;
	}
	else if (write > read)
	{
		event_numb = write - read;
	}
	else
	{
		event_numb = write + CONFIG_APP_SCHEDULER_EVENT - read + 1;
	}
	
	return event_numb;
}

/** @brief   获取下一个索引值
 *  @param   index[in]：当前索引
 *  @return  下一个索引值
 */
static inline uint16_t app_scheduler_next_index(uint16_t index) 
{
	return (index + 1 > CONFIG_APP_SCHEDULER_EVENT ? 0 : index + 1);
}

/** @brief   判断调度器是否满
 *  @param   无
 *  @return  true：已满   false：未满 
 */
static inline bool app_scheduler_is_full(void) 
{
	uint8_t temp = app_scheduler_next_index(app_scheduler_write);
	return (temp == app_scheduler_read);
}

/** @brief   判断调度器是否空
 *  @param   无
 *  @return  true：空  false：非空
 */
static inline bool app_scheduler_is_empty(void) 
{
	return (app_scheduler_write == app_scheduler_read);
}

/** @brief   压入调度器
 *  @param   func[in]:要执行的函数
 *  @param   parg[in]:传给函数的参数
 *  @param   arg_size[in]: 参数的大小
 *  @return  true：空  false：非空
 */
CONFIG_RESULT_T app_scheduler_put(
	papp_scheduler_cb_t func, 
	void *parg, 
	uint16_t arg_size
) 
{
	if (func == NULL)
	{
		return RESULT_NULL;
	}
	if (arg_size > CONFIG_APP_SCHEDULER_EVENT_SIZE)
	{
		DEBUG_INFO("arg_size err");
		return RESULT_INVALID_SIZE;
	}

	uint16_t index = UINT16_MAX;
	__set_PRIMASK(1);
	if (app_scheduler_is_full() == false)
	{
		index = app_scheduler_write;
		app_scheduler_write = app_scheduler_next_index(app_scheduler_write);
	}
	__set_PRIMASK(0);
	if (index == UINT16_MAX)
	{
		DEBUG_INFO("app scheduler is full");
		return RESULT_FULL;
	}
	
	app_scheduler_msg[index].pfunc = func;
	if (arg_size == 0 || parg == NULL)
	{
		app_scheduler_msg[index].arg_size = 0;
	}
	else
	{
		app_scheduler_msg[index].arg_size = arg_size;
		memcpy(app_scheduler_msg[index].arg_buff, parg, arg_size);
	}
	return RESULT_SUCCESS;
}

/** @brief   获取调度器中的信息
 *  @param   pscheduler[out]:调度器内容
 *  @return  true：空  false：非空
 */
CONFIG_RESULT_T app_scheduler_get(app_scheduler_msg_t **ppscheduler)
{
	if (app_scheduler_is_empty() == true)
	{
		return RESULT_EMPTY;
	}
	*ppscheduler = &app_scheduler_msg[app_scheduler_read];
	app_scheduler_read = app_scheduler_next_index(app_scheduler_read);
	
	return RESULT_SUCCESS;
}

/** @brief   调度器执行
 *  @param   无
 *  @return  无
 */
void app_scheduler_exe(void)
{
	app_scheduler_msg_t *pscheduler;
	while(app_scheduler_get(&pscheduler) == RESULT_SUCCESS)
	{
		pscheduler->pfunc(pscheduler->arg_buff, pscheduler->arg_size);
	}
}

/** @brief   调度器初始化
 *  @param   无
 *  @return  返回值 @see CONFIG_RESULT_T
 */
CONFIG_RESULT_T app_scheduler_init(void)
{
	return RESULT_SUCCESS;
}



