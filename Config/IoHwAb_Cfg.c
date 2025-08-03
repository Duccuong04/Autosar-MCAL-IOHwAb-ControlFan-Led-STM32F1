/**********************************************************
 * @file    IoHwAb_Cfg.h
 * @brief   IoHwAb Driver Configuration Header File
 * @details File này chứa các định nghĩa các struct config của driver IoHwAb0
 *          theo chuẩn AUTOSAR
 *
 * @version 1.0
 * @date    2025-07-29
 * @author  Nguyen Duc Cuong
 **********************************************************/

#include "IoHwAb_Cfg.h"

/* ========== PORT CONFIG ========== */
const Port_PinConfigType IoHwAb0_PortCfg_Pins[IoHwAb0_PortCfg_PinsCount] = {
    /* PA0: ADC input cho biến trở */
    {
        .Pin = PORT_PIN_A0,
        .Mode = PORT_PIN_MODE_ADC,
        .Speed = SPEED_HIGH,
        .Direction = PORT_PIN_IN,
        .DirectionChangeable = 0,
        .Level = PORT_PIN_LEVEL_LOW,
        .Pull = PORT_PIN_PULL_NONE,
        .ModeChangeable = 0
    },
    /* PA1: ADC input cho cảm biến nhiệt độ LM35 */
    {
        .Pin = PORT_PIN_A1,
        .Mode = PORT_PIN_MODE_ADC,
        .Speed = SPEED_HIGH,
        .Direction = PORT_PIN_IN,
        .DirectionChangeable = 0,
        .Level = PORT_PIN_LEVEL_LOW,
        .Pull = PORT_PIN_PULL_NONE,
        .ModeChangeable = 0
    },
    /* PA2: PWM điều chỉnh tốc độ quay của quạt */
    {
        .Pin = PORT_PIN_A2,
        .Mode = PORT_PIN_MODE_PWM,
        .Speed = SPEED_HIGH,
        .Direction = PORT_PIN_OUT,
        .DirectionChangeable = 0,
        .Level = PORT_PIN_LEVEL_LOW,
        .Pull = PORT_PIN_PULL_NONE,
        .ModeChangeable = 0
    },
    /* PA3: PWM output cho LED để điều chỉnh độ sáng */
    {
        .Pin = PORT_PIN_A3,
        .Mode = PORT_PIN_MODE_PWM,
        .Speed = SPEED_HIGH,
        .Direction = PORT_PIN_OUT,
        .DirectionChangeable = 0,
        .Level = PORT_PIN_LEVEL_LOW,
        .Pull = PORT_PIN_PULL_NONE,
        .ModeChangeable = 0
    },
    /* PA15: DIO output cho LED */
    {
        .Pin = PORT_PIN_C13,
        .Mode = PORT_PIN_MODE_DIO,
        .Speed = SPEED_HIGH,
        .Direction = PORT_PIN_OUT,
        .DirectionChangeable = 0,
        .Level = PORT_PIN_LEVEL_LOW,
        .Pull = PORT_PIN_PULL_NONE,
        .ModeChangeable = 0
    }
};

const Port_ConfigType IoHwAb0_PortCfg = {
    .PinConfigs = IoHwAb0_PortCfg_Pins,
    .PinCount   = IoHwAb0_PortCfg_PinsCount 
};

const Adc_GroupDefType IoHwAb0_AdcGroups[] = {
    {
        .GroupId = 0,
        .ConversionMode = ADC_CONV_MODE_CONTINUOUS,
        .TriggerSource = ADC_TRIGG_SRC_SW,
        .ResultAlignment = ADC_ALIGN_RIGHT,
        .AdcInstance = ADC_INSTANCE_1,
        .Priority = 1,
        .NumChannels = 2,
        .Channels = {
            {.Channel = 0, .SamplingTime = ADC_SampleTime_55Cycles5, .Rank = 1},
            {.Channel = 1, .SamplingTime = ADC_SampleTime_55Cycles5, .Rank = 2}
        },
        .DMA_Channel = DMA1_Channel1,
        .Adc_StreamEnableType = 1,
        .Adc_StreamBufferMode = ADC_STREAM_BUFFER_CIRCULAR,
        .DMAIntNotificationCb = NULL,  // Callback khi DMA hoàn thành
        .StreamBufferType = 1
    }
};

const Adc_ConfigType IoHwAb0_AdcCfg = {
    .NumGroups = 1,
    .Groups = IoHwAb0_AdcGroups,
    .InitCallback = NULL_PTR
};

/* ========== PWM CONFIG ========== */
const Pwm_ChannelConfigType IoHwAb0_PwmChannels[] = {
    /*Channel 3: PA2 - TIM2_CH3 - Điều khiển quạt*/
    {
        .TIMx             = TIM2,
        .channel          = 3,
        .classType        = PWM_VARIABLE_PERIOD,
        .defaultPeriod    = 99,      /* 10 kHz */
        .defaultDutyCycle = 0,       /* 0% ban đầu */
        .polarity         = PWM_HIGH,
        .idleState        = PWM_LOW,
        .notificationEnable = 0
    },
    /* Channel 4: PA3 - TIM2_CH4 - Điều khiển LED*/
    {
        .TIMx             = TIM2,
        .channel          = 4,
        .classType        = PWM_VARIABLE_PERIOD,
        .defaultPeriod    = 999,     /* 1 kHz */
        .defaultDutyCycle = 0,       /* 0% ban đầu */
        .polarity         = PWM_HIGH,
        .idleState        = PWM_LOW,
        .notificationEnable = 0
    }
};

const Pwm_ConfigType IoHwAb0_PwmCfg = {
    .Channels    = IoHwAb0_PwmChannels,
    .NumChannels = sizeof(IoHwAb0_PwmChannels) / sizeof(Pwm_ChannelConfigType)
};

/* ========== SENSOR & MOTOR ========== */
const TemperatureSensorType LM35_Sensor = {
    .adcChannelId = 0,    /* ADC1 channel 0 */
    .minValue = 0,
    .maxValue = 100,
    .lowThreshold = 20,   /* Ngưỡng thấp 20°C */
    .highThreshold = 80   /* Ngưỡng cao 80°C */
};

const CoolingMotorType FanMotor = {
    .pwmChannelId = 0,       /* PWM channel index 0 trong IoHwAb0_PwmChannels */
    .pwmPeriod = 99,
    .outputCompareValue = 0
};

/* ========== Potentiometer & Bright of LED ========== */
const PotentiometerType Potentiometer = {
    .adcChannelId = 1,    /* ADC1 channel 1 */
    .minValue = 0,
    .maxValue = 4096,
};

const LEDPwmType BrightLED = {
    .pwmChannelId = 1,       /* PWM channel index 1 trong IoHwAb0_PwmChannels */
    .pwmPeriod = 99,
    .outputCompareValue = 0
};



/* ========== IoHwAb Config tổng hợp ========== */
const IoHwAb0_ConfigType IoHwAb0_Config = {
    .IoHwAb0_PortCfg = &IoHwAb0_PortCfg,
    .IoHwAb0_AdcCfg  = &IoHwAb0_AdcCfg,
    .IoHwAb0_PwmCfg  = &IoHwAb0_PwmCfg,
    .tempSensor      = &LM35_Sensor,
    .coolingMotor    = &FanMotor,
    .potentiometer   = &Potentiometer,
    .brightLED       = &BrightLED,
    .ledChannel      = PORT_PIN_C13
};
