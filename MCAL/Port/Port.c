/**********************************************************
 * @file    Port.c
 * @brief   Port Driver Source File (AUTOSAR Standard)
 * @details File này hiện thực các API chuẩn AUTOSAR cho Port Driver,
 *          dành cho STM32F103 sử dụng SPL. Driver cho phép cấu hình,
 *          đổi mode, đổi chiều, refresh và đọc thông tin version của port/pin.
 *
 * @version 3.0
 * @date    2025-08-01
 * @author  Nguyen Duc Cuong
 **********************************************************/

/* ===============================
 *           Includes
 * =============================== */
#include "Port.h"
#include "stm32f10x_rcc.h"
#include <stddef.h>

/* ===============================
 *     Static/Internal Variables
 * =============================== */
static uint8_t Port_Initialized = 0;               /* Trạng thái đã init */
static const Port_ConfigType* Port_ConfigPtr = NULL;  /* Lưu con trỏ config runtime */

/* ===============================
 *      Internal Helper Function
 * =============================== */

/* 
 * Tìm index trong mảng cấu hình dựa vào số hiệu Pin
 * @return index >= 0 nếu tìm thấy, -1 nếu không tìm thấy
 */
static sint16 Port_FindPinIndex(Port_PinType Pin)
{
    if (Port_ConfigPtr == NULL) return -1;

    for (uint16_t i = 0; i < Port_ConfigPtr->PinCount; i++)
    {
        if (Port_ConfigPtr->PinConfigs[i].Pin == Pin)
            return i;
    }
    return -1;
}

/* 
 * Cấu hình 1 pin GPIO dựa trên thông số AUTOSAR
 */
static void Port_ApplyPinConfig(const Port_PinConfigType* pinCfg) 
{
    GPIO_InitTypeDef GPIO_InitStruct;

    Port_PinType pinId = pinCfg->Pin;
    uint8_t portNum = PORT_GET_PORT_NUM(pinId);
    uint8_t pinNum  = PORT_GET_PIN_NUM(pinId);
    uint16_t pinMask = (1U << pinNum);

    /* Kích hoạt clock cho PORT */
    switch (portNum) 
    {
        case PORT_ID_A: RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); break;
        case PORT_ID_B: RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE); break;
        case PORT_ID_C: RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE); break;
        case PORT_ID_D: RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE); break;
        default: return;
    }

    /* Pin mask */
    GPIO_InitStruct.GPIO_Pin = pinMask;

    /* Cấu hình tốc độ */
    if(pinCfg->Speed == SPEED_SMALL)
        GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;
    else if(pinCfg->Speed == SPEED_MEDIUM)
        GPIO_InitStruct.GPIO_Speed = GPIO_Speed_10MHz;
    else
        GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;

    /* Cấu hình mode - DIO */
    if (pinCfg->Mode == PORT_PIN_MODE_DIO) 
    {
        if (pinCfg->Direction == PORT_PIN_OUT) 
        {
            GPIO_InitStruct.GPIO_Mode = (pinCfg->Pull == PORT_PIN_PULL_UP) ? GPIO_Mode_Out_PP : GPIO_Mode_Out_OD;
        } 
        else 
        {
            if (pinCfg->Pull == PORT_PIN_PULL_UP)
                GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
            else if (pinCfg->Pull == PORT_PIN_PULL_DOWN)
                GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPD;
            else
                GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
        }
    }

    /* Cấu hình mode - ADC */
    if(pinCfg->Mode == PORT_PIN_MODE_ADC)
    {
        GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AIN;
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC2, ENABLE);
        RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
    }

    /* Cấu hình mode - PWM */
    if(pinCfg->Mode == PORT_PIN_MODE_PWM)
    {
        GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
    }

    /* Khởi tạo chân */
    GPIO_TypeDef* gpioPort = PORT_GET_PORT(portNum); 
    GPIO_Init(gpioPort, &GPIO_InitStruct);

    /* Nếu là output, set mức ban đầu */
    if (pinCfg->Direction == PORT_PIN_OUT) {
        if (pinCfg->Level == PORT_PIN_LEVEL_HIGH)
            GPIO_SetBits(gpioPort, pinMask);
        else
            GPIO_ResetBits(gpioPort, pinMask);
    }
}

/* ===============================
 *     Function Definitions
 * =============================== */

void Port_Init(const Port_ConfigType* ConfigPtr) 
{
    if (ConfigPtr == NULL) return;
    Port_ConfigPtr = ConfigPtr;

    for (uint16_t i = 0; i < Port_ConfigPtr->PinCount; i++) {
        Port_ApplyPinConfig(&Port_ConfigPtr->PinConfigs[i]);
    }
    Port_Initialized = 1;
}

void Port_SetPinDirection(Port_PinType Pin, Port_PinDirectionType Direction) 
{
    if (!Port_Initialized || Port_ConfigPtr == NULL) return;

    sint16 index = Port_FindPinIndex(Pin);
    if (index == -1) return;  /* Pin không có trong config */

    if (!Port_ConfigPtr->PinConfigs[index].DirectionChangeable) return;

    Port_PinConfigType pinCfgTemp = Port_ConfigPtr->PinConfigs[index];
    pinCfgTemp.Direction = Direction;
    Port_ApplyPinConfig(&pinCfgTemp);
}

void Port_SetPinMode(Port_PinType Pin, Port_PinModeType Mode) 
{
    if (!Port_Initialized || Port_ConfigPtr == NULL) return;

    sint16 index = Port_FindPinIndex(Pin);
    if (index == -1) return;

    if (!Port_ConfigPtr->PinConfigs[index].ModeChangeable) return;

    Port_PinConfigType pinCfgTemp = Port_ConfigPtr->PinConfigs[index];
    pinCfgTemp.Mode = Mode;
    Port_ApplyPinConfig(&pinCfgTemp);
}

void Port_RefreshPortDirection(void) 
{
    if (!Port_Initialized || Port_ConfigPtr == NULL) return;

    for (uint16_t i = 0; i < Port_ConfigPtr->PinCount; i++) {
        if (!Port_ConfigPtr->PinConfigs[i].DirectionChangeable) {
            Port_ApplyPinConfig(&Port_ConfigPtr->PinConfigs[i]);
        }
    }
}

void Port_GetVersionInfo(Std_VersionInfoType* versioninfo) 
{
    if (versioninfo == NULL) return;
    versioninfo->vendorID = PORT_VENDOR_ID;
    versioninfo->moduleID = PORT_MODULE_ID;
    versioninfo->sw_major_version = PORT_SW_MAJOR_VERSION;
    versioninfo->sw_minor_version = PORT_SW_MINOR_VERSION;
    versioninfo->sw_patch_version = PORT_SW_PATCH_VERSION;
}
