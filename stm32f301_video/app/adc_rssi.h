
/** @brief    RSSI信号的ADC采样控制
 *  @file     adc_rssi.h
 *  @author   yjsstk@163.com
 *  @version  v1.0
 *  @date     2020/04/19
 */
 
#ifndef _ADC_RSSI_H_
#define _ADC_RSSI_H_

#include "config.h"
#include <stdint.h>

#define ADC_CHANNEL_NUMB     2
#define ADC_DMA_BUFF_LEN     8

typedef void (*padc_value_change_cb_t)(uint16_t channel1, uint16_t channel2);

/** @brief   注册ADC值改变回调函数
 *  @param   func[in] 
 *  @return  无
 *  @note    
 */
extern void adc_value_change_reg(padc_value_change_cb_t func);
	
/** @brief   该模块的应用初始化函数 
 *  @param   无 
 *  @return  返回值 @see CONFIG_RESULT_T
 *  @note    使用该模块的其他功能前调用以使初始化模块
 */
extern CONFIG_RESULT_T adc_init(void);

extern void adc_dma_isr_cb(void);

#endif

