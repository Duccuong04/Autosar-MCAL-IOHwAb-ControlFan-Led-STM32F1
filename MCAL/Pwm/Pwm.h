/**********************************************************
 * @file    Pwm.h
 * @brief   Pulse Width Modulation (PWM) Driver Header File
 * @details File này chứa các định nghĩa về kiểu dữ liệu và 
 *          khai báo các API của PWM Driver tuân theo chuẩn AUTOSAR.
 *          Driver này được thiết kế để điều khiển Timer PWM của STM32F103 
 *          sử dụng thư viện SPL.
 * @version 2.0
 * @date    2024-07-03
 * @author  Nguyen Duc Cuong
 **********************************************************/

#ifndef PWM_H
#define PWM_H

#include "Std_Types.h"          /* Các kiểu dữ liệu chuẩn AUTOSAR */
#include "stm32f10x_tim.h"      /* Thư viện SPL: Timer PWM cho STM32F103 */
#include "misc.h"  // NVIC_InitTypeDef
#include "Dio.h"

/* Define sử dụng 12 chân phần cứng PWM (TIM2, TIM3, TIM4) STM32F1
 * TIM2_CH1	PA0	TIM2_CH1	
 * TIM2_CH2	PA1	TIM2_CH2	
 * TIM2_CH3	PA2	TIM2_CH3	
 * TIM2_CH4	PA3	TIM2_CH4
 * TIM3_CH1 PA6 hoặc PB4 (remap)
 * TIM3_CH2	PA7 hoặc PB5 (remap)		
 * TIM3_CH3	PB0	TIM3_CH3	
 * TIM3_CH4	PB1	TIM3_CH4
 * TIM4_CH1	PB6	TIM4_CH1	
 * TIM4_CH2	PB7	TIM4_CH2	
 * TIM4_CH3	PB8	TIM4_CH3	
 * TIM4_CH4	PB9	TIM4_CH4
 */
#define PWM_CHANNEL_TIM2_CH1   ((Pwm_ChannelType)0)
#define PWM_CHANNEL_TIM2_CH2   ((Pwm_ChannelType)1)
#define PWM_CHANNEL_TIM2_CH3   ((Pwm_ChannelType)2)
#define PWM_CHANNEL_TIM2_CH4   ((Pwm_ChannelType)3)

#define PWM_CHANNEL_TIM3_CH1   ((Pwm_ChannelType)4)
#define PWM_CHANNEL_TIM3_CH2   ((Pwm_ChannelType)5)
#define PWM_CHANNEL_TIM3_CH3   ((Pwm_ChannelType)6)
#define PWM_CHANNEL_TIM3_CH4   ((Pwm_ChannelType)7)

#define PWM_CHANNEL_TIM4_CH1   ((Pwm_ChannelType)8)
#define PWM_CHANNEL_TIM4_CH2   ((Pwm_ChannelType)9)
#define PWM_CHANNEL_TIM4_CH3   ((Pwm_ChannelType)10)
#define PWM_CHANNEL_TIM4_CH4   ((Pwm_ChannelType)11)

/**********************************************************
 * Định nghĩa các kiểu dữ liệu của PWM Driver
 **********************************************************/

/**********************************************************
 * @typedef Pwm_ChannelType
 * @brief   Kiểu dữ liệu cho một kênh PWM
 * @details Đây là kiểu định danh cho một kênh PWM cụ thể (0, 1, 2, ...)
 **********************************************************/
typedef uint8 Pwm_ChannelType;

/**********************************************************
 * @typedef Pwm_PeriodType
 * @brief   Kiểu dữ liệu cho chu kỳ của PWM (tính bằng tick)
 **********************************************************/
typedef uint16 Pwm_PeriodType;

/**********************************************************
 * @enum    Pwm_OutputStateType
 * @brief   Trạng thái đầu ra của kênh PWM (HIGH hoặc LOW)
 **********************************************************/
typedef enum {
    PWM_HIGH = 0x00,  /**< Đầu ra mức cao */
    PWM_LOW  = 0x01   /**< Đầu ra mức thấp */
} Pwm_OutputStateType;

/**********************************************************
 * @enum    Pwm_EdgeNotificationType
 * @brief   Loại cạnh để thông báo ngắt PWM
 **********************************************************/
typedef enum {
    PWM_RISING_EDGE  = 0x00,   /**< Cạnh lên */
    PWM_FALLING_EDGE = 0x01,   /**< Cạnh xuống */
    PWM_BOTH_EDGES   = 0x02    /**< Cả hai cạnh */
} Pwm_EdgeNotificationType;

/**********************************************************
 * @enum    Pwm_ChannelClassType
 * @brief   Kiểu kênh PWM (chu kỳ cố định, biến đổi, v.v.)
 **********************************************************/
