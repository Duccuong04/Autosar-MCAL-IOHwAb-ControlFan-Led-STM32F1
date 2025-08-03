/**********************************************************
 * @file    ADC.h
 * @brief   ADC Driver Source File (AUTOSAR Standard)
 * @details File này định nghĩa các API chuẩn AUTOSAR cho ADC Driver,
 *          dành cho STM32F103 sử dụng SPL. Driver cho phép cấu hình,
 *          thiết lập mode, kênh đọc ADC, đọc giá trị từ ngoại vi
 *
 * @version 1.0
 * @date    2025-06-27
 * @author  Nguyen Duc Cuong
 **********************************************************/

#include "Adc.h"
#include "Adc_hw.h"

#define MAX_ADC_GROUPS 4

static const Adc_ConfigType* Adc_ConfigPtr = NULL_PTR;
static Adc_ValueGroupType* Adc_ResultBuffer[MAX_ADC_GROUPS] = {0}; /* Mảng con trỏ lưu giá trị đọc được của từng channel trong mỗi group*/
static Adc_StatusType Adc_GroupStatus[MAX_ADC_GROUPS] = {ADC_IDLE};

/**
 * @brief       Initializes the ADC hardware units and driver.
 * @param       ConfigPtr: Pointer to configuration set in Variant PB (Variant PC requires a NULL_PTR).
 * @return      void
 */

void Adc_Init (const Adc_ConfigType* ConfigPtr)
{
    /* Check input ConfigPtr*/
    if(ConfigPtr == NULL_PTR)
    {
        return;
    }

    Adc_ConfigPtr = ConfigPtr;

    for(int i = 0; i < ConfigPtr->NumGroups; i++)
    {
        const Adc_GroupDefType* group = &ConfigPtr->Groups[i];
        ADC_TypeDef *adc = Adc_GetHwInstance(group->AdcInstance);

        /* Khởi tạo clock & GPIO đã được làm trong Port Driver
         * ...................................................
        */

        /* ADC Init Hardware sử dụng SPL cho STM32F1*/
        Adc_HwConfigureADC(adc, group);
        
    }

    if (ConfigPtr->InitCallback)
    {
        ConfigPtr->InitCallback();
    } 
}

/**
 * @brief       Returns all ADC HW Units to a state comparable to their power on reset state.
 * @param       void
 * @return      void
 */

void Adc_DeInit()
{
    /* Deinitialize hardware config */
    ADC_DeInit(ADC1);

    /* Remove config */
    Adc_ConfigPtr = NULL_PTR;
}

/**
 * @brief       Initializes ADC driver with the group specific result buffer start address where the conversion
                results will be stored. The application has to ensure that the application buffer, where Data
                BufferPtr points to, can hold all the conversion results of the specified group. The initialization
                with Adc_SetupResultBuffer is required after reset, before a group conversion can be started.
 * @param       Group: Numeric ID of requested ADC Channel group.
 *              DataBufferPtr: pointer to result data buffer
 * @return      E_OK, E_NOT_OK
 */

Std_ReturnType Adc_SetupResultBuffer(Adc_GroupType Group, Adc_ValueGroupType* DataBufferPtr)
{
    if (Group >= 8 || !DataBufferPtr) return E_NOT_OK;
    Adc_ResultBuffer[Group] = DataBufferPtr;
    return E_OK;
}

/**
 * @brief       Starts the conversion of all channels of the requested ADC Channel group.
 * @param       Group: Numeric ID of requested ADC Channel group.
 * @return      void
 */
void Adc_StartGroupConversion(Adc_GroupType Group)
{
    if (!Adc_ConfigPtr || Group >= Adc_ConfigPtr->NumGroups) return;

    const Adc_GroupDefType* group = &Adc_ConfigPtr->Groups[Group];
    ADC_TypeDef* adc = Adc_GetHwInstance(group->AdcInstance);

    if (group->Adc_StreamEnableType == 1)
    {
        DMA_InitTypeDef DMA_InitStruct;
        DMA_DeInit(group->DMA_Channel);  // Reset DMA channel trước khi init

        DMA_InitStruct.DMA_Mode = (group->Adc_StreamBufferMode == ADC_STREAM_BUFFER_CIRCULAR) ? DMA_Mode_Circular : DMA_Mode_Normal;
        DMA_InitStruct.DMA_DIR = DMA_DIR_PeripheralSRC;
        DMA_InitStruct.DMA_M2M = DMA_M2M_Disable;
        DMA_InitStruct.DMA_BufferSize = group->NumChannels;
        DMA_InitStruct.DMA_MemoryBaseAddr = (uint32)(uintptr_t)Adc_ResultBuffer[Group]; // Giả sử group 0, cần thay đổi nếu có nhiều group
        DMA_InitStruct.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
        DMA_InitStruct.DMA_MemoryInc = DMA_MemoryInc_Enable;
        DMA_InitStruct.DMA_PeripheralBaseAddr = (uint32)&adc->DR;
        DMA_InitStruct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
        DMA_InitStruct.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
        DMA_InitStruct.DMA_Priority = DMA_Priority_High;

    DMA_Init(group->DMA_Channel, &DMA_InitStruct);
    // DMA_ITConfig(group->DMA_Channel, DMA_IT_TC, ENABLE);
    // NVIC_EnableIRQ(DMA1_Channel1_IRQn);

    DMA_Cmd(group->DMA_Channel, ENABLE);
    ADC_DMACmd(adc, ENABLE);
    }

    for (uint8 i = 0; i < group->NumChannels; i++) 
    {
        ADC_RegularChannelConfig(adc, group->Channels[i].Channel,
                                 group->Channels[i].Rank,
                                 group->Channels[i].SamplingTime);
    }

    ADC_SoftwareStartConvCmd(adc, ENABLE);

    Adc_GroupStatus[Group] = ADC_BUSY;
}

