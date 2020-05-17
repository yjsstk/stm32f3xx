/**
  ******************************************************************************
  * File Name          : TIM.c
  * Description        : This file provides code for the configuration
  *                      of the TIM instances.
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

/* Includes ------------------------------------------------------------------*/
#include "tim.h"

/* USER CODE BEGIN 0 */
#include "debug.h"
#include "stdbool.h"
/* USER CODE END 0 */

TIM_HandleTypeDef htim2;

/* TIM2 init function */
void MX_TIM2_Init(void)
{
  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_IC_InitTypeDef sConfigIC = {0};

  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 0;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 0xFFFFFFFF; // @note 固定为0xFFFFFFFF 不允许修改
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_IC_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_FALLING;
  sConfigIC.ICSelection = TIM_ICSELECTION_DIRECTTI;
  sConfigIC.ICPrescaler = TIM_ICPSC_DIV1;
  sConfigIC.ICFilter = 3;//3
  if (HAL_TIM_IC_ConfigChannel(&htim2, &sConfigIC, TIM_CHANNEL_4) != HAL_OK)
  {
    Error_Handler();
  }
  
  HAL_TIM_IC_Start(&htim2, TIM_CHANNEL_4);
  HAL_TIM_IC_Start_IT(&htim2, TIM_CHANNEL_4);
  HAL_TIM_Base_Start_IT(&htim2);

}

void HAL_TIM_Base_MspInit(TIM_HandleTypeDef* tim_baseHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(tim_baseHandle->Instance==TIM2)
  {
  /* USER CODE BEGIN TIM2_MspInit 0 */

  /* USER CODE END TIM2_MspInit 0 */
    /* TIM2 clock enable */
    __HAL_RCC_TIM2_CLK_ENABLE();
  
    __HAL_RCC_GPIOA_CLK_ENABLE();
		

    /**TIM2 GPIO Configuration    
    PA3     ------> TIM2_CH4 
    */
    GPIO_InitStruct.Pin = GPIO_PIN_3;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF1_TIM2;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);


    /* TIM2 interrupt Init */
    HAL_NVIC_SetPriority(TIM2_IRQn, 1, 0);
    HAL_NVIC_EnableIRQ(TIM2_IRQn);
  /* USER CODE BEGIN TIM2_MspInit 1 */

  /* USER CODE END TIM2_MspInit 1 */
  }
}

void HAL_TIM_Base_MspDeInit(TIM_HandleTypeDef* tim_baseHandle)
{

  if(tim_baseHandle->Instance==TIM2)
  {
  /* USER CODE BEGIN TIM2_MspDeInit 0 */

  /* USER CODE END TIM2_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_TIM2_CLK_DISABLE();
  
    /**TIM2 GPIO Configuration    
    PA3     ------> TIM2_CH4 
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_3);


    /* TIM2 interrupt Deinit */
    HAL_NVIC_DisableIRQ(TIM2_IRQn);
  /* USER CODE BEGIN TIM2_MspDeInit 1 */

  /* USER CODE END TIM2_MspDeInit 1 */
  }
} 

/* USER CODE BEGIN 1 */


// 场周期
#define FIELD_CYCLE_50HZ_US         (20000ul)
#define FIELD_CYCLE_60HZ_US         (16667ul)
#define FIELD_CYCLE_TOLERANCE_US    (10)  // @note must <32
#define TICK_TO_US(n)                ( (n) / 72ul) // TODO

void (*m_capture_fied_cycle_evt_handler)(filed_cycle_t filed_cycle) = NULL;

// @brief 场周期捕获事件注册
void tim_field_cycle_capture_evt_reg(void (*evt)(filed_cycle_t filed_cycle))
{
    m_capture_fied_cycle_evt_handler = evt;
}

