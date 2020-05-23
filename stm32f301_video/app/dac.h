/**
  ******************************************************************************
  * File Name          : DAC.h
  * Description        : This file provides code for the configuration
  *                      of the DAC instances.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __dac_H
#define __dac_H
#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

#define DAC_OUTPUT_VCC        0.55                   // DAC输出电压，单位V
#define DAC_VCC_TO_VAL(VCC)  ((VCC) * 4096 / 3.3)    // 设定电压值转为DAC的输出值

/** @brief   设置DAC输出电压值
 *  @param   vcc[in]: 输出的电压值, MAX: 3.3
 *  @return  无
 */
extern inline void dac_set_output_vcc(float vcc);
	
void MX_DAC_Init(void);

#ifdef __cplusplus
}
#endif
#endif /*__ dac_H */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