typedef enum {
    PWM_VARIABLE_PERIOD      = 0x00,   /**< PWM period thay đổi được */
    PWM_FIXED_PERIOD         = 0x01,   /**< PWM period cố định */
    PWM_FIXED_PERIOD_SHIFTED = 0x02    /**< PWM period cố định, shifted */
} Pwm_ChannelClassType;

/**********************************************************
 * @struct  Pwm_ChannelConfigType
 * @brief   Cấu trúc cấu hình cho từng kênh PWM
 **********************************************************/
typedef struct {
    TIM_TypeDef*              TIMx;             /**< Timer sử dụng (TIM1, TIM2, ...) */
    Pwm_ChannelType           channel;          /**< Channel số (1, 2, 3, 4 tương ứng CH1, CH2, CH3, CH4) */
    Pwm_ChannelClassType      classType;        /**< Loại kênh */
    Pwm_PeriodType            defaultPeriod;    /**< Chu kỳ mặc định */
    uint16                    defaultDutyCycle; /**< Duty Cycle mặc định (0x0000 - 0x8000) */
    Pwm_OutputStateType       polarity;         /**< Đầu ra ban đầu */
    Pwm_OutputStateType       idleState;        /**< Trạng thái khi idle */
    uint8                   notificationEnable; /**< Cho phép ngắt ở chân Pwm hay không */
} Pwm_ChannelConfigType;

/**********************************************************
 * @struct  Pwm_ConfigType
 * @brief   Cấu trúc cấu hình tổng thể cho driver PWM
 **********************************************************/
typedef struct {
    const Pwm_ChannelConfigType* Channels;    /**< Danh sách các cấu hình kênh */
    uint8                        NumChannels; /**< Số lượng kênh PWM */
    void (*NotificationCb)(uint8);            /* Callback chung, tham số truyền vào ID của channel*/
} Pwm_ConfigType;

/**********************************************************
 * Khai báo các API của PWM Driver (chuẩn AUTOSAR)
 **********************************************************/

/**********************************************************
 * @brief   Khởi tạo PWM driver với cấu hình chỉ định
 * @param   ConfigPtr: Con trỏ tới cấu hình PWM
 **********************************************************/
void Pwm_Init(const Pwm_ConfigType* ConfigPtr);

/**********************************************************
 * @brief   Giải phóng tài nguyên và tắt tất cả kênh PWM
 **********************************************************/
void Pwm_DeInit(void);

/**********************************************************
 * @brief   Cài đặt duty cycle cho kênh PWM
 * @param   ChannelNumber: Số thứ tự kênh PWM
 * @param   DutyCycle: Tỷ lệ (0x0000 - 0x8000, ứng với 0%-100%)
 **********************************************************/
void Pwm_SetDutyCycle(Pwm_ChannelType ChannelNumber, uint16 DutyCycle);

/**********************************************************
 * @brief   Đặt period và duty cycle cho kênh PWM (nếu hỗ trợ)
 * @param   ChannelNumber: Số thứ tự kênh PWM
 * @param   Period: Chu kỳ PWM (tính bằng tick timer)
 * @param   DutyCycle: Duty cycle (0x0000 - 0x8000)
 **********************************************************/
void Pwm_SetPeriodAndDuty(Pwm_ChannelType ChannelNumber, Pwm_PeriodType Period, uint16 DutyCycle);

/**********************************************************
 * @brief   Đưa kênh PWM về trạng thái idle
 * @param   ChannelNumber: Số thứ tự kênh PWM
 **********************************************************/
void Pwm_SetOutputToIdle(Pwm_ChannelType ChannelNumber);

/**********************************************************
 * @brief   Đọc trạng thái đầu ra hiện tại của kênh PWM
 * @param   ChannelNumber: Số thứ tự kênh PWM
 * @return  PWM_HIGH hoặc PWM_LOW
 **********************************************************/
Pwm_OutputStateType Pwm_GetOutputState(Pwm_ChannelType ChannelNumber);

/**********************************************************
 * @brief   Tắt thông báo ngắt cho kênh PWM
 * @param   ChannelNumber: Số thứ tự kênh PWM
 **********************************************************/
void Pwm_DisableNotification(Pwm_ChannelType ChannelNumber);

/**********************************************************
 * @brief   Bật thông báo ngắt cạnh lên/xuống/cả 2 cho kênh PWM
 * @param   ChannelNumber: Số thứ tự kênh PWM
 * @param   Notification:  Loại cạnh cần thông báo
 **********************************************************/
void Pwm_EnableNotification(Pwm_ChannelType ChannelNumber, Pwm_EdgeNotificationType Notification);

/**********************************************************
 * @brief   Lấy thông tin phiên bản của driver PWM
 * @param   versioninfo: Con trỏ tới cấu trúc Std_VersionInfoType để nhận thông tin phiên bản
 **********************************************************/
void Pwm_GetVersionInfo(Std_VersionInfoType* versioninfo);

void TIM2_IRQHandler(void);

void Pwm_IsrHandler(TIM_TypeDef* TIMx);



#endif /* PWM_H */
