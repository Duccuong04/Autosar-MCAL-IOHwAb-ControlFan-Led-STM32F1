/**********************************************************
 * @file    IoHwAb.c
 * @brief   IoHwAb Driver Configuration Source File
 * @details File này chứa các định nghĩa API cho driver IoHwAb
 *          theo chuẩn AUTOSAR
 *
 * @version 1.0
 * @date    2024-07-29
 * @author  Nguyen Duc Cuong
 **********************************************************/

#include "IoHwAb.h"

#define ADC_INVALID_VALUE 0xFFFF

/* Con trỏ cấu hình toàn cục */
static const IoHwAb0_ConfigType* IoHwAb0_ConfigPtr = NULL_PTR;

Adc_ValueGroupType AdcGroup0Buffer[ADC_GROUP0_BUFFER_SIZE];

/* ================== API khởi tạo ================== */
void IoHwAb_Init(const IoHwAb0_ConfigType* ConfigPtr)
{
    IoHwAb0_ConfigPtr = ConfigPtr;

    /* Khởi tạo MCAL drivers */
    Port_Init(ConfigPtr->IoHwAb0_PortCfg);
    Adc_Init(ConfigPtr->IoHwAb0_AdcCfg);
    Pwm_Init(ConfigPtr->IoHwAb0_PwmCfg);


    Adc_SetupResultBuffer(0, AdcGroup0Buffer);
    Adc_StartGroupConversion(0);
}

/* ================== Điều khiển LED ON/OFF ================== */
Std_ReturnType IoHwAb0_SetLedState(boolean state)
{
    if (IoHwAb0_ConfigPtr == NULL_PTR)
        return E_NOT_OK;

    Dio_WriteChannel(IoHwAb0_ConfigPtr->ledChannel, state ? STD_HIGH : STD_LOW);
    return E_OK;
}

/* ================== Đọc nhiệt độ LM35 ================== */
Std_ReturnType IoHwAb0_GetTemperature(uint16* temperature)
{
    if ((IoHwAb0_ConfigPtr == NULL_PTR) || (temperature == NULL_PTR))
        return E_NOT_OK;

    /* Đọc giá trị ADC từ DMA buffer */
    uint16 adcValue = AdcGroup0Buffer[0];

    /* LM35DZ: 10 mV / °C
       ADC: 12-bit, Vref = 3.3V
       Điện áp (mV) = adcValue * 3300 / 4095
       Nhiệt độ (°C) = Điện áp / 10 mV
    */
    uint32 voltage_mV = ((uint32)adcValue * 3300U) / 4095U;
    *temperature = (uint16)(voltage_mV / 10U);   /* Đơn vị: °C */

    return E_OK;
}

/* ================== Đặt tốc độ quạt theo % duty ================== */
Std_ReturnType IoHwAb0_SetFanSpeed(uint8 percent)
{
    if ((IoHwAb0_ConfigPtr == NULL_PTR) || (percent > 100U))
        return E_NOT_OK;

    const CoolingMotorType* motor = IoHwAb0_ConfigPtr->coolingMotor;

    /* Map 0–100% -> 0–0x8000 */
    uint16 duty = (uint16)(((uint32)percent * 0x8000U) / 100U);

    Pwm_SetDutyCycle(motor->pwmChannelId, duty);

    return E_OK;
}

void IoHwAb0_ControlFanFromTemperature(void)
{
    uint16 tempC;
    if (IoHwAb0_GetTemperature(&tempC) == E_OK)
    {
        uint8 percent = 0;

        /* Ví dụ: 30°C -> 0%, 80°C -> 100% */
        if (tempC >= 30U)
        {
            if (tempC >= 80U)
                percent = 100U;
            else
                percent = (uint8)((tempC - 30U) * 100U / 50U);
        }

        IoHwAb0_SetFanSpeed(percent);
    }
}

/* ================== Đọc biến trở để điều khiển độ sáng LED  ================== */
Std_ReturnType IoHwAb0_GetPotentiometerValue(uint16* value)
{
    if ((IoHwAb0_ConfigPtr == NULL_PTR) || (value == NULL_PTR))
        return E_NOT_OK;

    const PotentiometerType* pot = IoHwAb0_ConfigPtr->potentiometer;

    /* Đọc giá trị ADC từ DMA buffer */
    uint16 adcValue = AdcGroup0Buffer[pot->adcChannelId];

    if(adcValue > 2000)
    {
        IoHwAb0_SetLedState(STD_LOW); 
    }

    else
    {
        IoHwAb0_SetLedState(STD_HIGH); 
    }

    /* Chuyển đổi giá trị ADC về khoảng 0-100% */
    *value = (uint16)((adcValue * 100U) / 4095U);  /* Giả sử ADC 12-bit */

    return E_OK;
}

/* ============ Điều khiển độ sáng LED dựa vào giá trị biến trở đọc được ============*/
Std_ReturnType IoHwAb0_ControlLedBrightness(void)
{
    uint16 potValuePercent;
    if (IoHwAb0_GetPotentiometerValue(&potValuePercent) != E_OK)
        return E_NOT_OK;

    const LEDPwmType* ledPwm = IoHwAb0_ConfigPtr->brightLED;

    /* Map 0-100% -> 0-0x8000 */
    uint16 duty = (uint16)(((uint32)potValuePercent * 0x8000U) / 100U);
    Pwm_SetDutyCycle(ledPwm->pwmChannelId, duty);

    return E_OK;
}
