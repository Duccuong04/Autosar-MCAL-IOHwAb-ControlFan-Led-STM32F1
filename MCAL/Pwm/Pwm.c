/**********************************************************
 * @file    Pwm.c
 * @brief   Pulse Width Modulation (PWM) Driver Source File
 * @details Hiện thực các API của PWM Driver chuẩn AUTOSAR cho STM32F103, sử dụng SPL.
 *          Quản lý chức năng PWM, không cấu hình chân GPIO.
 * 
 * @version 2.0
 * @date    2025-07-03
 * @author  Nguyen Duc Cuong
 **********************************************************/

#include "Pwm.h"
#include "IoHwAb_Cfg.h"

/* ===============================
 *     Static Variables & Defines
 * =============================== */

/* Lưu trữ cấu hình driver PWM hiện tại */
static const Pwm_ConfigType* Pwm_CurrentConfigPtr = NULL;

/* Trạng thái đã khởi tạo của driver PWM */
static uint8 Pwm_IsInitialized = 0;

/* ===============================
 *        Function Definitions
 * =============================== */

/**********************************************************
 * @brief   Khởi tạo PWM driver với cấu hình chỉ định
 * @details Khởi tạo tất cả timer/kênh PWM theo cấu hình. Phần cấu hình chân GPIO phải thực hiện riêng.
 *
 * @param[in] ConfigPtr Con trỏ tới cấu hình PWM
 **********************************************************/
void Pwm_Init(const Pwm_ConfigType* ConfigPtr)
{
    /* Kiểm tra xem chân PWM được khởi tạo hay chưa*/
    if (Pwm_IsInitialized) return;
    if (ConfigPtr == NULL) return;

    Pwm_CurrentConfigPtr = ConfigPtr;
    for (uint8 i = 0; i < ConfigPtr->NumChannels; i++)
    {
        const Pwm_ChannelConfigType* channelConfig = &ConfigPtr->Channels[i];

        /* Cấu hình Timer*/
        TIM_TimeBaseInitTypeDef tim;
        tim.TIM_ClockDivision = TIM_CKD_DIV1;
        tim.TIM_CounterMode = TIM_CounterMode_Up;
        tim.TIM_Period = channelConfig->defaultPeriod;
        tim.TIM_Prescaler = 8 - 1;  /* 1us CNT dem len 1*/
        //tim.TIM_Prescaler = 8000 - 1;   /* 1ms */
        TIM_TimeBaseInit(channelConfig->TIMx, &tim);

        /* Giá trị compare ban đầu theo duty cycle mặc định */
        uint16_t compareValue = ((uint32_t)channelConfig->defaultPeriod * channelConfig->defaultDutyCycle) >> 15;

        /* Cấu hình PWM*/
        TIM_OCInitTypeDef oc;
        oc.TIM_OCMode = TIM_OCMode_PWM1;
        oc.TIM_OutputState = TIM_OutputState_Enable;
        oc.TIM_OCPolarity = TIM_OCPolarity_High;
        oc.TIM_Pulse       = compareValue;

       /* Channel ID PWM -> 4 CH CH1, CH2, CH3, CH4*/
        switch (channelConfig->channel) {
            case 1: TIM_OC1Init(channelConfig->TIMx, &oc); TIM_OC1PreloadConfig(channelConfig->TIMx, TIM_OCPreload_Enable); break;
            case 2: TIM_OC2Init(channelConfig->TIMx, &oc); TIM_OC2PreloadConfig(channelConfig->TIMx, TIM_OCPreload_Enable); break;
            case 3: TIM_OC3Init(channelConfig->TIMx, &oc); TIM_OC3PreloadConfig(channelConfig->TIMx, TIM_OCPreload_Enable); break;
            case 4: TIM_OC4Init(channelConfig->TIMx, &oc); TIM_OC4PreloadConfig(channelConfig->TIMx, TIM_OCPreload_Enable); break;
            default: break;
        }

        /* Khởi động TImer*/
        TIM_Cmd(channelConfig->TIMx, ENABLE);
    }

    Pwm_IsInitialized = 1;
}

/**********************************************************
 * @brief   Khởi tạo lại kênh PWM ở trạng thái IDLE
 * @details Dùng hàm OCStructInit trong SPL
 *          1. Khởi tạo lại chân PWM khi ở Idle State
 *          2. Đặt dutycycle = 0
 *          3. Đặt chân về IdleState (Polarity)
 *          4. Không dùng ngắt PWM (Notification)
 **********************************************************/
