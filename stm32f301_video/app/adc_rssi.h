
/** @brief    RSSI信号的ADC采样控制
 *  @file     adc_rssi.h
 *  @author   yjsstk@163.com
 *  @version  v1.0
 *  @date     2020/04/19
 */
 
#ifndef _ADC_RSSI_H_
#define _ADC_RSSI_H_

#include "config.h"

#define ADC_CHANNEL_NUMB     1
#define ADC_DMA_BUFF_LEN     5


/** @brief   该模块的应用初始化函数 
 *  @param   无 
 *  @return  返回值 @see CONFIG_RESULT_T
 *  @note    使用该模块的其他功能前调用以使初始化模块
 */
extern CONFIG_RESULT_T adc_init(void);



#endif

