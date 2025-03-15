#include <IAP.h>

void EraseAPP(uint8_t ml){
    uint8_t Sector=0;
    if(ml==APP1){
        Sector=GetSector(FLASH_APP1_START_ADDR);
    }else
    {
        Sector=GetSector(FLASH_APP2_START_ADDR);
    }
        FLASH_EraseInitTypeDef EraseInitStruct;
        HAL_FLASH_Unlock();
    	uint32_t SECTORError = 0;
        EraseInitStruct.TypeErase     = FLASH_TYPEERASE_SECTORS;
        EraseInitStruct.VoltageRange  = FLASH_VOLTAGE_RANGE_3;/* 以“字”的大小进行操作 */ 
        EraseInitStruct.Sector        = Sector;
        EraseInitStruct.NbSectors     = 1;
        FLASH_WaitForLastOperation(0xFFFFFFFF); 
                /* 开始擦除操作 */
        while(HAL_FLASHEx_Erase(&EraseInitStruct, &SECTORError) != HAL_OK)
        {
            
        }
        FLASH_WaitForLastOperation(0xFFFFFFFF);
        HAL_FLASH_Lock();
}

uint16_t CRC16_Calculate(uint8_t* data,uint16_t len)
{
    len=len-10;
    // 读取计算结果
    uint16_t crc = 0xFFFF;  // 初始值
    const uint16_t polynomial = 0x1021;  // CRC-16/CCITT-FALSE 多项式

    for (size_t i = 0; i < len; ++i) {
        crc ^= (uint16_t)data[i] << 8;  // 将当前字节与 CRC 高字节异或

        for (int j = 0; j < 8; ++j) {
            if (crc & 0x8000) {  // 检查最高位是否为 1
                crc = (crc << 1) ^ polynomial;  // 左移并异或多项式
            } else {
                crc <<= 1;  // 左移
            }
        }
    }

    return crc;
}


uint32_t SECTORError = 0;
//将接收中断中接收到的数据包放入APP区
//只处理一个数据包。可以每次中断处理一次
void TOAPP(uint8_t* payload,uint8_t _ab,uint8_t packN,uint16_t Arraysize){
    if (HAL_FLASH_Unlock() != HAL_OK) {
        // 解锁失败处理
        while (1);
    }
    uint32_t Sector;
    if(_ab==APP1)
    {
        Sector=FLASH_APP1_START_ADDR;
    }else
    {
        Sector=FLASH_APP2_START_ADDR;
    }
    for(int i=0;i<Arraysize;i++)
    {
                
                if(HAL_FLASH_Program(FLASH_TYPEPROGRAM_BYTE, Sector +(packN-1)*256+ i, payload[i])!= HAL_OK){
                // 错误处理
                    while(1){  
                        LED1_TOGGLE;
                    };
                }   
     }
//    if(_ab==APP1){
//            for(int i=0;i<Arraysize;i++){
//                
//                if(HAL_FLASH_Program(FLASH_TYPEPROGRAM_BYTE, FLASH_APP1_START_ADDR +(packN-1)*256+ i, payload[i])!= HAL_OK){
//                // 错误处理
//                    while(1){  
//                        LED1_TOGGLE;
//                    };
//                }   
//            }
//    }else
//    {
//        for(int i=0;i<Arraysize;i++){
//                
//                if(HAL_FLASH_Program(FLASH_TYPEPROGRAM_BYTE, FLASH_APP2_START_ADDR +(packN-1)*256+ i, payload[i])!= HAL_OK){
//                // 错误处理
//                    while(1){  
//                        LED1_TOGGLE;
//                    };
//                }   
//            }
//    }
    HAL_FLASH_Lock();
}

//搬运app功能，将app搬到运行区
//传入判断是搬运a区还是b区的参数，判断是a还是b由标志区来传入
uint8_t TransportApp(uint8_t _ab){
    FLASH_EraseInitTypeDef EraseInitStruct;
    //起始扇区和扇区数
    uint32_t FirstSector = 0;
	uint32_t NbOfSectors = 0;
	uint32_t SECTORError = 0;
    uint32_t data=0;
    //解锁flash
    HAL_FLASH_Unlock();
    FLASH_WaitForLastOperation(0xFFFFFFFF);
	FirstSector = GetSector(FLASH_RUN_START_ADDR);
	NbOfSectors =1;
    //先擦除运行区
    EraseInitStruct.TypeErase     = FLASH_TYPEERASE_SECTORS;
    EraseInitStruct.VoltageRange  = FLASH_VOLTAGE_RANGE_3;/* 以“字”的大小进行操作 */ 
    EraseInitStruct.Sector        = FirstSector;
    EraseInitStruct.NbSectors     = NbOfSectors;
    /* 开始擦除操作 */
    if (HAL_FLASHEx_Erase(&EraseInitStruct, &SECTORError) != HAL_OK)
    {
        /*擦除出错，返回，实际应用中可加入处理 */
        HAL_FLASH_Lock();
        return -1;
    }
    uint32_t APPR;//记录APP的开始地址
    if(_ab==APP1){//判断是APP1还是APP2
        APPR=FLASH_APP1_START_ADDR;
    }else
    {
        APPR=FLASH_APP2_START_ADDR;
    }
    for(int i=0;i<FLASH_SECTOR_SIZE_128KB/sizeof(uint32_t);i++){
        data=*(uint32_t*)(APPR + i * sizeof(uint32_t));
        FLASH_WaitForLastOperation(0xFFFFFFFF);
        if(HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, FLASH_RUN_START_ADDR + i * sizeof(uint32_t), data)!= HAL_OK){
        // 错误处理
        while(1){
            HAL_FLASH_Lock();
            return -1;};
        }
    }
 
