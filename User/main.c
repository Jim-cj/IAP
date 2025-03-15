/**
  *********************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2019-xx-xx
  * @brief   FreeRTOS V9.0.0 + STM32 LwIP
  *********************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ��  STM32ȫϵ�п����� 
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :https://fire-stm32.taobao.com
  *
  **********************************************************************
  */ 
 
/*
*************************************************************************
*                             ������ͷ�ļ�
*************************************************************************
*/ 
#include "main.h"
/* FreeRTOSͷ�ļ� */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include <IAP.h>
#include "tcpecho.h"
/**************************** ������ ********************************/
/* 
 * ��������һ��ָ�룬����ָ��һ�����񣬵����񴴽���֮�����;�����һ��������
 * �Ժ�����Ҫ��������������Ҫͨ�������������������������������Լ�����ô
 * ����������ΪNULL��
 */
static TaskHandle_t AppTaskCreate_Handle = NULL;/* ���������� */
static TaskHandle_t Test1_Task_Handle = NULL;/* LED������ */
//static TaskHandle_t Test2_Task_Handle = NULL;/* KEY������ */

/********************************** �ں˶����� *********************************/
/*
 * �ź�������Ϣ���У��¼���־�飬�����ʱ����Щ�������ں˵Ķ���Ҫ��ʹ����Щ�ں�
 * ���󣬱����ȴ����������ɹ�֮��᷵��һ����Ӧ�ľ����ʵ���Ͼ���һ��ָ�룬������
 * �ǾͿ���ͨ��������������Щ�ں˶���
 *
 * �ں˶���˵���˾���һ��ȫ�ֵ����ݽṹ��ͨ����Щ���ݽṹ���ǿ���ʵ��������ͨ�ţ�
 * �������¼�ͬ���ȸ��ֹ��ܡ�������Щ���ܵ�ʵ��������ͨ��������Щ�ں˶���ĺ���
 * ����ɵ�
 * 
 */

/******************************* ȫ�ֱ������� ************************************/
/*
 * ��������дӦ�ó����ʱ�򣬿�����Ҫ�õ�һЩȫ�ֱ�����
 */


/******************************* �궨�� ************************************/
/*
 * ��������дӦ�ó����ʱ�򣬿�����Ҫ�õ�һЩ�궨�塣
 */


/*
*************************************************************************
*                             ��������
*************************************************************************
*/
static void AppTaskCreate(void);/* ���ڴ������� */

static void Test1_Task(void* pvParameters);/* Test1_Task����ʵ�� */
//static void Test2_Task(void* pvParameters);/* Test2_Task����ʵ�� */

extern void TCPIP_Init(void);

extern xSemaphoreHandle s_xSemaphore;
// �������
QueueHandle_t xDataQueue = NULL;
/*****************************************************************
  * @brief  ������
  * @param  ��
  * @retval ��
  * @note   ��һ����������Ӳ����ʼ�� 
            �ڶ���������APPӦ������
            ������������FreeRTOS����ʼ���������
  ****************************************************************/
   

int main(void)
{	
  BaseType_t xReturn = pdPASS;/* ����һ��������Ϣ����ֵ��Ĭ��ΪpdPASS */
  
    
  /* ������Ӳ����ʼ�� */
  BSP_Init();
  /* ����AppTaskCreate���� */
  xReturn = xTaskCreate((TaskFunction_t )AppTaskCreate,  /* ������ں��� */
                        (const char*    )"AppTaskCreate",/* �������� */
                        (uint16_t       )512,  /* ����ջ��С */
                        (void*          )NULL,/* ������ں������� */
                        (UBaseType_t    )1, /* ��������ȼ� */
                        (TaskHandle_t*  )&AppTaskCreate_Handle);/* ������ƿ�ָ�� */ 
  /* ����������� */           
  if(pdPASS == xReturn)
    vTaskStartScheduler();   /* �������񣬿������� */
  else
    return -1;  
  
  while(1);   /* ��������ִ�е����� */    
}


/***********************************************************************
  * @ ������  �� AppTaskCreate
  * @ ����˵���� Ϊ�˷���������е����񴴽����������������������
  * @ ����    �� ��  
  * @ ����ֵ  �� ��
  **********************************************************************/
