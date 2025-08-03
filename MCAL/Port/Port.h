/**********************************************************
 * @file    Port.h
 * @brief   Port Driver Header File (AUTOSAR Standard)
 * @details File này chứa các định nghĩa về kiểu dữ liệu,
 *          macro ánh xạ GPIO, và khai báo các API chuẩn
 *          AUTOSAR cho Port Driver. Driver này được thiết
 *          kế cho STM32F103 sử dụng SPL.
 * @version 2.0
 * @date    2025-07-05
 * @author  Nguyen Duc Cuong
 **********************************************************/

#ifndef PORT_H
#define PORT_H

#include "Std_Types.h"         /* Các kiểu dữ liệu chuẩn AUTOSAR */
#include "stm32f10x_gpio.h"    /* Thư viện chuẩn của STM32F103 */

/**********************************************************
 * Định nghĩa các giá trị PortId cho các cổng GPIO
 **********************************************************/
#define PORT_ID_A   0   /* GPIOA */
#define PORT_ID_B   1   /* GPIOB */
#define PORT_ID_C   2   /* GPIOC */
#define PORT_ID_D   3   /* GPIOD */

/**********************************************************
 * Macro xác định con trỏ PORT theo PortNum
 **********************************************************/
#define PORT_GET_PORT(PortNum) \
    ((PortNum) == PORT_ID_A ? GPIOA : \
    (PortNum) == PORT_ID_B ? GPIOB : \
    (PortNum) == PORT_ID_C ? GPIOC : \
    (PortNum) == PORT_ID_D ? GPIOD : NULL)

/**********************************************************
 * Macro xác định Pin Mask cho từng chân GPIO (0-15)
 **********************************************************/
#define PORT_GET_PORT_NUM(pin)   ((pin) / 16U)
#define PORT_GET_PIN_NUM(pin)    ((pin) % 16U)
#define PORT_GET_PIN_MASK(pin)   ((uint16_t)(1U << PORT_GET_PIN_NUM(pin)))

/**********************************************************
 * Định nghĩa các chế độ mode và trạng thái, tốc độ, Port Type cho pin
 **********************************************************/
#define PORT_PIN_MODE_DIO       0
#define PORT_PIN_MODE_ADC       1
#define PORT_PIN_MODE_PWM       2
#define PORT_PIN_MODE_SPI       3
#define PORT_PIN_MODE_CAN       4
#define PORT_PIN_MODE_LIN       5

#define PORT_PIN_PULL_NONE      0
#define PORT_PIN_PULL_UP        1
#define PORT_PIN_PULL_DOWN      2

#define PORT_PIN_LEVEL_LOW      0
#define PORT_PIN_LEVEL_HIGH     1

#define SPEED_SMALL             0
#define SPEED_MEDIUM            1
#define SPEED_HIGH              2

/*Define Port để cấu hình*/
#define PORTA 0
#define PORTB 1
#define PORTC 2
#define PORTD 3

/*Define Port Pin A0 - A15 - Pin 0 -> 16*/
#define PORT_PIN_A0   PORT_PIN(PORTA, 0)
#define PORT_PIN_A1   PORT_PIN(PORTA, 1)
#define PORT_PIN_A2   PORT_PIN(PORTA, 2) 
#define PORT_PIN_A3   PORT_PIN(PORTA, 3)
#define PORT_PIN_A4   PORT_PIN(PORTA, 4)
#define PORT_PIN_A5   PORT_PIN(PORTA, 5)
#define PORT_PIN_A6   PORT_PIN(PORTA, 6)
#define PORT_PIN_A7   PORT_PIN(PORTA, 7)
#define PORT_PIN_A8   PORT_PIN(PORTA, 8) 
#define PORT_PIN_A9   PORT_PIN(PORTA, 9)
#define PORT_PIN_A10  PORT_PIN(PORTA, 10)
#define PORT_PIN_A11  PORT_PIN(PORTA, 11)
#define PORT_PIN_A12  PORT_PIN(PORTA, 12)
#define PORT_PIN_A13  PORT_PIN(PORTA, 13)
#define PORT_PIN_A14  PORT_PIN(PORTA, 14) 
#define PORT_PIN_A15  PORT_PIN(PORTA, 15)

/*Define Port Pin B0 - B15 - Pin 17 -> 32*/
#define PORT_PIN_B0   PORT_PIN(PORTB, 0)
#define PORT_PIN_B1   PORT_PIN(PORTB, 1)
#define PORT_PIN_B2   PORT_PIN(PORTB, 2) 
#define PORT_PIN_B3   PORT_PIN(PORTB, 3)
#define PORT_PIN_B4   PORT_PIN(PORTB, 4)
#define PORT_PIN_B5   PORT_PIN(PORTB, 5)
#define PORT_PIN_B6   PORT_PIN(PORTB, 6)
#define PORT_PIN_B7   PORT_PIN(PORTB, 7)
#define PORT_PIN_B8   PORT_PIN(PORTB, 8) 
#define PORT_PIN_B9   PORT_PIN(PORTB, 9)
#define PORT_PIN_B10  PORT_PIN(PORTB, 10)
#define PORT_PIN_B11  PORT_PIN(PORTB, 11)
#define PORT_PIN_B12  PORT_PIN(PORTB, 12)
#define PORT_PIN_B13  PORT_PIN(PORTB, 13)
#define PORT_PIN_B14  PORT_PIN(PORTB, 14) 
#define PORT_PIN_B15  PORT_PIN(PORTB, 15)