/**
 * @brief       Returns the conversion status of the requested ADC Channel group.
 * @param       Group: Numeric ID of requested ADC Channel group.
 * @return      Adc_StatusType: Conversion status for the requested group.
 */
Adc_StatusType Adc_GetGroupStatus(Adc_GroupType Group)
{
    if (Group >= 8) return ADC_IDLE;
    return Adc_GroupStatus[Group];
}

/**
 * @brief       Reads the group conversion result of the last completed conversion round of the requested
 *              group and stores the channel values starting at the DataBufferPtr address. The group channel
 *              values are stored in ascending channel number order ( in contrast to the storage layout of the
 *              result buffer if streaming access is configured).
 * @param       Group: Numeric ID of requested ADC channel group.
 * @param       DataBufferPtr: ADC results of all channels of the selected group are stored in the data buffer addressed with the pointer.
 * @return      Std_ReturnType: 
 *              E_OK: results are available and written to the data buffer
 *              E_NOT_OK: no results are available or development error occured
 */

Std_ReturnType Adc_ReadGroup(Adc_GroupType Group, Adc_ValueGroupType* DataBufferPtr)
{
    if (!Adc_ConfigPtr || Group >= Adc_ConfigPtr->NumGroups || !DataBufferPtr)
        return E_NOT_OK;

    const Adc_GroupDefType* group = &Adc_ConfigPtr->Groups[Group];
    ADC_TypeDef* adc = Adc_GetHwInstance(group->AdcInstance);

    /* Sắp xếp theo Rank để đảm bảo đúng thứ tự chuyển đổi */
    for (uint8 rank = 1; rank <= group->NumChannels; rank++) {
        while (!ADC_GetFlagStatus(adc, ADC_FLAG_EOC));

        uint16_t value = ADC_GetConversionValue(adc);

        /*Gán giá trị vào đúng vị trí tương ứng với Channel*/
        for (uint8 i = 0; i < group->NumChannels; i++) {
            if (group->Channels[i].Rank == rank) {
                DataBufferPtr[i] = value;
                break;
            }
        }
    }

    Adc_GroupStatus[Group] = ADC_COMPLETED;
    return E_OK;
}


/**
 * @brief       Enables the notification mechanism for the requested ADC Channel group.
 * @param       Group: Numeric ID of requested ADC Channel group.
 * @return      void
 */
void Adc_EnableGroupNotification(Adc_GroupType Group)
{
    if (!Adc_ConfigPtr || Group >= MAX_ADC_GROUPS) return;

    const Adc_GroupDefType* group = &Adc_ConfigPtr->Groups[Group];
    ADC_TypeDef* adc = Adc_GetHwInstance(group->AdcInstance);

    ADC_ITConfig(adc, ADC_IT_EOC, ENABLE);    

    NVIC_InitTypeDef NVICInitStruct;

    NVICInitStruct.NVIC_IRQChannel = ADC1_2_IRQn;
    NVICInitStruct.NVIC_IRQChannelPreemptionPriority = 0x00;
    NVICInitStruct.NVIC_IRQChannelSubPriority = 0x00;
    NVICInitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_EnableIRQ(ADC1_2_IRQn);
    NVIC_Init(&NVICInitStruct);
}

/**
 * @brief       Disables the notification mechanism for the requested ADC Channel group.
 * @param       Group: Numeric ID of requested ADC Channel group.
 * @return      void
 */
void Adc_DisableGroupNotification(Adc_GroupType Group)
{
    if (!Adc_ConfigPtr || Group >= MAX_ADC_GROUPS) return;
    const Adc_GroupDefType* group = &Adc_ConfigPtr->Groups[Group];
    ADC_ITConfig(Adc_GetHwInstance(group->AdcInstance), ADC_IT_EOC, DISABLE);
}