void Pwm_DeInit(void)
{
    /* Kiểm tra xem chân PWM được khởi tạo hay chưa*/
    if (!Pwm_IsInitialized || Pwm_CurrentConfigPtr == NULL) return;
    for (uint8 i = 0; i < Pwm_CurrentConfigPtr->NumChannels; i++)
    {
        const Pwm_ChannelConfigType* channelConfig = &Pwm_CurrentConfigPtr->Channels[i];

        /* Cấu hình lại output compare của chân PWM về Default*/
        TIM_OCInitTypeDef oc;

        /* Gọi OCStructInit để khởi tạo lại cấu hình về ban đầu*/
        TIM_OCStructInit(&oc);

        /* Điều chỉnh lại Polarity theo Idle State*/
        oc.TIM_OCPolarity = (channelConfig->idleState == PWM_HIGH) ? TIM_OCPolarity_High : TIM_OCPolarity_Low;

        /* Gán dutycycle = 0%*/
        oc.TIM_Pulse = 0;

        /* Gọi OCInit tương ứng để khởi tạo lại*/
        switch (channelConfig->channel) {
            case 1: TIM_OC1Init(channelConfig->TIMx, &oc); break;
            case 2: TIM_OC2Init(channelConfig->TIMx, &oc); break;
            case 3: TIM_OC3Init(channelConfig->TIMx, &oc); break;
            case 4: TIM_OC4Init(channelConfig->TIMx, &oc); break;
            default: break;
        }

        /* Dừng TIMER*/
        TIM_Cmd(channelConfig->TIMx, DISABLE);
    }
    Pwm_IsInitialized = 0;
    Pwm_CurrentConfigPtr = NULL;
}

/**********************************************************
 * @brief   Đặt duty cycle cho một kênh PWM
 * @details Chỉ thay đổi duty cycle, không thay đổi period.
 *
 * @param[in] ChannelNumber Số thứ tự kênh PWM
 * @param[in] DutyCycle     Duty cycle mới (0x0000 - 0x8000)
 **********************************************************/
void Pwm_SetDutyCycle(Pwm_ChannelType ChannelNumber, uint16 DutyCycle)
{
    /* Kiểm tra xem chân PWM được khởi tạo hay chưa & số ID của chân truyền vào có vượt quá 12*/
    if (!Pwm_IsInitialized || ChannelNumber >= Pwm_CurrentConfigPtr->NumChannels) return;

    /* Gọi struct channelConfig với ChannelNumber tương ứng*/
    const Pwm_ChannelConfigType* channelConfig = &Pwm_CurrentConfigPtr->Channels[ChannelNumber];

    /* Giá trị Period (ARR)*/
    uint16_t period = channelConfig->TIMx->ARR;

    /* Tính ra giá trị đếm Compare từ dutycycle muốn thay đổi và ARR*/
    uint16_t compareValue = ((uint32_t)period * DutyCycle) >> 15;
    switch (channelConfig->channel) {
    case 1: channelConfig->TIMx->CCR1 = compareValue; break;
    case 2: channelConfig->TIMx->CCR2 = compareValue; break;
    case 3: channelConfig->TIMx->CCR3 = compareValue; break;
    case 4: channelConfig->TIMx->CCR4 = compareValue; break;
    default: break;
    }
}

/**********************************************************
 * @brief   Đặt period và duty cycle cho một kênh PWM (nếu hỗ trợ)
 * @details Thay đổi đồng thời period (ARR) và duty cycle (CCR).
 *
 * @param[in] ChannelNumber Số thứ tự kênh PWM
 * @param[in] Period        Chu kỳ PWM mới (tick timer)
 * @param[in] DutyCycle     Duty cycle mới (0x0000 - 0x8000)
 **********************************************************/
void Pwm_SetPeriodAndDuty(Pwm_ChannelType ChannelNumber, Pwm_PeriodType Period, uint16 DutyCycle)
{
    /* Kiểm tra xem chân PWM được khởi tạo hay chưa & số ID của chân truyền vào có vượt quá 12*/
    if (!Pwm_IsInitialized || ChannelNumber >= Pwm_CurrentConfigPtr->NumChannels) return;

    /* Gọi struct channelConfig với ChannelNumber tương ứng*/
    const Pwm_ChannelConfigType* channelConfig = &Pwm_CurrentConfigPtr->Channels[ChannelNumber];

    /* Kiểm tra xem phần cài đặt có cho phép thay đổi*/
    if (channelConfig->classType != PWM_VARIABLE_PERIOD) return;

    /* Thay đổi giá trị Period*/
    channelConfig->TIMx->ARR = Period;

    /* Tính ra giá trị đếm Compare từ dutycycle muốn thay đổi và ARR*/
    uint16_t compareValue = ((uint32_t)Period * DutyCycle) >> 15;
    switch (channelConfig->channel) {
    case 1: channelConfig->TIMx->CCR1 = compareValue; break;
    case 2: channelConfig->TIMx->CCR2 = compareValue; break;
    case 3: channelConfig->TIMx->CCR3 = compareValue; break;
    case 4: channelConfig->TIMx->CCR4 = compareValue; break;
    default: break;
    }
}

