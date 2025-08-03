/*======== startup_stm32f103.s ===========
      - Định nghĩa vector table cho STM32F103
      - Copy .data từ Flash vào RAM, clear .bss
      - Gọi main(), vào vòng lặp vô hạn nếu main() trả về
    ==========================================*/

    .syntax unified
    .cpu cortex-m3
    .thumb

/* ========= Vector Table ========= */
    .section .isr_vector, "a", %progbits
    .align  2
    .type   g_pfnVectors, %object
    .size   g_pfnVectors, .-g_pfnVectors

g_pfnVectors:
    .word   _estack                 /* 0x00: Initial Stack Pointer */
    .word   Reset_Handler           /* 0x04: Reset Handler */
    .word   NMI_Handler             /* 0x08: NMI Handler */
    .word   HardFault_Handler       /* 0x0C: HardFault Handler */
    .word   MemManage_Handler       /* 0x10: MemManage Handler */
    .word   BusFault_Handler        /* 0x14: BusFault Handler */
    .word   UsageFault_Handler      /* 0x18: UsageFault Handler */
    .word   0                        /* 0x1C: Reserved */
    .word   0                        /* 0x20: Reserved */
    .word   0                        /* 0x24: Reserved */
    .word   0                        /* 0x28: Reserved */
    .word   SVC_Handler             /* 0x2C: SVCall Handler */
    .word   DebugMon_Handler        /* 0x30: DebugMon Handler */
    .word   0                        /* 0x34: Reserved */
    .word   PendSV_Handler          /* 0x38: PendSV Handler */
    .word   SysTick_Handler         /* 0x3C: SysTick Handler */

    /* External Interrupts */
    .word  WWDG_IRQHandler
    .word  PVD_IRQHandler
    .word  TAMPER_IRQHandler
    .word  RTC_IRQHandler
    .word  FLASH_IRQHandler
    .word  RCC_IRQHandler
    .word  EXTI0_IRQHandler
    .word  EXTI1_IRQHandler
    .word  EXTI2_IRQHandler
    .word  EXTI3_IRQHandler
    .word  EXTI4_IRQHandler
    .word  DMA1_Channel1_IRQHandler
    .word  DMA1_Channel2_IRQHandler
    .word  DMA1_Channel3_IRQHandler
    .word  DMA1_Channel4_IRQHandler
    .word  DMA1_Channel5_IRQHandler
    .word  DMA1_Channel6_IRQHandler
    .word  DMA1_Channel7_IRQHandler
    .word  ADC1_2_IRQHandler
    .word  USB_HP_CAN_TX_IRQHandler
    .word  USB_LP_CAN_RX0_IRQHandler
    .word  CAN_RX1_IRQHandler
    .word  CAN_SCE_IRQHandler
    .word  EXTI9_5_IRQHandler
    .word  TIM1_BRK_IRQHandler
    .word  TIM1_UP_IRQHandler
    .word  TIM1_TRG_COM_IRQHandler
    .word  TIM1_CC_IRQHandler
    .word  TIM2_IRQHandler
    .word  TIM3_IRQHandler
    .word  TIM4_IRQHandler
    .word  I2C1_EV_IRQHandler
    .word  I2C1_ER_IRQHandler
    .word  I2C2_EV_IRQHandler
    .word  I2C2_ER_IRQHandler
    .word  SPI1_IRQHandler
    .word  SPI2_IRQHandler
    .word  USART1_IRQHandler
    .word  USART2_IRQHandler
    .word  USART3_IRQHandler
    .word  EXTI15_10_IRQHandler
    .word  RTCAlarm_IRQHandler
    .word  USBWakeUp_IRQHandler

