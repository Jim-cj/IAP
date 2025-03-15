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
        EraseInitStruct.VoltageRange  = FLASH_VOLTAGE_RANGE_3;/* �ԡ��֡��Ĵ�С���в��� */ 
        EraseInitStruct.Sector        = Sector;
        EraseInitStruct.NbSectors     = 1;
        FLASH_WaitForLastOperation(0xFFFFFFFF); 
                /* ��ʼ�������� */
        while(HAL_FLASHEx_Erase(&EraseInitStruct, &SECTORError) != HAL_OK)
        {
            
        }
        FLASH_WaitForLastOperation(0xFFFFFFFF);
        HAL_FLASH_Lock();
}

uint16_t CRC16_Calculate(uint8_t* data,uint16_t len)
{
    len=len-10;
    // ��ȡ������
    uint16_t crc = 0xFFFF;  // ��ʼֵ
    const uint16_t polynomial = 0x1021;  // CRC-16/CCITT-FALSE ����ʽ

    for (size_t i = 0; i < len; ++i) {
        crc ^= (uint16_t)data[i] << 8;  // ����ǰ�ֽ��� CRC ���ֽ����

        for (int j = 0; j < 8; ++j) {
            if (crc & 0x8000) {  // ������λ�Ƿ�Ϊ 1
                crc = (crc << 1) ^ polynomial;  // ���Ʋ�������ʽ
            } else {
                crc <<= 1;  // ����
            }
        }
    }

    return crc;
}


uint32_t SECTORError = 0;
//�������ж��н��յ������ݰ�����APP��
//ֻ����һ�����ݰ�������ÿ���жϴ���һ��
void TOAPP(uint8_t* payload,uint8_t _ab,uint8_t packN,uint16_t Arraysize){
    if (HAL_FLASH_Unlock() != HAL_OK) {
        // ����ʧ�ܴ���
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
                // ������
                    while(1){  
                        LED1_TOGGLE;
                    };
                }   
     }
//    if(_ab==APP1){
//            for(int i=0;i<Arraysize;i++){
//                
//                if(HAL_FLASH_Program(FLASH_TYPEPROGRAM_BYTE, FLASH_APP1_START_ADDR +(packN-1)*256+ i, payload[i])!= HAL_OK){
//                // ������
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
//                // ������
//                    while(1){  
//                        LED1_TOGGLE;
//                    };
//                }   
//            }
//    }
    HAL_FLASH_Lock();
}

//����app���ܣ���app�ᵽ������
//�����ж��ǰ���a������b���Ĳ������ж���a����b�ɱ�־��������
uint8_t TransportApp(uint8_t _ab){
    FLASH_EraseInitTypeDef EraseInitStruct;
    //��ʼ������������
    uint32_t FirstSector = 0;
	uint32_t NbOfSectors = 0;
	uint32_t SECTORError = 0;
    uint32_t data=0;
    //����flash
    HAL_FLASH_Unlock();
    FLASH_WaitForLastOperation(0xFFFFFFFF);
	FirstSector = GetSector(FLASH_RUN_START_ADDR);
	NbOfSectors =1;
    //�Ȳ���������
    EraseInitStruct.TypeErase     = FLASH_TYPEERASE_SECTORS;
    EraseInitStruct.VoltageRange  = FLASH_VOLTAGE_RANGE_3;/* �ԡ��֡��Ĵ�С���в��� */ 
    EraseInitStruct.Sector        = FirstSector;
    EraseInitStruct.NbSectors     = NbOfSectors;
    /* ��ʼ�������� */
    if (HAL_FLASHEx_Erase(&EraseInitStruct, &SECTORError) != HAL_OK)
    {
        /*�����������أ�ʵ��Ӧ���пɼ��봦�� */
        HAL_FLASH_Lock();
        return -1;
    }
    uint32_t APPR;//��¼APP�Ŀ�ʼ��ַ
    if(_ab==APP1){//�ж���APP1����APP2
        APPR=FLASH_APP1_START_ADDR;
    }else
    {
        APPR=FLASH_APP2_START_ADDR;
    }
    for(int i=0;i<FLASH_SECTOR_SIZE_128KB/sizeof(uint32_t);i++){
        data=*(uint32_t*)(APPR + i * sizeof(uint32_t));
        FLASH_WaitForLastOperation(0xFFFFFFFF);
        if(HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, FLASH_RUN_START_ADDR + i * sizeof(uint32_t), data)!= HAL_OK){
        // ������
        while(1){
            HAL_FLASH_Lock();
            return -1;};
        }
    }
 
