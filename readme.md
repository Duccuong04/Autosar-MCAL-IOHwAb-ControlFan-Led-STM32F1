# Mini Project: AUTOSAR Classic – Design MCAL (I/O Drivers) & IoHwAb Layer for Fan Speed & LED Brightness Control with LM35DZ and Potentiometer on STM32F1

# Link video demo: https://youtu.be/J_zCwrdbbQ8

## **1. Mục tiêu dự án**

- Xây dựng một mini project theo phong cách AUTOSAR Classic Platform, triển khai IoHardware Abstraction (IoHwAb) trên STM32F103.

- Đọc nhiệt độ từ cảm biến LM35DZ qua ADC DMA và điều khiển tốc độ quạt thông qua PWM với MOSFET và điều chỉnh độ sáng LED qua giá trị đọc được từ biến trở

- Kiến trúc AUTOSAR Classic: Application → IoHwAb → MCAL (I/O Drivers) → Hardware.

- Viết driver IoHwAb và sử dụng MCAL Drivers (ADC, PWM, DIO, Port) theo chuẩn AUTOSAR.

- Kỹ năng tích hợp phần mềm với phần cứng nhúng thực tế, bao gồm biến trở, cảm biến nhiệt LM35DZ, MOSFET, quạt DC và LED.
---

## **2. Giới thiệu lớp IoHwAb trong AUTOSAR**

Lớp **IoHardware Abstraction (IoHwAb)**:
- Là cầu nối giữa **Application layer** và **MCAL layer**.
- Giúp **tách biệt ứng dụng với phần cứng**:
  - Ứng dụng không cần quan tâm tới chân GPIO, ADC channel, hay timer cụ thể.
  - Khi thay đổi phần cứng, chỉ cần chỉnh **MCAL/IoHwAb** mà không phải sửa **Application**.
- Cung cấp API mức cao cho **App**, ví dụ:
  - `IoHwAb0_GetTemperature()` – đọc nhiệt độ từ LM35.
  - `IoHwAb0_SetFanSpeed()` – đặt tốc độ quạt theo % duty.
  - `IoHwAb0_ControlLedBrightness()` – điều khiển độ sáng LED dựa trên biến trở.