static void AppTaskCreate(void)
{
  BaseType_t xReturn = pdPASS;/* ����һ��������Ϣ����ֵ��Ĭ��ΪpdPASS */
  
  TCPIP_Init();
  
  tcpecho_init();

  
  printf("��������ʾTCP����ʵ��,���Է������ݵ�������,�����彫���ݷ��ص�������\n\n");
  
  printf("��������ģ�����£�\n\t ����<--����-->·��<--����-->������\n\n");
  
  printf("ʵ����ʹ��TCPЭ�鴫�����ݣ�������ΪTCP Client ����������ΪTCP Server\n\n");
  
  printf("�����̵�IP��ַ����User/arch/sys_arch.h�ļ����޸�\n\n");
    
  printf("�����̲ο�<<LwIPӦ��ʵս����ָ��>> ʹ�� NETCONN �ӿڱ��\n\n");
  
  printf("�뽫������λ������ΪTCP Client.���뿪�����IP��ַ(��192.168.0.122)\n\n");  
  
  printf("�����޸�IP��ַ��˿ںţ����޸Ķ�Ӧ�ĺ궨��:IP_ADDR0,IP_ADDR1,IP_ADDR2,IP_ADDR3,LOCAL_PORT\n\n");  
  
  taskENTER_CRITICAL();           //�����ٽ���

    
    // ��������
    xDataQueue = xQueueCreate(QUEUE_LENGTH, ITEM_SIZE);
    if (xDataQueue == NULL) {
        // ���д���ʧ�ܣ����д�����
        while (1);
    }


  /* ����Test1_Task���� */
  xReturn = xTaskCreate((TaskFunction_t )Test1_Task, /* ������ں��� */
                        (const char*    )"Test1_Task",/* �������� */
                        (uint16_t       )512,   /* ����ջ��С */
                        (void*          )NULL,	/* ������ں������� */
                        (UBaseType_t    )2,	    /* ��������ȼ� */
                        (TaskHandle_t*  )&Test1_Task_Handle);/* ������ƿ�ָ�� */
  if(pdPASS == xReturn)
    printf("Create Test1_Task sucess...\r\n");
  
//  /* ����Test2_Task���� */
//  xReturn = xTaskCreate((TaskFunction_t )Test2_Task,  /* ������ں��� */
//                        (const char*    )"Test2_Task",/* �������� */
//                        (uint16_t       )100,  /* ����ջ��С */
//                        (void*          )NULL,/* ������ں������� */
//                        (UBaseType_t    )3, /* ��������ȼ� */
//                        (TaskHandle_t*  )&Test2_Task_Handle);/* ������ƿ�ָ�� */ 
//  if(pdPASS == xReturn)
//    printf("Create Test2_Task sucess...\n\n");
  
  vTaskDelete(AppTaskCreate_Handle); //ɾ��AppTaskCreate����
  
  taskEXIT_CRITICAL();            //�˳��ٽ���
}



/**********************************************************************
  * @ ������  �� Test1_Task
  * @ ����˵���� Test1_Task��������
  * @ ����    ��   
  * @ ����ֵ  �� ��
  ********************************************************************/

static void Test1_Task(void* parameter)
{	
  while (1)
  {
    LED3_TOGGLE;
    vTaskDelay(3);/* ��ʱ1000��tick */
      // ��ȡ��ʱ��ʣ��ʱ��
      if(xTaskGetTickCount()<8000|uxQueueMessagesWaiting(xDataQueue)!=0|(packN>0&&rxComplete==0))
        {
            UP_USART();
            
        }else
        {
//            uint8_t readData;
//             readData = *(__IO uint32_t *)FLASH_FLAG_START_ADDR;
//             if(readData==0xFF){
//                 //APPA
//                 //EraseAPPA();
//                 if(TransportApp(APP1)==(uint8_t)(-1))
//                 {
//                     
//                 }else
//                 {
//                     JumpToApplication(FLASH_RUN_START_ADDR);
//                 }
//                 
//                
//             }else{
//                 //APPB
//                 if(TransportApp(APP2)==(uint8_t)(-1))
//                 {
//                     
//                 }else
//                 {
//                     JumpToApplication(FLASH_RUN_START_ADDR);
//                 }
//                 
//             }
            
        
        }
    
//      UBaseType_t uxMessagesWaiting = uxQueueMessagesWaiting(xDataQueue);
//      if(uxMessagesWaiting > 0){
//        
//      }else
//      {

//        
//      }
  }
}

/**********************************************************************
  * @ ������  �� Test2_Task
  * @ ����˵���� Test2_Task��������
  * @ ����    ��   
  * @ ����ֵ  �� ��
  ********************************************************************/
int t=0;
static void Test2_Task(void* parameter)
{	 
  while (1)
  { 
    vTaskDelay(3000);/* ��ʱ2000��tick */
        uint32_t readData;
        readData = *(__IO uint32_t *)FLASH_FLAG_START_ADDR;
         if(readData==0xFF){
             //APPA
             //EraseAPPA();
             TransportApp(APP1);
            JumpToApplication(FLASH_RUN_START_ADDR);
         }else{
             //APPB
             TransportApp(APP2);
             JumpToApplication(FLASH_RUN_START_ADDR);
         }
  }
}



/********************************END OF FILE****************************/
