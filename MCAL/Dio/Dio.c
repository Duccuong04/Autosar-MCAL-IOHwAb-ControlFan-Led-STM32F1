/********************************************************************************************* 
 * @file         DIO.c
 * @brief        Định nghĩa các hàm theo chuẩn AUTOSAR Classic
 * @version      1.0
 * @date         2025-06-01
 * @author       Nguyen Duc Cuong
 *********************************************************************************************/

/* ===========================================================================================
 * Includes
 * =========================================================================================== */

#include "Dio.h"

/* ===========================================================================================
 * Fuction Definitions
 * =========================================================================================== */

 /********************************************************************************************* 
 * @brief       Đọc trạng thái của 1 Pin       
 * @param       Channel ID
 * @return      Dio_LevelType: STD_HIGH/ STD_LOW
 *********************************************************************************************/

 Dio_LevelType Dio_ReadChannel(Dio_ChannelType ChannelId)
{
    GPIO_TypeDef *GPIO_Port;
    uint16_t GPIO_Pin;

    /* Xác định Port GPIO*/
    GPIO_Port = DIO_GET_PORT_FROM_CHANNEL(ChannelId);

    /* Kiểm tra xem Port có hợp lệ không*/
    if(GPIO_Port == NULL)
    {
        return STD_LOW;
    }

    /*Xác định chân GPIO*/
    GPIO_Pin = DIO_GET_PIN(ChannelId);

    /*Đọc trạng thái logic của chân GPIO tương ứng (sử dụng SPL Library)*/
    if(GPIO_ReadInputDataBit(GPIO_Port, GPIO_Pin) == 1)
    {
        return STD_HIGH;
    }
    else
    {
        return STD_LOW;
    }
}

 /********************************************************************************************* 
 * @brief       Ghi trạng thái cho 1 Pin       
 * @param       Channel ID, Level
 * @return      void
 *********************************************************************************************/

void Dio_WriteChannel(Dio_ChannelType ChannelId, Dio_LevelType Level)
{
    GPIO_TypeDef *GPIO_Port;
    uint16_t GPIO_Pin;

    /* Xác định Port GPIO*/
    GPIO_Port = DIO_GET_PORT_FROM_CHANNEL(ChannelId);

    /*Xác định chân GPIO*/
    GPIO_Pin = DIO_GET_PIN(ChannelId);

    if(Level == STD_HIGH)
    {
        GPIO_SetBits(GPIO_Port, GPIO_Pin);
    }
    else
    {
        GPIO_ResetBits(GPIO_Port, GPIO_Pin);
    }
}
/********************************************************************************************* 
 * @brief       Đọc trạng thái của 1 Port     
 * @param       PortID
 * @return      PortLevel
 *********************************************************************************************/
Dio_PortLevelType Dio_ReadPort(Dio_PortType PortId)
{
    GPIO_TypeDef *GPIO_Port;

    GPIO_Port = DIO_GET_PORT_FROM_PORT(PortId);

    if(GPIO_Port == NULL)
    {
        return 0;
    }

    return (Dio_PortLevelType)GPIO_ReadInputData(GPIO_Port);    
}

/********************************************************************************************* 
 * @brief       Ghi trạng thái logic cho toàn bộ cổng DIO của 1 Port     
 * @param       PortID (DIO_PORT_A, DIO_PORTB,...), Level cho 16 pin của Port đối với STM32F1
 * @return      void
 *********************************************************************************************/
void Dio_WritePort(Dio_PortType PortId, Dio_PortLevelType Level)
{
    GPIO_TypeDef *GPIO_Port = DIO_GET_PORT_FROM_PORT(PortId);

    if (GPIO_Port != NULL)
    {
        if (Level == STD_HIGH)
        {
            GPIO_Port->ODR = 0xFFFF;  
        }
        else
        {
            GPIO_Port->ODR = 0x0000; 
        }
    }
}


/********************************************************************************************* 
 * @brief       Đọc trạng thái logic của một GroupChannel    
 * @param       Con trỏ trỏ đến 1  ChannelGroup
 * @return      Dio_PortLevelType
 *********************************************************************************************/
Dio_PortLevelType Dio_ReadChannelGroup(const Dio_ChannelGroupType* ChannelGroupPtr)
{
    /* Sử dụng maco DIO_GET_PORT để xác định cổng GPIO*/

    GPIO_TypeDef *GPIO_Port; 
	
		GPIO_Port = DIO_GET_PORT_FROM_GROUP(ChannelGroupPtr);

    /*Kiểm tra xem Port chứa GroupChannel có hợp lệ không*/
    if(GPIO_Port == NULL)
    {
        return 0;
    }

    /*Đọc trạng thái logic của toàn bộ cổng GPIO*/

    uint16_t portData = GPIO_ReadInputData(GPIO_Port);

    /*Lấy giá trị của nhóm kênh dùng mask và offset*/

    return (Dio_PortLevelType)((portData & ChannelGroupPtr->mask) >> ChannelGroupPtr->offset);
}

/********************************************************************************************* 
 * @brief       Ghi trạng thái logic cho một GroupChannel    
 * @param       Con trỏ trỏ đến 1  ChannelGroup, PortLevel
 * @return      void
 *********************************************************************************************/

void Dio_WriteChannelGroup(const Dio_ChannelGroupType* ChannelGroupPtr, Dio_PortLevelType Level)
{
    GPIO_TypeDef* GPIO_Port = DIO_GET_PORT_FROM_GROUP(ChannelGroupPtr);

    /* Sử dụng thanh ghi BSRR/ BRR - ghi dữ liệu cho từng chân*/
    if (Level == STD_HIGH)
    {
        GPIO_Port->BSRR = ChannelGroupPtr->mask;
    }
    else
    {
        GPIO_Port->BRR = ChannelGroupPtr->mask;
    }
}


/********************************************************************************************* 
 * @brief       Lấy dữ liệu về version của software đang dùng    
 * @param       Con trỏ VersionInfo
 * @return      void
 *********************************************************************************************/

void Dio_GetVersionInfo (Std_VersionInfoType* VersionInfo)
{
    VersionInfo->vendorID = 0x1234;
    VersionInfo->moduleID = 0x5678;
    VersionInfo->sw_major_version = 1;
    VersionInfo->sw_minor_version = 0;
    VersionInfo->sw_patch_version = 0;
}

/********************************************************************************************* 
 * @brief       Đảo ngược trạng thái của 1 channel   
 * @param       ChannelId
 * @return      STD_HIGH/ STD_LOW
 *********************************************************************************************/
Dio_LevelType Dio_FlipChannel (Dio_ChannelType ChannelId)
{
    Dio_LevelType currentLevel = Dio_ReadChannel(ChannelId);

    if (currentLevel == STD_HIGH)
    {
        Dio_WriteChannel(ChannelId, STD_LOW);
        return STD_LOW;
    }
    else
    {
        Dio_WriteChannel(ChannelId, STD_HIGH);
        return STD_HIGH;
    }
}
