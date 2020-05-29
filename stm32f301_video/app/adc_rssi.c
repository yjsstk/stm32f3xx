
/** @brief    RSSI信号的ADC采样控制
 *  @file     adc_rssi.c
 *  @author   yjsstk@163.com
 *  @version  v1.0
 *  @date     2020/04/19
 */

#include "config.h"
#if (CONFIG_DEBUG_EN == 1)
#define DEBUG_INFO_EN       1
#define DEBUG_MODULE_NAME   "ADC"
#endif
#include "debug.h"

#include "adc_rssi.h"
#include "stm32f3xx_hal.h"
#include "stm32f3xx_hal_adc.h"
#include "stm32f3xx_hal_gpio.h"
#include "stm32f3xx_hal_def.h"
#include "app_scheduler.h"
#include <string.h>
#include <stdbool.h>
#include "systick.h"

static padc_value_change_cb_t padc_value_change_cb=NULL;

static ADC_HandleTypeDef  AdcHandle;
static DMA_HandleTypeDef  DmaHandle;

static uint16_t adc_dma_buff[ADC_CHANNEL_NUMB * ADC_DMA_BUFF_LEN];
static uint32_t adc_add_sum[ADC_CHANNEL_NUMB];

/** @brief   ADC值处理
 *  @param   hadc[in] 
 *  @return  无
 *  @note    
 */
static void adc_value_deal(void *parg, uint16_t size)
{
	static bool first_half = true;
	
	uint16_t *pbuff=NULL;
	if (first_half == true)
	{
		pbuff = &adc_dma_buff[0];
	}
	else
	{
		pbuff = &adc_dma_buff[ADC_DMA_BUFF_LEN / 2];
	}
	first_half = !first_half;
	
//	DEBUG_PRINTF("adc_dma_buff: ");
//	for (uint8_t n=0; n<ADC_DMA_BUFF_LEN; n++)
//	{
//		DEBUG_PRINTF("%d ", adc_dma_buff[n]);
//	}
//	DEBUG_PRINTF("\r\n");
	
	memset(adc_add_sum, 0, sizeof(adc_add_sum));
	for (uint16_t i=0; i<ADC_DMA_BUFF_LEN / 2; i++)
	{
		for (uint8_t j=0; j<ADC_CHANNEL_NUMB; j++)
		{
			adc_add_sum[j] += pbuff[2 * i + j];
		}
	}
	
	for (uint8_t k=0; k<ADC_CHANNEL_NUMB; k++)
	{
		adc_add_sum[k] = adc_add_sum[k] * 2 / ADC_DMA_BUFF_LEN;
	}
	
	if (padc_value_change_cb != NULL)
	{
		padc_value_change_cb(adc_add_sum[1], adc_add_sum[0]);
	}
}

/** @brief   DMA回调
 *  @param   hadc[in] 
 *  @return  无
 *  @note    
 */
void adc_dma_isr_cb(void)
{
	HAL_ADC_Stop_DMA(&AdcHandle);
	if (__HAL_DMA_GET_FLAG(&DmaHandle, DMA_FLAG_TE1) != DMA_FLAG_TE1)
	{
		app_scheduler_put(adc_value_deal, NULL, 0);
	}
	__HAL_DMA_CLEAR_FLAG(&DmaHandle, DMA_FLAG_TE1 | DMA_FLAG_TC1 | DMA_FLAG_HT1);
}

/** @brief   注册ADC值改变回调函数
 *  @param   func[in] 
 *  @return  无
 *  @note    
 */
void adc_value_change_reg(padc_value_change_cb_t func)
{
	padc_value_change_cb = func;
}

/** @brief   用于控制ADC采样时间
 *  @param   func[in] 
 *  @return  无
 *  @note    
 */
static void adc_sample_1ms_cb(void *pcontent)
{
	static uint8_t sample_time_ctrl=0;
	
	if (sample_time_ctrl++ >= 2)
	{
		sample_time_ctrl = 0;
		HAL_ADC_Start_DMA(&AdcHandle,
						  (uint32_t *)adc_dma_buff,
						   ADC_CHANNEL_NUMB * ADC_DMA_BUFF_LEN
						   );
	}
}

/** @brief   该模块的应用初始化函数 
 *  @param   无 
 *  @return  返回值 @see CONFIG_RESULT_T
 *  @note    使用该模块的其他功能前调用以使初始化模块
 */
