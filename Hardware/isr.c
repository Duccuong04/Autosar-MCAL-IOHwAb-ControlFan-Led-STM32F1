/**********************************************************
 * @file    isr.c
 * @brief   ISR Entry point
 * @details Khi có ngắt xảy ra tại TIMER vào đây để thực hiện hàm Handler tương ứng
 *          Vì ngắt Timer2 có thể xảy ra từ nhiều nguồn nên phải có file này có ADC thì không
 * @version 1.0
 * @date    2025-07-06
 * @author  Nguyen Duc Cuong
 **********************************************************/

#include "stm32f10x.h"
#include "misc.h"
#include "MCAL/Pwm/Pwm.h"

void TIM2_IRQHandler(void)
{
    Pwm_IsrHandler(TIM2);
}


void ADC1_2_IRQHandler(void)
{
    // Do nothing
    //Adc_IsrHandler();
}

void DMA1_Channel1_IRQHandler(void)
{
    Adc_DMAIsrHandler();
}

