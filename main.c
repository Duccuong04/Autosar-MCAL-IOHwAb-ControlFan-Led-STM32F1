#include "IoHwAb/IoHwAb.h"
#include "IoHwAb_Cfg.h"

int main(void)
{
    IoHwAb_Init(&IoHwAb0_Config);

    while(1)
    {
        /* Cập nhật tốc độ quạt dựa trên nhiệt độ*/ 
        IoHwAb0_ControlFanFromTemperature();

        /* Cập nhật độ sáng LED dựa trên giá trị biến trở */
        IoHwAb0_ControlLedBrightness();
    }
}