CONFIG_RESULT_T adc_init(void)
{
	GPIO_InitTypeDef  GPIO_InitStruct;
	
	__HAL_RCC_GPIOA_CLK_ENABLE();
	GPIO_InitStruct.Pin   = GPIO_PIN_0 | GPIO_PIN_1;
	GPIO_InitStruct.Mode  = GPIO_MODE_ANALOG;
	GPIO_InitStruct.Pull  = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct); 
	
	__HAL_RCC_ADC1_CLK_ENABLE();
	__HAL_RCC_ADC1_CONFIG(RCC_ADC1PLLCLK_DIV1);

	/* ### - 1 - Initialize ADC peripheral #################################### */
	AdcHandle.Instance = ADC1;
	if (HAL_ADC_DeInit(&AdcHandle) != HAL_OK)
	{
		return RESULT_ERROR;
	}

	AdcHandle.Init.ClockPrescaler        = ADC_CLOCK_SYNC_PCLK_DIV1;      /* Synchronous clock mode, input ADC clock divided by 2*/
	AdcHandle.Init.Resolution            = ADC_RESOLUTION_12B;            /* 12-bit resolution for converted data */
	AdcHandle.Init.DataAlign             = ADC_DATAALIGN_RIGHT;           /* Right-alignment for converted data */
	AdcHandle.Init.ScanConvMode          = ENABLE;                       /* Sequencer disabled (ADC conversion on only 1 channel: channel set on rank 1) */
	AdcHandle.Init.EOCSelection          = ADC_EOC_SINGLE_CONV;           /* EOC flag picked-up to indicate conversion end */
	AdcHandle.Init.LowPowerAutoWait      = DISABLE;                       /* Auto-delayed conversion feature disabled */
	AdcHandle.Init.ContinuousConvMode    = ENABLE;                        /* Continuous mode enabled (automatic conversion restart after each conversion) */
	AdcHandle.Init.NbrOfConversion       = ADC_CHANNEL_NUMB;             /* Parameter discarded because sequencer is disabled */
	AdcHandle.Init.DiscontinuousConvMode = ENABLE;                       /* Parameter discarded because sequencer is disabled */
	AdcHandle.Init.NbrOfDiscConversion   = 1;                             /* Parameter discarded because sequencer is disabled */
	AdcHandle.Init.ExternalTrigConv      = ADC_SOFTWARE_START;            /* Software start to trig the 1st conversion manually, without external event */
	AdcHandle.Init.ExternalTrigConvEdge  = ADC_EXTERNALTRIGCONVEDGE_NONE; /* Parameter discarded because software trigger chosen */
	AdcHandle.Init.DMAContinuousRequests = ENABLE;                        /* ADC DMA continuous request to match with DMA circular mode */
	AdcHandle.Init.Overrun               = ADC_OVR_DATA_OVERWRITTEN;      /* DR register is overwritten with the last conversion result in case of overrun */
	/* Initialize ADC peripheral according to the passed parameters */
	if (HAL_ADC_Init(&AdcHandle) != HAL_OK)
	{
		return RESULT_ERROR;
	}
	
	__HAL_RCC_DMA1_CLK_ENABLE();
	DmaHandle.Instance                 = DMA1_Channel1;
	DmaHandle.Init.Direction           = DMA_PERIPH_TO_MEMORY;
	DmaHandle.Init.PeriphInc           = DMA_PINC_DISABLE;
	DmaHandle.Init.MemInc              = DMA_MINC_ENABLE;
	DmaHandle.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
	DmaHandle.Init.MemDataAlignment    = DMA_MDATAALIGN_HALFWORD;
	DmaHandle.Init.Mode                = DMA_NORMAL;
	DmaHandle.Init.Priority            = DMA_PRIORITY_MEDIUM;
	/* Deinitialize  & Initialize the DMA for new transfer */
	HAL_DMA_DeInit(&DmaHandle);
	HAL_DMA_Init(&DmaHandle);
	
	/* Associate the DMA handle */
	__HAL_LINKDMA(&AdcHandle, DMA_Handle, DmaHandle);

	/* NVIC configuration for DMA Input data interrupt */
	HAL_NVIC_SetPriority(DMA1_Channel1_IRQn, 3, 0);
	HAL_NVIC_EnableIRQ(DMA1_Channel1_IRQn); 

//	HAL_NVIC_SetPriority(ADC1_IRQn, 3, 0);
//	HAL_NVIC_EnableIRQ(ADC1_IRQn);
	

	/* ### - 2 - Start calibration ############################################ */
	if (HAL_ADCEx_Calibration_Start(&AdcHandle, ADC_SINGLE_ENDED) !=  HAL_OK)
	{
		return RESULT_ERROR;
	}
	
	ADC_ChannelConfTypeDef sConfig;
	/* ### - 3 - Channel configuration ######################################## */
	sConfig.Channel      = ADC_CHANNEL_1;                /* Sampled channel number */
	sConfig.Rank         = ADC_REGULAR_RANK_1;          /* Rank of sampled channel number ADCx_CHANNEL */
	sConfig.SamplingTime = ADC_SAMPLETIME_61CYCLES_5;   /* Sampling time (number of clock cycles unit) */
	sConfig.SingleDiff   = ADC_SINGLE_ENDED;            /* Single-ended input channel */
	sConfig.OffsetNumber = ADC_OFFSET_NONE;             /* No offset subtraction */ 
	sConfig.Offset = 0;                                 /* Parameter discarded because offset correction is disabled */
	if (HAL_ADC_ConfigChannel(&AdcHandle, &sConfig) != HAL_OK)
	{
		return RESULT_ERROR;
	}
	
	sConfig.Channel      = ADC_CHANNEL_2;
	sConfig.Rank         = ADC_REGULAR_RANK_2;

	if (HAL_ADC_ConfigChannel(&AdcHandle, &sConfig) != HAL_OK)
	{
	}

	/* ### - 4 - Start conversion in DMA mode ################################# */
	if (HAL_ADC_Start_DMA(&AdcHandle,
						(uint32_t *)adc_dma_buff,
						ADC_CHANNEL_NUMB * ADC_DMA_BUFF_LEN
					   ) != HAL_OK)
	{
		return RESULT_ERROR;
	}
	
	systick_1ms_cb_reg(adc_sample_1ms_cb);
  
	return RESULT_SUCCESS;
}