//    if(_ab==APP1){
//        for(int i=0;i<FLASH_SECTOR_SIZE_128KB/sizeof(uint32_t);i++){
//            data=*(uint32_t*)(FLASH_APP1_START_ADDR + i * sizeof(uint32_t));
//            if(HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, FLASH_RUN_START_ADDR + i * sizeof(uint32_t), data)!= HAL_OK){
//            // ������
//            while(1);
//            }
//        }
//    }  
//    else{
//        for(int i=0;i<FLASH_SECTOR_SIZE_128KB/sizeof(uint32_t);i++){
//            data=*(uint32_t*)(FLASH_APP2_START_ADDR + i * sizeof(uint32_t));
//            if(HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, FLASH_RUN_START_ADDR + i * sizeof(uint32_t), data)!= HAL_OK){
//            // ������
//            while(1);
//            }
//        }
//    }
    //����flash
    HAL_FLASH_Lock();
}


/**
  * @brief  ��������ĵ�ַ���������ڵ�sector
  *					���磺
						uwStartSector = GetSector(FLASH_USER_START_ADDR);
						uwEndSector = GetSector(FLASH_USER_END_ADDR);	
  * @param  Address����ַ
  * @retval ��ַ���ڵ�sector
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


/**��boot����ת��������**/
typedef void (*pFunction)(void);

void JumpToApplication(uint32_t ApplicationAddress) {
    pFunction Jump_To_Application;
    uint32_t JumpAddress;
    // ���Ӧ�ó����ַ�Ƿ���Ч
    if (((*(__IO uint32_t*)ApplicationAddress) & 0x2FFE0000 ) == 0x20000000) {
        // ���ö�ջָ��
        __set_MSP(*(volatile uint32_t*)ApplicationAddress);

        // ���ó��������
        JumpAddress = *(volatile uint32_t*)(ApplicationAddress + 4);
        // �����ж�
        __disable_irq();
        SCB->VTOR =ApplicationAddress;
        Jump_To_Application = (pFunction)JumpAddress;

        // ��ת��Ӧ�ó���
        Jump_To_Application();
    }
}

// ������ջ�������״̬����
uint8_t rxBuffer[512]; // ���������ջ�����Ϊ512�ֽ�
volatile uint32_t rxIndex = 0; // ��ǰ���յ������ݳ���
volatile uint8_t rxComplete = 0; // ������ɱ�־
volatile uint8_t packN = 0;
volatile uint16_t Packlenth=0;
uint8_t  ml= 0;//����
/**��ʼԶ������**/
//���ڰ�
uint16_t UP_USART(){


    uint8_t Qdata=0;
	if(xQueueReceive(xDataQueue, &Qdata, NULL) == pdPASS)
	{		//һ�ν���һ���ֽ�
         
    //��ͷ2�ֽ����ϡ�2�ֽ��ܳ��ȡ�N�ֽ���������4�ֽڱ�����2�ֽ�crc16����
        // �����յ������ݴ洢��������
            rxBuffer[rxIndex]=Qdata;
            rxIndex++;
        if(rxIndex==4){//���ܳ��ȱ����������ȡ���ܳ���
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
        // ����Ƿ���յ����������ݰ�������ȡ�����ܳ���
        if (rxIndex == Packlenth)
        {
            
            ProtocolHeader* header = (ProtocolHeader*)rxBuffer;
                packN++;
                printf("jieshouyigeshujubao  successfully:%d",packN);
                // ��֤CRCУ����
                uint16_t calculatedCrc = CRC16_Calculate(header->payload,header->length);
                if (calculatedCrc == header->crc16)
                {
                    // CRCУ��ͨ������������,APP1���ֵ�ɱ�־λ����
                    //������ӦAPP��
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
                        
                        printf("������");
                    }
                    if(packN==1)
                    {
                        printf("������");
                    }
                    TOAPP(header->payload,ml,packN,header->length-10);
                    printf("crc  successfully");
                    if(packN==2){
                        printf("������");
                    }
                    if(packN==3){
                        printf("������");
                    }
                    if(packN==4){
                        printf("������");
                    }
                    if(packN==5){
                        printf("������");
                    }
                    if(packN==6){
                        printf("������");
                    }
                    //���������Ϊ��Щ�ֽڵ�ʱ��˵���������һ�����ˣ�
                    //�������ƣ�������д�����İ�����������ݸպø��������û���ɹ��ˡ�
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
                    // CRCУ��ʧ�ܣ��������ݰ�
                }

                // ���ý���״̬
                rxIndex = 0;
        }
	}

}