/*Define Port Pin C0 - C15 - Pin 33 -> 47*/
#define PORT_PIN_C0   PORT_PIN(PORTC, 0)
#define PORT_PIN_C1   PORT_PIN(PORTC, 1)
#define PORT_PIN_C2   PORT_PIN(PORTC, 2) 
#define PORT_PIN_C3   PORT_PIN(PORTC, 3)
#define PORT_PIN_C4   PORT_PIN(PORTC, 4)
#define PORT_PIN_C5   PORT_PIN(PORTC, 5)
#define PORT_PIN_C6   PORT_PIN(PORTC, 6)
#define PORT_PIN_C7   PORT_PIN(PORTC, 7)
#define PORT_PIN_C8   PORT_PIN(PORTC, 8) 
#define PORT_PIN_C9   PORT_PIN(PORTC, 9)
#define PORT_PIN_C10  PORT_PIN(PORTC, 10)
#define PORT_PIN_C11  PORT_PIN(PORTC, 11)
#define PORT_PIN_C12  PORT_PIN(PORTC, 12)
#define PORT_PIN_C13  PORT_PIN(PORTC, 13)
#define PORT_PIN_C14  PORT_PIN(PORTC, 14) 
#define PORT_PIN_C15  PORT_PIN(PORTC, 15)

/*ChannelID Pin cụ thể*/
#define PORT_PIN(PORT, PIN)   ((Port_PinType)(((PORT) * 16U) + (PIN)))

/**********************************************************
 * Định nghĩa kiểu dữ liệu của Port Driver AUTOSAR
 **********************************************************/

/**
 * @typedef Port_PinType
 * @brief   Kiểu định danh cho một chân Port (0..47: A0..A15, B0..B15, C0..C15)
 *          Khác với bên DIO driver, Port dùng kiểu dữ liệu này để cấu hình Pin
 */
typedef uint8 Port_PinType;

/**
 * @typedef Port_PinDirectionType
 * @brief   Enum xác định chiều vào/ra của pin
 */
typedef enum {
    PORT_PIN_IN  = 0x00,   /**< Input  */
    PORT_PIN_OUT = 0x01    /**< Output */
} Port_PinDirectionType;

/**
 * @typedef Port_PinModeType
 * @brief   Kiểu định danh cho mode chức năng của pin (DIO, ADC, PWM...)
 */
typedef uint8 Port_PinModeType;

/**
 * @struct Port_PinConfigType
 * @brief  Cấu hình cho từng chân pin (phần mềm sẽ sinh theo tool/hoặc config tay)
 */
typedef struct {
    Port_PinType Pin; 
    Port_PinModeType Mode;     /**< Chế độ chức năng */
    uint8 Speed;
    Port_PinDirectionType Direction;  /**< Chiều ban đầu */
    uint8  DirectionChangeable;/**< 1=cho phép đổi chiều runtime */
    uint8  Level;              /**< Giá trị mức ban đầu nếu output */
    uint8  Pull;               /**< Kiểu pull: none, up, down */
    uint8  ModeChangeable;     /**< 1=cho phép đổi mode runtime */
} Port_PinConfigType;

/**
 * @struct Port_ConfigType
 * @brief  Cấu trúc tổng hợp cấu hình tất cả các pin (gán khi init)
 */
typedef struct {
    const Port_PinConfigType* PinConfigs; /**< Con trỏ tới mảng cấu hình pin */
    uint16 PinCount;                      /**< Số lượng chân cấu hình */
} Port_ConfigType;

/**********************************************************
 * Macro định nghĩa phiên bản, vendor, module ID cho VersionInfo
 **********************************************************/
#define PORT_VENDOR_ID          1234U
#define PORT_MODULE_ID          81U
#define PORT_SW_MAJOR_VERSION   1U
#define PORT_SW_MINOR_VERSION   0U
#define PORT_SW_PATCH_VERSION   0U

/**********************************************************
 * Khai báo prototype các hàm API Port Driver AUTOSAR
 **********************************************************/

/**
 * @brief   Khởi tạo toàn bộ các Port/Pin
 * @param[in] ConfigPtr Con trỏ tới cấu hình port/pin
 */
void Port_Init(const Port_ConfigType* ConfigPtr);

/**
 * @brief   Đổi chiều một chân Port (nếu được phép)
 * @param[in] Pin        Số hiệu pin
 * @param[in] Direction  Chiều cần đặt
 */
void Port_SetPinDirection(Port_PinType Pin, Port_PinDirectionType Direction);

/**
 * @brief   Làm tươi lại chiều tất cả các pin không cho đổi chiều runtime
 */
void Port_RefreshPortDirection(void);

/**
 * @brief   Lấy thông tin version của Port Driver
 * @param[out] versioninfo  Con trỏ tới cấu trúc Std_VersionInfoType để nhận version
 */
void Port_GetVersionInfo(Std_VersionInfoType* versioninfo);

/**
 * @brief   Đổi mode chức năng cho một chân pin (nếu cho phép)
 * @param[in] Pin    Số hiệu pin
 * @param[in] Mode   Mode chức năng cần set
 */
void Port_SetPinMode(Port_PinType Pin, Port_PinModeType Mode);

#endif /* PORT_H */