Default_Handler:
  b .

    .text
    .thumb
    /* Weak aliases for each Exception handler to the Default_Handler */
    .weak  NMI_Handler
    .set   NMI_Handler, Default_Handler

    .weak  HardFault_Handler
    .set   HardFault_Handler, Default_Handler

    .weak  MemManage_Handler
    .set   MemManage_Handler, Default_Handler

    .weak  BusFault_Handler
    .set   BusFault_Handler, Default_Handler

    .weak  UsageFault_Handler
    .set   UsageFault_Handler, Default_Handler

    .weak  SVC_Handler
    .set   SVC_Handler, Default_Handler

    .weak  DebugMon_Handler
    .set   DebugMon_Handler, Default_Handler

    .weak  PendSV_Handler
    .set   PendSV_Handler, Default_Handler

    .weak  SysTick_Handler
    .set   SysTick_Handler, Default_Handler

    .weak  WWDG_IRQHandler
    .set   WWDG_IRQHandler, Default_Handler
    .weak  PVD_IRQHandler
    .set   PVD_IRQHandler, Default_Handler
    .weak  TAMPER_IRQHandler
    .set   TAMPER_IRQHandler, Default_Handler
    .weak  RTC_IRQHandler
    .set   RTC_IRQHandler, Default_Handler
    .weak  FLASH_IRQHandler
    .set   FLASH_IRQHandler, Default_Handler
    .weak  RCC_IRQHandler
    .set   RCC_IRQHandler, Default_Handler
    .weak  EXTI0_IRQHandler
    .set   EXTI0_IRQHandler, Default_Handler
    .weak  EXTI1_IRQHandler
    .set   EXTI1_IRQHandler, Default_Handler
    .weak  EXTI2_IRQHandler
    .set   EXTI2_IRQHandler, Default_Handler
    .weak  EXTI3_IRQHandler
    .set   EXTI3_IRQHandler, Default_Handler
    .weak  EXTI4_IRQHandler
    .set   EXTI4_IRQHandler, Default_Handler
    .weak  DMA1_Channel1_IRQHandler
    .set   DMA1_Channel1_IRQHandler, Default_Handler
    .weak  DMA1_Channel2_IRQHandler
    .set   DMA1_Channel2_IRQHandler, Default_Handler
    .weak  DMA1_Channel3_IRQHandler
    .set   DMA1_Channel3_IRQHandler, Default_Handler
    .weak  DMA1_Channel4_IRQHandler
    .set   DMA1_Channel4_IRQHandler, Default_Handler
    .weak  DMA1_Channel5_IRQHandler
    .set   DMA1_Channel5_IRQHandler, Default_Handler
    .weak  DMA1_Channel6_IRQHandler
    .set   DMA1_Channel6_IRQHandler, Default_Handler
    .weak  DMA1_Channel7_IRQHandler
    .set   DMA1_Channel7_IRQHandler, Default_Handler
    .weak  ADC1_2_IRQHandler
    .set   ADC1_2_IRQHandler, Default_Handler
    .weak  USB_HP_CAN_TX_IRQHandler
    .set   USB_HP_CAN_TX_IRQHandler, Default_Handler
    .weak  USB_LP_CAN_RX0_IRQHandler
    .set   USB_LP_CAN_RX0_IRQHandler, Default_Handler
    .weak  CAN_RX1_IRQHandler
    .set   CAN_RX1_IRQHandler, Default_Handler
    .weak  CAN_SCE_IRQHandler
    .set   CAN_SCE_IRQHandler, Default_Handler
    .weak  EXTI9_5_IRQHandler
    .set   EXTI9_5_IRQHandler, Default_Handler
    .weak  TIM1_BRK_IRQHandler
    .set   TIM1_BRK_IRQHandler, Default_Handler
    .weak  TIM1_UP_IRQHandler
    .set   TIM1_UP_IRQHandler, Default_Handler
    .weak  TIM1_TRG_COM_IRQHandler
    .set   TIM1_TRG_COM_IRQHandler, Default_Handler
    .weak  TIM1_CC_IRQHandler
    .set   TIM1_CC_IRQHandler, Default_Handler
    .weak  TIM2_IRQHandler
    .set   TIM2_IRQHandler, Default_Handler
    .weak  TIM3_IRQHandler
    .set   TIM3_IRQHandler, Default_Handler
    .weak  TIM4_IRQHandler
    .set   TIM4_IRQHandler, Default_Handler
    .weak  I2C1_EV_IRQHandler
    .set   I2C1_EV_IRQHandler, Default_Handler
    .weak  I2C1_ER_IRQHandler
    .set   I2C1_ER_IRQHandler, Default_Handler
    .weak  I2C2_EV_IRQHandler
    .set   I2C2_EV_IRQHandler, Default_Handler
    .weak  I2C2_ER_IRQHandler
    .set   I2C2_ER_IRQHandler, Default_Handler
    .weak  SPI1_IRQHandler
    .set   SPI1_IRQHandler, Default_Handler
    .weak  SPI2_IRQHandler
    .set   SPI2_IRQHandler, Default_Handler
    .weak  USART1_IRQHandler
    .set   USART1_IRQHandler, Default_Handler
    .weak  USART2_IRQHandler
    .set   USART2_IRQHandler, Default_Handler
    .weak  USART3_IRQHandler
    .set   USART3_IRQHandler, Default_Handler
    .weak  EXTI15_10_IRQHandler
    .set   EXTI15_10_IRQHandler, Default_Handler
    .weak  RTCAlarm_IRQHandler
    .set   RTCAlarm_IRQHandler, Default_Handler
    .weak  USBWakeUp_IRQHandler
    .set   USBWakeUp_IRQHandler, Default_Handler

/* ========= Reset Handler ========= */
    .section .text.Reset_Handler, "ax", %progbits
    .weak   Reset_Handler
    .type   Reset_Handler, %function
Reset_Handler:
    /* 1/ Copy .data từ Flash sang RAM */
    LDR   R0, =_sidata      /* _sidata = địa chỉ đầu của vùng gốc .data trong Flash */
    LDR   R1, =_sdata       /* _sdata = địa chỉ đầu vùng .data trong RAM */
    LDR   R2, =_edata       /* _edata = địa chỉ kết thúc vùng .data trong RAM */
copy_data_loop:
    CMP   R1, R2            /* nếu R1 >= R2 thì dừng */
    ITT   LT
    LDRLT R3, [R0], #4      /* load 4 byte tại R0, R0 += 4 */
    STRLT R3, [R1], #4      /* store 4 byte vào R1, R1 += 4 */
    BLT   copy_data_loop

    /* 2/ Clear .bss (set 0) */
    LDR   R0, =_sbss        /* _sbss = địa chỉ đầu của vùng .bss trong RAM */
    LDR   R1, =_ebss        /* _ebss = địa chỉ kết thúc vùng .bss trong RAM */
    MOV   R2, #0
clear_bss_loop:
    CMP   R0, R1            /* nếu R0 >= R1 thì dừng */
    ITT   LT
    STRLT R2, [R0], #4      /* store 0 vào [R0], R0 += 4 */
    BLT   clear_bss_loop

    /* 3/ Gọi hàm main() */
    BL    main

    /* 4/ Nếu main() trả về, vào vòng lặp vô hạn */
infinite_loop:
    B    infinite_loop

    .size Reset_Handler, .-Reset_Handler