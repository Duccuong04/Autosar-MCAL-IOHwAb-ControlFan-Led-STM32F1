/********************************************************************************************* 
 * @file         Std_Types.h
 * @brief        Khai báo, định nghĩa các biến cho các module I/O Drivers trong AUTOSAR Classic 
 * @details      File này cung cấp việc định nghĩa bao gồm:
 *                   các kiểu dữ liệu
 *                   tên trạng thái trả về 
 *                   số hiệu version dùng
 * @version      1.0
 * @date         2025-06-01
 * @author       Nguyen Duc Cuong
 *********************************************************************************************/

#ifndef STD_TYPES_H
#define STD_TYPES_H

/* ===========================================================================================
 * Includes
 * =========================================================================================== */
#include <stdint.h>

/* ===========================================================================================
 * Software version number definitions
 * =========================================================================================== */
/********************************************************************************************* 
 * @brief        Định nghĩa software version
 * @details      AUTOSAR Version Info & Vendor Specific Version Info
 *********************************************************************************************/

#define STD_TYPES_AR_RELEASE_MAJOR_VERSION      0x04U
#define STD_TYPES_AR_RELEASE_MINOR_VERSION      0x03U
#define STD_TYPES_AR_RELEASE_REVISION_VERSION   0x01U

#define STD_TYPES_SW_MAJOR_VERSION              0x01U
#define STD_TYPES_SW_MINOR_VERSION              0x00U
#define STD_TYPES_SW_PATCH_VERSION              0x00U

/* ===========================================================================================
 * Standard Return Type
 * =========================================================================================== */
/********************************************************************************************* 
 * @typedef      Std_ReturnType
 * @brief        Kiểu trả về theo tiêu chuẩn AUTOSAR
 * @details      Được sử dụng cho các hàm API, trả về E_OK và E_NOT_OK
 *********************************************************************************************/

typedef uint8_t Std_ReturnType;
/* 
Because E_OK is already defined within OSEK, the symbol E_OK has to be shared. To avoid
name clashes and redefinition problems, the symbols have to be defined in the following way
*/
#ifndef STATUSTYPEDEFINED
#define STATUSTYPEDEFINED
    #define E_OK                                0x00U
    #define E_NOT_OK                            0x01U
    typedef unsigned char StatusType;                  /*<< OSEK compliance */
#endif

/* ===========================================================================================
 * Logical State Definitions
 * =========================================================================================== */
/********************************************************************************************* 
 * @brief        Định nghĩa logic cao và thấp 
 * @details      Được sử dụng cho tín hiệu vào/ ra
 *********************************************************************************************/

#define STD_HIGH                                0x01u /*<< Physical state 5V or 3.3V */
#define STD_LOW                                 0x00u /*<< Physical state 0V */

/* ===========================================================================================
 * NULL Pointer Definition
 * =========================================================================================== */
/********************************************************************************************* 
 * @brief        Định nghĩa con trỏ NULL
 * @details      Trỏ đến 0
 *********************************************************************************************/

#ifndef 	NULL_PTR
    #define	NULL_PTR                            ((void*)0)
#endif

/* ===========================================================================================
 * Platform Independent Data Types
 * =========================================================================================== */
/********************************************************************************************* 
 * @brief        Kiểu dữ liệu độc lập với chip sử dụng
 * @details      Định nghĩa rõ ràng về kích thước và dấu của kiểu dữ liệu
 *********************************************************************************************/

typedef signed char         uint8;                      /*<< Số nguyên không dấu 8 bit*/ 
typedef unsigned char       sint8;                      /*<< Số nguyên có dấu 8 bit*/ 
typedef signed short        uint16;                     /*<< Số nguyên không dấu 16 bit*/ 
typedef unsigned short      sint16;                     /*<< Số nguyên có dấu 16 bit*/ 
typedef signed long         uint32;                     /*<< Số nguyên không dấu 32 bit*/ 
typedef unsigned long       sint32;                     /*<< Số nguyên có dấu 32 bit*/ 
typedef signed long long    uint64;                     /*<< Số nguyên không dấu 64 bit*/ 
typedef unsigned long long  sint64;                     /*<< Số nguyên có dấu 8 bit*/ 

typedef float               float32;                    /*<< Số thực 32 bit*/ 
typedef double              float64;                    /*<< Số thực 64 bit*/

/* ===========================================================================================
 * Platform Independent Data Types (Volatile Type)
 * =========================================================================================== */
/********************************************************************************************* 
 * @brief        Kiểu dữ liệu Volatile độc lập với chip sử dụng
 * @details      Được sử dụng cho các thanh ghi và vùng nhớ phần cứng
 *********************************************************************************************/

typedef volatile uint8      vuint8;                      /*<< Số nguyên không dấu 8 bit volatile*/ 
typedef volatile sint8      vsint8;                      /*<< Số nguyên có dấu 8 bit volatile*/ 
typedef volatile uint16     vuint16;                     /*<< Số nguyên không dấu 16 bit volatile*/ 
typedef volatile sint16     vsint16;                     /*<< Số nguyên có dấu 16 bit volatile*/ 
typedef volatile uint32     vuint32;                     /*<< Số nguyên không dấu 32 bit volatile*/ 
typedef volatile sint32     vsint32;                     /*<< Số nguyên có dấu 32 bit volatile*/ 
typedef volatile uint64     vuint64;                     /*<< Số nguyên không dấu 64 bit volatile*/ 
typedef volatile sint64     vsint64;                     /*<< Số nguyên có dấu 8 bit volatile*/ 

/* ===========================================================================================
 * Boolean Type Definition
 * =========================================================================================== */
/********************************************************************************************* 
 * @brief        Kiểu dữ liệu boolean
 * @details      Mô tả đúng/ sai
 *********************************************************************************************/
typedef uint8 boolean;
#ifndef TRUE
    #define TRUE                                1U
#endif

#ifndef FALSE
    #define FALSE                               0U
#endif

/* ===========================================================================================
 * Version Information Struct
 * =========================================================================================== */
/********************************************************************************************* 
 * @struct       Std_VersionInfoType
 * @brief        Cấu trúc phiên bản phần mềm
 * @details      Lưu trữ thông tin của phiên bản
 *********************************************************************************************/
typedef struct {
    uint16 vendorID;                                     /*<< ID nhà cung cấp*/  
    uint16 moduleID;                                     /*<< ID Module*/ 
    uint8  sw_major_version;                             /*<< Phiên bản chính của phần mềm*/ 
    uint8  sw_minor_version;                             /*<< Phiên bản phụ của phần mềm*/ 
    uint8  sw_patch_version;                             /*<< Phiên bản sửa lỗi của phần mềm*/ 
} Std_VersionInfoType;

#endif /* STD_TYPES_H */