/**
 * @brief       Enables the hardware trigger for the requested ADC Channel group.
 * @param       Group: Numeric ID of requested ADC Channel group.
 * @return      void
 */
void Adc_EnableHardwareTrigger(Adc_GroupType Group)
{
    if (!Adc_ConfigPtr || Group >= MAX_ADC_GROUPS) return;
    const Adc_GroupDefType* group = &Adc_ConfigPtr->Groups[Group];
    ADC_TypeDef* adc = Adc_GetHwInstance(group->AdcInstance);

    ADC_ExternalTrigConvCmd(adc, ENABLE);
}

/**
 * @brief       Disables the hardware trigger for the requested ADC Channel group.
 * @param       Group: Numeric ID of requested ADC Channel group.
 * @return      void
 */
void Adc_DisableHardwareTrigger(Adc_GroupType Group)
{
    if (!Adc_ConfigPtr || Group >= MAX_ADC_GROUPS) return;
    const Adc_GroupDefType* group = &Adc_ConfigPtr->Groups[Group];
    ADC_TypeDef* adc = Adc_GetHwInstance(group->AdcInstance);

    ADC_ExternalTrigConvCmd(adc, DISABLE);
}

/**
 * @brief       Disable DMA
 * @param       Group
 * @return      void
 */
void Adc_DisableDma(Adc_GroupType Group)
{
    if (!Adc_ConfigPtr || Group >= Adc_ConfigPtr->NumGroups) return;

    const Adc_GroupDefType* group = &Adc_ConfigPtr->Groups[Group];
    ADC_TypeDef* adc = Adc_GetHwInstance(group->AdcInstance);
    Adc_HwDisableDMA(adc, group);
}

/**
 * @brief       Allow ADC convert on Power state
 * @param[in]   GroupADC, State
 * @return      E_OK E_NOT_OK
 */

Std_ReturnType Adc_SetPowerState(Adc_GroupType Group, Adc_PowerStateType state)
{
    if (!Adc_ConfigPtr || Group >= MAX_ADC_GROUPS) return;

    const Adc_GroupDefType* group = &Adc_ConfigPtr->Groups[Group];
    ADC_TypeDef* adc = Adc_GetHwInstance(group->AdcInstance);
    switch (state) {
        case ADC_POWERSTATE_OFF:
            ADC_Cmd(adc, DISABLE);
            return E_OK;
        case ADC_POWERSTATE_ON:
            ADC_Cmd(adc, ENABLE);
            return E_OK;
        case ADC_POWERSTATE_LOWPOWER:
            // VD: giảm prescaler, tắt clock khi idle
            return E_OK;
        default:
            return E_NOT_OK;
    }
}
void Adc_GetVersionInfo (Std_VersionInfoType* versioninfo)
{
    /* Check if VersionInfo is null */
    if (versioninfo == NULL_PTR) 
    {
        return;  
    }

    /* Information of vendor ID and module ID */
    versioninfo->vendorID = 1;          
    versioninfo->moduleID = 123;        

    /* Information of sw version */
    versioninfo->sw_major_version = 1;  
    versioninfo->sw_minor_version = 0;  
    versioninfo->sw_patch_version = 0;  
}

/**
 * @brief       IsrHandlerprocess ADC interrupt
 * @param       ADCTydeDef* (ADC Instance: ADC1/ ADC2)
 * @return      void
 */
void Adc_IsrHandler()
{
    if (!Adc_ConfigPtr) return;

    /* Duyệt từng Group ADC*/
    for (uint8 i = 0; i < Adc_ConfigPtr->NumGroups; i++)
    {
        const Adc_GroupDefType* group = &Adc_ConfigPtr->Groups[i];

        if (group->AdcInstance == ADC_INSTANCE_1)
        {
            ADC_ClearITPendingBit(ADC1, ADC_IT_EOC);

            if (group->IntNotificationCb)
            {
                group->IntNotificationCb();
            }
        }

        else if (group->AdcInstance == ADC_INSTANCE_2)
        {
            ADC_ClearITPendingBit(ADC2, ADC_IT_EOC);

            if (group->IntNotificationCb)
            {
                group->IntNotificationCb();
            }
        }
    }
}

void Adc_DMAIsrHandler()
{
    const Adc_GroupDefType* group = &Adc_ConfigPtr->Groups[0];

    if (DMA_GetITStatus(DMA1_FLAG_TC1)) 
    {
        DMA_ClearITPendingBit(DMA1_FLAG_TC1);
        if (group->DMAIntNotificationCb) group->DMAIntNotificationCb();
    }
}