/**********************************************************
 * @brief   Đưa kênh PWM về trạng thái idle (tắt output)
 **********************************************************/
void Pwm_SetOutputToIdle(Pwm_ChannelType ChannelNumber)
{
    if (!Pwm_IsInitialized || ChannelNumber >= Pwm_CurrentConfigPtr->NumChannels) return;

    const Pwm_ChannelConfigType* channelConfig = &Pwm_CurrentConfigPtr->Channels[ChannelNumber];

    /* Cấu hình lại Output Compare trạng thái idle*/
    TIM_OCInitTypeDef oc;
    TIM_OCStructInit(&oc); // Khởi tạo giá trị mặc định

    oc.TIM_OCMode = TIM_OCMode_PWM1;
    oc.TIM_OutputState = TIM_OutputState_Enable;
    oc.TIM_Pulse = 0;

    /* Trạng thái idle PWM_HIGH / PWM_LOW sang polarity của kênh PWM cần SetOutputToIdle*/
    oc.TIM_OCPolarity = (channelConfig->idleState == PWM_HIGH) ? TIM_OCPolarity_High : TIM_OCPolarity_Low;

    switch (channelConfig->channel) {
        case 1: TIM_OC1Init(channelConfig->TIMx, &oc); break;
        case 2: TIM_OC2Init(channelConfig->TIMx, &oc); break;
        case 3: TIM_OC3Init(channelConfig->TIMx, &oc); break;
        case 4: TIM_OC4Init(channelConfig->TIMx, &oc); break;
        default: break;
    }
}


/**********************************************************
 * @brief   Đọc trạng thái đầu ra hiện tại của kênh PWM, sử dụng DIO_ReadChannel()
 *          dựa vào bảng ánh xạ từ Pwm channel sang DIO channel
 * @return  PWM_HIGH nếu compare enable, ngược lại PWM_LOW
 **********************************************************/
Pwm_OutputStateType Pwm_GetOutputState(Pwm_ChannelType ChannelNumber)
{
    /* Kiểm tra ChannelNumber nhập vào có hợp lệ k và Pwm được khởi tạo hay chưa*/
    if (!Pwm_IsInitialized || ChannelNumber >= Pwm_CurrentConfigPtr->NumChannels)
        return PWM_LOW;
    /* Map từ channel của Pwm sang Dio*/
    Dio_ChannelType dioChannel = PwmToDioChannelMap[ChannelNumber];

    /*Đọc mức logic điện áp*/
    Dio_LevelType level = Dio_ReadChannel(dioChannel);

    return (level == STD_HIGH) ? PWM_HIGH : PWM_LOW;
}

/**********************************************************
 * @brief   Tắt thông báo ngắt cho kênh PWM
 * @details Nếu sử dụng callback hoặc ngắt, tắt tại đây.
 *
 * @param[in] ChannelNumber Số thứ tự kênh PWM
 **********************************************************/
void Pwm_DisableNotification(Pwm_ChannelType ChannelNumber)
{
    if (!Pwm_IsInitialized || ChannelNumber >= Pwm_CurrentConfigPtr->NumChannels) return;
    const Pwm_ChannelConfigType* channelConfig = &Pwm_CurrentConfigPtr->Channels[ChannelNumber];
    switch (channelConfig->channel) {
    case 1: TIM_ITConfig(channelConfig->TIMx, TIM_IT_CC1, DISABLE); break;
    case 2: TIM_ITConfig(channelConfig->TIMx, TIM_IT_CC2, DISABLE); break;
    case 3: TIM_ITConfig(channelConfig->TIMx, TIM_IT_CC3, DISABLE); break;
    case 4: TIM_ITConfig(channelConfig->TIMx, TIM_IT_CC4, DISABLE); break;
    default: break;
    }
}

/**********************************************************
 * @brief   Bật thông báo ngắt cạnh lên/xuống/cả 2 cho kênh PWM
 * @details Nếu sử dụng callback hoặc ngắt, bật tại đây.
 *
 * @param[in] ChannelNumber Số thứ tự kênh PWM
 * @param[in] Notification  Loại cạnh cần thông báo
 **********************************************************/
