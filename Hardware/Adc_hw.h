/**********************************************************
 * @file    ADC_hw.h
 * @brief   ADC Hardware Driver Header File 
 * @details File này định nghĩa API liên quan đến phần cứng STM32
 * @version 1.0
 * @date    2025-06-27
 * @author  Nguyen Duc Cuong
 **********************************************************/

#ifndef ADC_HW_H
#define ADC_HW_H

#include "MCAL/Adc/Adc.h"
#include "stm32f10x_adc.h"

static inline ADC_TypeDef* Adc_GetHwInstance(Adc_InstanceType instance)
{
    switch (instance)
    {
        case ADC_INSTANCE_1: return ADC1;
        case ADC_INSTANCE_2: return ADC2;
        default: return NULL_PTR;
    }
}

/**
 * @brief       Function to initialize the ADC hardware 
 * @param       ConfigPtr: Pointer to configuration set in Variant PB (Variant PC requires a NULL_PTR).
 * @return      void
 */

static inline void Adc_HwConfigureADC(ADC_TypeDef* adc, const Adc_GroupDefType* group)
{
    ADC_DeInit(adc);

    ADC_InitTypeDef init;
    init.ADC_Mode = ADC_Mode_Independent;
    init.ADC_ScanConvMode = group->NumChannels > 1 ? ENABLE : DISABLE;
    init.ADC_ContinuousConvMode = (group->ConversionMode == ADC_CONV_MODE_CONTINUOUS) ? ENABLE : DISABLE;
    init.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
    init.ADC_DataAlign = (group->ResultAlignment == ADC_ALIGN_LEFT) ? ADC_DataAlign_Left : ADC_DataAlign_Right;
    init.ADC_NbrOfChannel = group->NumChannels;

    /* Khác với SPL, Hàm khởi tạo từng Channel nằm trong hàm StartGrouponversion*/

    ADC_Init(adc, &init);
    ADC_Cmd(adc, ENABLE);

    ADC_ResetCalibration(adc);                  // Bắt đầu reset hiệu chuẩn
    while (ADC_GetResetCalibrationStatus(adc)); // Chờ reset xong

    ADC_StartCalibration(adc);                  // Bắt đầu hiệu chuẩn
    while (ADC_GetCalibrationStatus(adc));      // Chờ hiệu chuẩn hoàn tất
}


static inline void Adc_HwDisableDMA(ADC_TypeDef* adc, const Adc_GroupDefType* group)
{
    DMA_Cmd(group->DMA_Channel, DISABLE);
    ADC_DMACmd(adc, DISABLE);
    DMA_ITConfig(group->DMA_Channel, DMA_IT_TC, DISABLE);
    NVIC_DisableIRQ(DMA1_Channel1_IRQn);
}

#endif /*ADC_HW_H*/