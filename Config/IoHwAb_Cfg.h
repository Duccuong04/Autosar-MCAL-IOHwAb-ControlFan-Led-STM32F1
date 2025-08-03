/**********************************************************
 * @file    IoHwAb_Cfg.h
 * @brief   IoHwAb Driver Configuration Header File
 * @details File này chứa các struct config của driver IoHwAb0
 *          theo chuẩn AUTOSAR
 *
 * @version 1.0
 * @date    2025-07-29
 * @author  Nguyen Duc Cuong
 **********************************************************/

#ifndef IOHWAB_CFG_H
#define IOHWAB_CFG_H    

#include "Std_Types.h"

#include "Port.h"
#include "MCAL/Adc/Adc.h"
#include "Pwm.h"

/* Số lượng Pin dùng trong Module có Id 0 (IoHwAb0)*/
#define IoHwAb0_PortCfg_PinsCount    6         

/* Định nghĩa ADC group và PWM channel */
#define TEMP_ADC_GROUP        0
#define FAN_PWM_CHANNEL       1

/* Cấu hình tương ứng với chức năng của dự án (IoHwAb Id 0) */

/* Config Port*/
extern const Port_PinConfigType IoHwAb0_PortCfg_Pins[IoHwAb0_PortCfg_PinsCount ];
extern const Port_ConfigType IoHwAb0_PortCfg;

/* Config Adc*/
extern const Adc_ConfigType IoHwAb0_AdcCfg;
extern const Adc_GroupDefType IoHwAb0_Adc_GroupList[];

/* Config Pwm*/
extern const Pwm_ChannelConfigType IoHwAb0_PwmChannelsConfig[];
extern const Pwm_ConfigType IoHwAb0_PwmCfg;

/* Struct cảm biến nhiệt độ */
typedef struct {
    Adc_ChannelType adcChannelId;
    uint16 minValue;
    uint16 maxValue;
    uint16 lowThreshold;
    uint16 highThreshold;
} TemperatureSensorType;

/* Struct điều khiển quạt */
typedef struct {
    Pwm_ChannelType pwmChannelId;
    Pwm_PeriodType  pwmPeriod;
    uint16          outputCompareValue;
} CoolingMotorType;

/* Struct biến trở */
typedef struct {
    Adc_ChannelType adcChannelId;
    uint16 minValue;
    uint16 maxValue;
} PotentiometerType;

/* Struct LED điều chỉnh độ sáng */
typedef struct {      
    Pwm_ChannelType pwmChannelId;
    Pwm_PeriodType  pwmPeriod;
    uint16          outputCompareValue;
} LEDPwmType;

/* Cấu trúc tổng quát */
typedef struct {
    const Port_ConfigType* IoHwAb0_PortCfg;
    const Adc_ConfigType*  IoHwAb0_AdcCfg;
    const Pwm_ConfigType*  IoHwAb0_PwmCfg;
    const TemperatureSensorType* tempSensor; /* Cảm biến nhiệt độ */
    const CoolingMotorType*      coolingMotor; /* Quạt làm mát */
    const PotentiometerType* potentiometer; /* Biến trở */
    const LEDPwmType* brightLED; /* LED điều chỉnh độ sáng */
    Dio_ChannelType              ledChannel;  /* Chân LED */
} IoHwAb0_ConfigType;

/* Instance 0 config */
extern const IoHwAb0_ConfigType IoHwAb0_Config;

/*................Thêm các config cho IoHwAb1, IoHwAb2,... dưới đây*/

#endif // IOHWAB_CFG_H