//    if(_ab==APP1){
//        for(int i=0;i<FLASH_SECTOR_SIZE_128KB/sizeof(uint32_t);i++){
//            data=*(uint32_t*)(FLASH_APP1_START_ADDR + i * sizeof(uint32_t));
//            if(HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, FLASH_RUN_START_ADDR + i * sizeof(uint32_t), data)!= HAL_OK){
//            // 错误处理
//            while(1);
//            }
//        }
//    }  
//    else{
//        for(int i=0;i<FLASH_SECTOR_SIZE_128KB/sizeof(uint32_t);i++){
//            data=*(uint32_t*)(FLASH_APP2_START_ADDR + i * sizeof(uint32_t));
//            if(HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, FLASH_RUN_START_ADDR + i * sizeof(uint32_t), data)!= HAL_OK){
//            // 错误处理
//            while(1);
//            }
//        }
//    }
    //锁定flash
    HAL_FLASH_Lock();
}


/**
  * @brief  根据输入的地址给出它所在的sector
  *					例如：
						uwStartSector = GetSector(FLASH_USER_START_ADDR);
						uwEndSector = GetSector(FLASH_USER_END_ADDR);	
  * @param  Address：地址
  * @retval 地址所在的sector
  */
uint32_t GetSector(uint32_t Address)
{
  uint32_t sector = 0;
  
  if((Address < ADDR_FLASH_SECTOR_1) && (Address >= ADDR_FLASH_SECTOR_0))
  {
    sector = FLASH_SECTOR_0;  
  }
  else if((Address < ADDR_FLASH_SECTOR_2) && (Address >= ADDR_FLASH_SECTOR_1))
  {
    sector = FLASH_SECTOR_1;  
  }
  else if((Address < ADDR_FLASH_SECTOR_3) && (Address >= ADDR_FLASH_SECTOR_2))
  {
    sector = FLASH_SECTOR_2;  
  }
  else if((Address < ADDR_FLASH_SECTOR_4) && (Address >= ADDR_FLASH_SECTOR_3))
  {
    sector = FLASH_SECTOR_3;  
  }
  else if((Address < ADDR_FLASH_SECTOR_5) && (Address >= ADDR_FLASH_SECTOR_4))
  {
    sector = FLASH_SECTOR_4;  
  }
  else if((Address < ADDR_FLASH_SECTOR_6) && (Address >= ADDR_FLASH_SECTOR_5))
  {
    sector = FLASH_SECTOR_5;  
  }
  else if((Address < ADDR_FLASH_SECTOR_7) && (Address >= ADDR_FLASH_SECTOR_6))
  {
    sector = FLASH_SECTOR_6;  
  }
  else if((Address < ADDR_FLASH_SECTOR_8) && (Address >= ADDR_FLASH_SECTOR_7)) /*(Address < FLASH_END_ADDR) && (Address >= ADDR_FLASH_SECTOR_23))*/
  {
    sector = FLASH_SECTOR_7;  
  }
  else if((Address < ADDR_FLASH_SECTOR_9) && (Address >= ADDR_FLASH_SECTOR_8)) /*(Address < FLASH_END_ADDR) && (Address >= ADDR_FLASH_SECTOR_23))*/
  {
    sector = FLASH_SECTOR_8;  
  }
  else if((Address < ADDR_FLASH_SECTOR_10) && (Address >= ADDR_FLASH_SECTOR_9)) /*(Address < FLASH_END_ADDR) && (Address >= ADDR_FLASH_SECTOR_23))*/
  {
    sector = FLASH_SECTOR_9;  
  }
  else if((Address < ADDR_FLASH_SECTOR_11) && (Address >= ADDR_FLASH_SECTOR_10)) /*(Address < FLASH_END_ADDR) && (Address >= ADDR_FLASH_SECTOR_23))*/
  {
    sector = FLASH_SECTOR_10;  
  }
  else if((Address < ADDR_FLASH_SECTOR_12) && (Address >= ADDR_FLASH_SECTOR_11)) /*(Address < FLASH_END_ADDR) && (Address >= ADDR_FLASH_SECTOR_23))*/
  {
    sector = FLASH_SECTOR_11;  
  }
  return sector;
}


