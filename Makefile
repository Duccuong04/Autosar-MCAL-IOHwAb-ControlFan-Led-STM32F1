# Makefile build cho AUTOSAR-Classic-ECU-Development/ 2. ADC Driver for STM32F103C8T6 Blue Pill
# Dùng để biên dịch và nạp firmware vào Blue Pill
# Output (.o, .elf, .bin) sẽ nằm trong thư mục Tools/

# Tên thư mục chứa file build
BUILDDIR = Tools
# Tên file đầu ra (không có phần mở rộng)
TARGET = $(BUILDDIR)/blinkled

# Toolchain
CC      = arm-none-eabi-gcc
OBJCOPY = arm-none-eabi-objcopy

# Include path và define macro
CFLAGS = -mcpu=cortex-m3 -mthumb -O0 -g -Wall -ffreestanding -nostdlib \
		 -I. \
         -ICMSIS \
         -IMCAL/Port \
		 -IMCAL/Dio \
		 -IMCAL/Adc \
		 -IMCAL/Pwm \
         -ISPL/inc \
         -ICommon \
         -IConfig \
		 -IIoHwAb\
		 -IHardware \
         -DSTM32F10X_MD -DUSE_STDPERIPH_DRIVER

# Linker script
LDSCRIPT = Linker/stm32f103.ld
LDFLAGS = -T$(LDSCRIPT) -nostdlib -Wl,--gc-sections

# Source files
SRCS_C = $(wildcard *.c) \
		 $(wildcard MCAL/Pwm/*.c) \
		 $(wildcard MCAL/Adc/*.c) \
		 $(wildcard MCAL/Port/*.c) \
		 $(wildcard MCAL/Dio/*.c) \
         $(wildcard Config/*.c) \
		 $(wildcard IoHwAb/*.c) \
         $(wildcard SPL/src/*.c) \
		 $(wildcard Hardware/*.c)
SRCS_S = $(wildcard Startup/*.s)

# List of object files (đặt trong BUILDDIR)
OBJS = $(patsubst %.c,$(BUILDDIR)/%.o,$(SRCS_C)) $(patsubst %.s,$(BUILDDIR)/%.o,$(SRCS_S))

# Tự tạo thư mục build nếu chưa có
$(shell mkdir -p $(BUILDDIR))

# Mục tiêu mặc định
all: $(TARGET).bin

# Biên dịch file C (object .o nằm trong Tools/)
$(BUILDDIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# Biên dịch file ASM (object .o nằm trong Tools/)
$(BUILDDIR)/%.o: %.s
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# Link thành ELF (trong Tools/)
$(TARGET).elf: $(OBJS) $(LDSCRIPT)
	$(CC) $(CFLAGS) $(OBJS) $(LDFLAGS) -o $@

# Tạo file .bin từ .elf (trong Tools/)
$(TARGET).bin: $(TARGET).elf
	$(OBJCOPY) -O binary $< $@

# Nạp firmware vào Blue Pill (cần openocd, ST-Link)
flash: $(TARGET).bin
	openocd -f interface/stlink.cfg -f target/stm32f1x.cfg -c "program $(TARGET).bin 0x08000000 verify reset exit"

# Xóa tất cả file build trong Tools/
clean:
	rm -rf $(BUILDDIR)/*
	rm -f SPL/src/*.o

.PHONY: all clean flash