- Dựa trên tài liệu **[AUTOSAR CP SWS IoHwAb](https://www.autosar.org/fileadmin/standards/R24-11/CP/AUTOSAR_CP_SWS_IOHardwareAbstraction.pdf)**.

---

## **3. Sơ đồ luồng phần mềm AUTOSAR**

```
+-------------------+
|   Application     |
| (Main Function)   |
+-------------------+
          |
          v
+-------------------+
|      IoHwAb       |  <-- IoHardware Abstraction Layer
|  - GetTemperature |
|  - SetFanSpeed    |
|  - ControlLED     |
+-------------------+
          |
          v
+-------------------+
|       MCAL        |  <-- Microcontroller Abstraction Layer
|  - Dio / Port     |
|  - Adc (DMA)      |
|  - Pwm            |
+-------------------+
          |
          v
+-------------------+
|      Hardware     |
|  STM32 + LM35 +   |
|  Fan + MOSFET     |
+-------------------+
```

---

## **4. Phần cứng sử dụng**

- **MCU:** STM32F103C8T6 (ARM Cortex-M3)
- **Cảm biến nhiệt độ:** LM35DZ
  - 10 mV/°C, nguồn 5V, tín hiệu analog 0–1V cho 0–100°C.
- **MOSFET công suất:** **IRLZ44N**
  - **Logic-level MOSFET**, có thể đóng cắt ở mức 3.3V từ STM32.
  - Điều khiển quạt 12V với dòng tải vài trăm mA.
  - Điện trở kéo xuống Gate: **10 kΩ** để tránh MOSFET tự mở khi MCU reset.
- **Quạt DC 12V** điều khiển bằng **PWM 1 kHz**.
- **Biến trở** để test điều khiển LED sáng/tắt theo giá trị ADC.

---

## **5. Cấu trúc dự án**

```
PROJECT_FANCONTROL_AUTOSAR/
│
├── .vscode/                     # Cấu hình VSCode
│   ├── c_cpp_properties.json
│   ├── launch.json
│   ├── settings.json
│   ├── tasks.json
│   └── exchange.txt
│
├── CMSIS/                        # Thư viện CMSIS cho STM32
│
├── Common/                       # Các file chung
│   └── Std_Types.h
│
├── Config/                       # File cấu hình IoHwAb
│   ├── IoHwAb_Cfg.c
│   └── IoHwAb_Cfg.h
│
├── Hardware/                     # Low-level Hardware / ISR
│   ├── Adc_hw.h
│   └── isr.c
│
├── IoHwAb/                       # IoHardware Abstraction Layer
│   ├── IoHwAb.c
│   └── IoHwAb.h
│
├── Linker/                       # File linker script
│
├── MCAL/                         # Microcontroller Abstraction Layer
│   ├── Adc/
│   │   ├── Adc.c
│   │   └── Adc.h
│   ├── Dio/
│   │   ├── Dio.c
│   │   └── Dio.h
│   ├── Port/
│   │   ├── Port.c
│   │   └── Port.h
│   └── Pwm/
│       ├── Pwm.c
│       └── Pwm.h
│
├── SPL/                          # STM32 Standard Peripheral Library
│
├── Startup/                      # File khởi động ASM và vector table
│
├── Tools/                        # Thư mục build output
│   └── exchange.txt
│
├── main.c                        # Hàm main() - Application Layer
├── Makefile                       # Build bằng GCC
├── readme.md                      # Hướng dẫn dự án
└── STM32F103.svd                  # Mô tả thanh ghi cho VSCode

```

---

## **6. Luồng hoạt động**

1. **Khởi tạo IoHwAb**:
   ```c
   IoHwAb_Init(&IoHwAb0_Config);
   ```
2. **ADC đọc LM35DZ** liên tục qua **DMA** vào buffer `AdcGroup0Buffer`.
3. **Hàm IoHwAb0_ControlFanFromTemperature()**:
   - Chuyển giá trị ADC → nhiệt độ (°C).
   - Map nhiệt độ sang duty PWM (0–0x8000).
   - Gọi `Pwm_SetDutyCycle()` để điều khiển quạt.
4. **Hàm IoHwAb0_ControlLedBrightness()**:
   - Đọc biến trở → phần trăm sáng.
   - Map 0–100% → 0–0x8000 → PWM cho LED.

---

## **7. Công thức chuyển đổi LM35DZ**

- Điện áp cảm biến:  

  \[
  V_{mV} = \dfrac{ADC \times 3300}{4095}
  \]

- Nhiệt độ:  

  \[
  Temp(\degree C) = \dfrac{V_{mV}}{10}
  \]

- Ví dụ:  
  - ADC = 372 → 0.3 V → ~30°C  
  - ADC = 1240 → 1.0 V → ~100°C

---

## **8. Giải thích các hàm trong `IoHwAb.c`**

### **1. `IoHwAb_Init()`**
- Khởi tạo tất cả driver phần cứng theo cấu hình AUTOSAR.
- Thiết lập DMA buffer và khởi động ADC liên tục.
- Các bước chính:
  1. Lưu con trỏ cấu hình `IoHwAb0_ConfigPtr`.
  2. Gọi `Port_Init()`, `Adc_Init()`, `Pwm_Init()`.
  3. Gọi `Adc_SetupResultBuffer()` và `Adc_StartGroupConversion()`.

### **2. `IoHwAb0_SetLedState(boolean state)`**
- Điều khiển LED ở mức cao/thấp.
- Gọi `Dio_WriteChannel()` để ghi giá trị STD_HIGH/STD_LOW vào chân LED.

### **3. `IoHwAb0_GetTemperature(uint16* temperature)`**
- Đọc giá trị ADC từ LM35 và trả về nhiệt độ °C.
- Các bước:
  1. Lấy `adcValue` từ `AdcGroup0Buffer[0]`.
  2. Quy đổi ADC → mV → °C.
  3. Trả về qua con trỏ `temperature`.

### **4. `IoHwAb0_SetFanSpeed(uint8 percent)`**
- Điều khiển tốc độ quạt theo phần trăm.
- Giới hạn giá trị trong 0–100%.
- Map sang dải duty PWM 0–0x8000.
- Gọi `Pwm_SetDutyCycle()` với duty tính được.

### **5. `IoHwAb0_ControlFanFromTemperature(void)`**
- Đọc nhiệt độ hiện tại và tự động tăng/giảm tốc độ quạt.
- Thuật toán ví dụ:
  - <30°C → 0% duty
  - 30–80°C → tuyến tính 0–100%
  - ≥80°C → 100% duty

### **6. `IoHwAb0_GetPotentiometerValue(uint16* value)`**
- Đọc giá trị biến trở và trả về phần trăm (0–100%).
- Đọc `adcValue` từ kênh cấu hình cho biến trở.
- Map sang phần trăm.
- Có thể dùng để **bật/tắt LED** nếu vượt ngưỡng.

### **7. `IoHwAb0_ControlLedBrightness(void)`**
- Điều chỉnh độ sáng LED dựa trên giá trị biến trở.
- Các bước:
  1. Gọi `IoHwAb0_GetPotentiometerValue()`.
  2. Map phần trăm sang duty 0–0x8000.
  3. Gọi `Pwm_SetDutyCycle()` để điều chỉnh LED.

---

## **9. Kết quả mong đợi**

- **Quạt chạy nhanh hơn khi nhiệt độ tăng.**
- **LED sáng/tắt hoặc điều chỉnh độ sáng theo biến trở.**
- Mã nguồn theo chuẩn **AUTOSAR IoHwAb**, dễ mở rộng và bảo trì.

# Documents

## Specification of I/O Hardware Abstraction

https://www.autosar.org/fileadmin/standards/R24-11/CP/AUTOSAR_CP_SWS_IOHardwareAbstraction.pdf

## Requirements on I/O Hardware Abstraction

https://www.autosar.org/fileadmin/standards/R24-11/CP/AUTOSAR_CP_RS_IOHWAbstraction.pdf