/**从boot区跳转到运行区**/
typedef void (*pFunction)(void);

void JumpToApplication(uint32_t ApplicationAddress) {
    pFunction Jump_To_Application;
    uint32_t JumpAddress;
    // 检查应用程序地址是否有效
    if (((*(__IO uint32_t*)ApplicationAddress) & 0x2FFE0000 ) == 0x20000000) {
        // 设置堆栈指针
        __set_MSP(*(volatile uint32_t*)ApplicationAddress);

        // 设置程序计数器
        JumpAddress = *(volatile uint32_t*)(ApplicationAddress + 4);
        // 禁用中断
        __disable_irq();
        SCB->VTOR =ApplicationAddress;
        Jump_To_Application = (pFunction)JumpAddress;

        // 跳转到应用程序
        Jump_To_Application();
    }
}

// 定义接收缓冲区和状态变量
uint8_t rxBuffer[512]; // 假设最大接收缓冲区为512字节
volatile uint32_t rxIndex = 0; // 当前接收到的数据长度
volatile uint8_t rxComplete = 0; // 接收完成标志
volatile uint8_t packN = 0;
volatile uint16_t Packlenth=0;
uint8_t  ml= 0;//命令
/**开始远程升级**/
//串口版
uint16_t UP_USART(){


    uint8_t Qdata=0;
	if(xQueueReceive(xDataQueue, &Qdata, NULL) == pdPASS)
	{		//一次接收一个字节
         
    //包头2字节命合、2字节总长度、N字节数据区、4字节保留、2字节crc16检验
        // 将接收到的数据存储到缓冲区
            rxBuffer[rxIndex]=Qdata;
            rxIndex++;
        if(rxIndex==4){//当总长度被传输过来后取出总长度
            uint16_t pj=0;
            uint8_t rx3=0;
            uint8_t rx2=0;
            uint8_t jh;
            rx3=rxBuffer[3];
            rx2=rxBuffer[2];
            jh=rx3;
            rx3=rx2;
            rx2=jh;
            pj=rx2<<8|rx3;
            Packlenth=pj;
            ml=rxBuffer[0];
        };
        // 检查是否接收到完整的数据包，根据取出的总长度
        if (rxIndex == Packlenth)
        {
            
            ProtocolHeader* header = (ProtocolHeader*)rxBuffer;
                packN++;
                printf("jieshouyigeshujubao  successfully:%d",packN);
                // 验证CRC校验码
                uint16_t calculatedCrc = CRC16_Calculate(header->payload,header->length);
                if (calculatedCrc == header->crc16)
                {
                    // CRC校验通过，处理数据,APP1这个值由标志位决定
                    //擦除对应APP区
                    if(packN==1)
                    {
                        uint8_t readData;
                        if(ml==APP1)
                        {
                            readData = *(__IO uint32_t *)FLASH_APP1_START_ADDR;
                        }else if(ml==APP2)
                        {
                            readData = *(__IO uint32_t *)FLASH_APP2_START_ADDR;
                        }
                        while(readData!=0xFF){
                            EraseAPP(ml);
                            if(ml==APP1)
                            {
                                readData = *(__IO uint32_t *)FLASH_APP1_START_ADDR;
                            }else if(ml==APP2)
                            {
                                readData = *(__IO uint32_t *)FLASH_APP2_START_ADDR;
                            }
                        }
                        
                        printf("包发完");
                    }
                    if(packN==1)
                    {
                        printf("包发完");
                    }
                    TOAPP(header->payload,ml,packN,header->length-10);
                    printf("crc  successfully");
                    if(packN==2){
                        printf("包发完");
                    }
                    if(packN==3){
                        printf("包发完");
                    }
                    if(packN==4){
                        printf("包发完");
                    }
                    if(packN==5){
                        printf("包发完");
                    }
                    if(packN==6){
                        printf("包发完");
                    }
                    //当包的最后为这些字节的时候说明就是最后一个包了，
                    //处理不完善，这是填写不满的包如果包的数据刚好覆盖这里就没法成功了。
                    if(header->payload[255]==0xF0&&header->payload[254]==0xF0
                        &&header->payload[253]==0xF0&&header->payload[252]==0xF0){
                        rxComplete=1;
                         //APPA
                         //EraseAPPA();
                            packN=0;
                         if(TransportApp(ml)==(uint8_t)(-1))
                         {
                             
                         }else
                         {
                             JumpToApplication(FLASH_RUN_START_ADDR);
                         }
                    }
                }
                else
                {
                    // CRC校验失败，丢弃数据包
                }

                // 重置接收状态
                rxIndex = 0;
        }
	}

}

