/**********************************************************
 * @file    IoHwAb.h
 * @brief   IoHwAb Driver Configuration Header File
 * @details File này chứa các khai báo API cho driver IoHwAb
 *          theo chuẩn AUTOSAR
 *
 * @version 1.0
 * @date    2024-07-29
 * @author  Nguyen Duc Cuong
 **********************************************************/

#ifndef IOHWAB_H

#define IOHWAB_H

#include "IoHwAb_Cfg.h"

#define ADC_GROUP0_BUFFER_SIZE  4
extern Adc_ValueGroupType AdcGroup0Buffer[ADC_GROUP0_BUFFER_SIZE];

typedef enum {
    IOHWAB0_OK = 0,
    IOHWAB0_BUSY,
    IOHWAB0_ERROR
} IoHwAb0_StatusType;

/* API khởi tạo module */
void IoHwAb_Init(const IoHwAb0_ConfigType* ConfigPtr);

/* Đọc nhiệt độ hiện tại từ LM35 (°C) */
Std_ReturnType IoHwAb0_GetTemperature(uint16* temperature);

/* Đặt tốc độ quạt theo % duty */
Std_ReturnType IoHwAb0_SetFanSpeed(uint8 percent);

/* Điều khiển LED ON/OFF */
Std_ReturnType IoHwAb0_SetLedState(boolean state);

/* Cập nhật tốc độ quạt dựa trên nhiệt độ */
void IoHwAb0_ControlFanFromTemperature(void);

/* Cập nhật độ sáng LED dựa trên giá trị biến trở */
Std_ReturnType IoHwAb0_ControlLedBrightness(void);

/* Đọc giá trị biến trở (0-100%) */
Std_ReturnType IoHwAb0_GetPotentiometerValue(uint16* value);

#endif // IOHWAB_H