// @brief 检验场周期
static inline filed_cycle_t check_field_cycle(uint32_t cycle_us)
{
    if ( cycle_us >= FIELD_CYCLE_60HZ_US - FIELD_CYCLE_TOLERANCE_US ||
         cycle_us <= FIELD_CYCLE_60HZ_US + FIELD_CYCLE_TOLERANCE_US )
    {
        DEBUG_INFO("<check_field_cycle> FILED_CYCLE_60HZ_ODD");
         
        return FILED_CYCLE_60HZ_ODD;
    }
    else if ( cycle_us >= FIELD_CYCLE_60HZ_US + 64 - FIELD_CYCLE_TOLERANCE_US ||
              cycle_us <= FIELD_CYCLE_60HZ_US + 64 + FIELD_CYCLE_TOLERANCE_US )
    {
        DEBUG_INFO("<check_field_cycle> FILED_CYCLE_60HZ_EVEN");
         
        return FILED_CYCLE_60HZ_EVEN;
    }
    else if ( cycle_us >= FIELD_CYCLE_50HZ_US - FIELD_CYCLE_TOLERANCE_US ||
              cycle_us <= FIELD_CYCLE_50HZ_US + FIELD_CYCLE_TOLERANCE_US )
    {
        DEBUG_INFO("<check_field_cycle> FILED_CYCLE_50HZ_ODD");
        
        return FILED_CYCLE_50HZ_ODD;
    }
    else if ( cycle_us >= FIELD_CYCLE_50HZ_US + 64 - FIELD_CYCLE_TOLERANCE_US ||
              cycle_us <= FIELD_CYCLE_50HZ_US + 64 + FIELD_CYCLE_TOLERANCE_US )
    {
        DEBUG_INFO("<check_field_cycle> FILED_CYCLE_50HZ_EVEN");
        
        return FILED_CYCLE_50HZ_EVEN;
    }

    DEBUG_INFO("<check_field_cycle> FILED_CYCLE_UNKNOWN");
              
    return FILED_CYCLE_UNKNOWN;
}


static uint32_t m_time_base_cycle_count;  // 时间基定时器走过的周期数


// @brief 是否捕获到同步头
static inline void capture_sync_head(uint32_t time_tick, void (*has_capture_head_evt)(uint32_t timestamp))
{
    static uint8_t capture_times = 0;
    static uint8_t delay_tick = 0;
    static bool enable = false;
    static uint32_t pre_time_tick = 0;
    uint32_t cycle_us = TICK_TO_US(time_tick - pre_time_tick);

    if (enable == true)
    {
        if (cycle_us >= 32 - 2 && cycle_us <= 32 + 2)
        {
            capture_times ++;
        }
        else
        {
            capture_times = 0;
        }

        if (capture_times >= 18)
        {
            capture_times = 0;
            enable = false;
            delay_tick = 0;
            if (has_capture_head_evt != NULL)
            {
                has_capture_head_evt(time_tick);
            }
        }
    }
    else
    {
        if (delay_tick < 20)
        {
            if ( ++ delay_tick >= 20)
            {
                enable = true;
            }
        }
    }

    pre_time_tick = time_tick;
}

// @brief 捕获到场同步头事件
static inline void capture_head_evt_handler(uint32_t time_tick)
{
    uint32_t count;
    static uint32_t pre_time_tick = 0;

    count = m_time_base_cycle_count;
    m_time_base_cycle_count = 0;
    

    if (count > 1)
    {
        DEBUG_INFO("<capture_head_evt_handler> out time capture");

        // 
        // 本场与上场之间超时
        // 视为无效场周期
        //
        pre_time_tick = time_tick;

        return ;
    }

    if (m_capture_fied_cycle_evt_handler != NULL)
    {
       m_capture_fied_cycle_evt_handler( check_field_cycle(TICK_TO_US(time_tick - pre_time_tick)) );
    }

    pre_time_tick = time_tick;
}

uint32_t g_cap_value[100] = {0};
static uint16_t m_cap_index = 0;
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
		if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_4)
		{
            capture_sync_head(htim->Instance->CCR4, capture_head_evt_handler);
            
            m_cap_index ++;
            if (m_cap_index >= 100)
            {
                m_cap_index = 0;
            }
            g_cap_value[m_cap_index] = htim->Instance->CCR4;

		}
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    // static uint8_t tick = 0;
    
    // HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, tick++ & 1);
    m_time_base_cycle_count ++;
    
}

/* USER CODE END 1 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