void Pwm_EnableNotification(Pwm_ChannelType ChannelNumber, Pwm_EdgeNotificationType Notification)
{
    if (!Pwm_IsInitialized || ChannelNumber >= Pwm_CurrentConfigPtr->NumChannels) return;
    const Pwm_ChannelConfigType* cfg = &Pwm_CurrentConfigPtr->Channels[ChannelNumber];

    //cfg->notificationEnable = 1;  // Đã cài đặt khi khởi tạo

    TIM_TypeDef* TIMx = cfg->TIMx;

    /* Flag cho channel tương ứng*/
    uint16_t cc_flag = TIM_IT_CC1 << (cfg->channel - 1);  

    /* Cấu hình ngắt cạnh theo yêu cầu*/
    // if (Notification & PWM_RISING_EDGE)
    //     TIM_ITConfig(TIMx, cc_flag, ENABLE);
    // if (Notification & PWM_FALLING_EDGE)
    // TIM_ITConfig(TIMx, TIM_IT_Update, ENABLE);

    TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);

    /* Xác định IRQn tương ứng để bật NVIC*/ 
    IRQn_Type irq = (TIMx == TIM2) ? TIM2_IRQn :
                    (TIMx == TIM3) ? TIM3_IRQn :
                    (TIMx == TIM4) ? TIM4_IRQn : (IRQn_Type)0xFF;

    /* Cấu hình NVIC cho ngắt Timer*/
    if (irq != (IRQn_Type)0xFF)
    {
        NVIC_InitTypeDef nvic;
        nvic.NVIC_IRQChannel = (uint8_t)irq;
        nvic.NVIC_IRQChannelPreemptionPriority = 0;
        nvic.NVIC_IRQChannelSubPriority = 0;
        nvic.NVIC_IRQChannelCmd = ENABLE;
        NVIC_Init(&nvic);
    }   
}

/**********************************************************
 * @brief   Hàm xử lý ngắt chung cho tất cả các TIMx, được gọi từ TIMx_IRQHandler
 * @param   TIMx  Địa chỉ timer xảy ra ngắt
 **********************************************************/
void Pwm_IsrHandler(TIM_TypeDef *TIMx)
{
    for (uint8_t i = 0; i < IoHwAb0_PwmCfg.NumChannels; i++)
    {
        /* Trỏ đến cấu hình từng channel để xử lý*/
        const Pwm_ChannelConfigType* cfg = &IoHwAb0_PwmCfg.Channels[i];

        /* Kiểm tra có thuộc ngắt của Timer truyền vào ko, lọc bớt các channel của Timer khác*/
        if (cfg->TIMx != TIMx) continue;

        boolean triggered = FALSE;

        /* Chỉ xử lý từ CH1 -> CH4*/
        if (cfg->channel >= 1 && cfg->channel <= 4)
        {
            /* Tạo mask
             * CH1 → TIM_IT_CC1
             * CH2 → TIM_IT_CC2
             * CH3 → TIM_IT_CC3
             * CH4 → TIM_IT_CC4
             */
            uint16_t cc_flag = TIM_IT_CC1 << (cfg->channel - 1);

            /* Xóa cờ ngắt Capture compare*/
            if (TIM_GetITStatus(TIMx, cc_flag) != RESET)
            {
                TIM_ClearITPendingBit(TIMx, cc_flag);
                triggered = TRUE;
            }
        }
        
        /* Xóa cờ ngắt Pending*/
        if (TIM_GetITStatus(TIMx, TIM_IT_Update) != RESET)
        {
            TIM_ClearITPendingBit(TIMx, TIM_IT_Update);
            triggered = TRUE;
        }

        /* Gọi callback nếu có ngắt xảy ra và chân đó có enable ngắt*/
        if (triggered && cfg->notificationEnable && IoHwAb0_PwmCfg.NotificationCb)
        {
            IoHwAb0_PwmCfg.NotificationCb(cfg->channel);  
        }
    }
}

/**********************************************************
 * @brief   Lấy thông tin phiên bản của driver PWM
 * @details Trả về thông tin phiên bản module PWM.
 *
 * @param[out] versioninfo Con trỏ tới Std_VersionInfoType để nhận version
 **********************************************************/
void Pwm_GetVersionInfo(Std_VersionInfoType* versioninfo)
{
    if (versioninfo == NULL) return;
    versioninfo->vendorID = 0x1234;
    versioninfo->moduleID = 0xABCD;
    versioninfo->sw_major_version = 1;
    versioninfo->sw_minor_version = 0;
    versioninfo->sw_patch_version = 0;
}
