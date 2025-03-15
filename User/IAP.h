#include "FreeRTOS.h"
#include "./led/bsp_led.h"   
#include <stdint.h>
#include "stm32f4xx_hal.h"
#include "bsp_debug_usart.h"
#include "queue.h"
#include "./eth/bsp_eth.h" 
#include "main.h"

#include "task.h"

#define FLASH_SECTOR_SIZE_128KB 131072 // 扇区大小，以字节为单位
#define RAM_APP_START_ADDR   0x20000000  // RAM 中应用程序的起始地址
//0-4扇区为boot程序的分区
//5为标志区
//6为运行区
//7为A区，APP1
//8为B区，APP2
//boot的结束地址，扇区4的最后地址
#define FLASH_BOOT_START_ADDR ADDR_FLASH_SECTOR_0
#define FLASH_BOOT_END_ADDR ADDR_FLASH_SECTOR_5
//标志区
#define FLASH_FLAG_START_ADDR ADDR_FLASH_SECTOR_7
#define FLASH_FLAG_END_ADDR ADDR_FLASH_SECTOR_8
//运行区
#define FLASH_RUN_START_ADDR ADDR_FLASH_SECTOR_6
#define FLASH_RUN_END_ADDR ADDR_FLASH_SECTOR_7
/**app的ab区**/
//a
#define FLASH_APP1_START_ADDR ADDR_FLASH_SECTOR_8
#define FLASH_APP1_END_ADDR ADDR_FLASH_SECTOR_9
//b
#define FLASH_APP2_START_ADDR ADDR_FLASH_SECTOR_9
#define FLASH_APP2_END_ADDR ADDR_FLASH_SECTOR_10

/* Base address of the Flash sectors */ 
#define ADDR_FLASH_SECTOR_0     ((uint32_t)0x08000000) /* Base address of Sector 0, 16 Kbytes   */
#define ADDR_FLASH_SECTOR_1     ((uint32_t)0x08004000) /* Base address of Sector 1, 16 Kbytes   */
#define ADDR_FLASH_SECTOR_2     ((uint32_t)0x08008000) /* Base address of Sector 2, 16 Kbytes   */
#define ADDR_FLASH_SECTOR_3     ((uint32_t)0x0800C000) /* Base address of Sector 3, 16 Kbytes   */
#define ADDR_FLASH_SECTOR_4     ((uint32_t)0x08010000) /* Base address of Sector 4, 64 Kbytes   */
#define ADDR_FLASH_SECTOR_5     ((uint32_t)0x08020000) /* Base address of Sector 5, 128 Kbytes  */

////运行区
#define ADDR_FLASH_SECTOR_6     ((uint32_t)0x08040000) /* Base address of Sector 6, 128 Kbytes  */

//标志区
#define ADDR_FLASH_SECTOR_7     ((uint32_t)0x08060000) /* Base address of Sector 7, 128 Kbytes  */
//APP1
//八九扇区不知道存了什么东西
//擦不掉
#define ADDR_FLASH_SECTOR_8     ((uint32_t)0x08080000) /* Base address of Sector 8, 128 Kbytes  */
#define ADDR_FLASH_SECTOR_9     ((uint32_t)0x080A0000) /* Base address of Sector 9, 128 Kbytes  */
#define ADDR_FLASH_SECTOR_10    ((uint32_t)0x080C0000) /* Base address of Sector 10, 128 Kbytes */
#define ADDR_FLASH_SECTOR_11    ((uint32_t)0x080E0000) /* Base address of Sector 11, 128 Kbytes */
#define ADDR_FLASH_SECTOR_12     ((uint32_t)0x08100000) /* Base address of Sector 12, 16 Kbytes  */

//判断是APP1还是APP2
#define APP1 0x1
#define APP2 0x2


// 定义协议结构体
typedef struct {
    uint16_t command;    // 2字节命令
    uint16_t length;    // 2字节总长度（包括包头、数据区和CRC16校验码）
    uint8_t payload[256]; // 数据区，这里假设最大长度为256字节
    uint8_t reserved[4];  // 4字节保留
    uint16_t crc16;     // 2字节CRC16校验码
} __attribute__((packed)) ProtocolHeader;

// 定义协议结构体
typedef struct {
    uint16_t command;    // 2字节命令
    uint16_t length;    // 2字节总长度（包括包头、数据区和CRC16校验码）
    uint8_t last[]; // 数据区和保留区和校验码都在这里
} __attribute__((packed)) LastProtocolHeader;



uint16_t CRC16_Calculate(uint8_t[],uint16_t);

void TOAPP(uint8_t[],uint8_t _ab,uint8_t N,uint16_t Arraysize);
uint8_t TransportApp(uint8_t ml);
uint32_t GetSector(uint32_t);
void JumpToApplication(uint32_t);
uint16_t UP_USART();
uint16_t UP_ETH();
void EraseAPP(uint8_t ml);

extern uint8_t rxBuffer[512]; // 假设最大接收缓冲区为512字节
extern volatile uint32_t rxIndex; // 当前接收到的数据长度
extern volatile uint8_t rxComplete; // 接收完成标志
extern volatile uint8_t packN;
extern volatile uint16_t Packlenth;
extern uint8_t ml;//命令


// 定义队列的长度和数据项的大小
#define QUEUE_LENGTH 1600
#define ITEM_SIZE    1  // 每个数据项为 1 字节



extern QueueHandle_t xDataQueue;

