/* Includes ---------------------------------------------------------------*/
#include "platform_config.h"
#include "stdio.h"
#include "usart.h"
#include "subfunction.h"
#include "RF_KEY.h"    

/* Private variables ---------------------------------------------------------*/
#define CO2Addr  (0x5A<<1) // the slave address (example)
#define TempAddr  (0x44<<1) // the slave address (example)

//////////////////////////////////////////////////////////////////////////////////////////////
/***************************** Main Function *********************************/
//////////////////////////////////////////////////////////////////////////////////////////////
int main(void)
{  
  int temp=0;
  
  SysInit();

  if (SysTick_Config(SystemCoreClock / 1000))                   while (1);

  printf ("\r\n[System                ] Power On");     
  printf ("\r\n[System                ] System Init Done.\r\n");     

  while (1)
  {
      WatchDog_Reset();                        // 왓치독 이베트 셋팅 함수
      temp++;

      if (temp>200000)
      {
        temp=0;
        ReadTempCmd();
        Delay(10);
        ReadCO2();
        Delay(10);
        ReadTemp();
      }       
      //RF_Key_Packet_handler();              // RF 모듈 패킷 핸들러
      //Packet_handler();                          // 월 패드 패킷 핸들러
      //Key_Polling();                               // 호출 버튼 감지 함수
  }
}


void ReadTemp()
{
  float temp_val;
  float Rhum_val;
  uint8_t received_data[6];
  int timeout;

  timeout=2000;
  while(I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY) == SET)
  {
      if((timeout--) == 0)
      {
        printf ("\r\nError 5");
        break;
      }
  }  

  I2C_TransferHandling(I2C1, TempAddr, 2, I2C_SoftEnd_Mode, I2C_Generate_Start_Write);
  timeout=2000;
  while(I2C_GetFlagStatus(I2C1,I2C_FLAG_TXIS) == RESET)
  {
     if((timeout--) == 0)
      {
        printf ("\r\nError 6");
        break;
      }
  }      
  
  
  I2C_SendData(I2C1, 0xE0);  
  timeout=2000;
  while(I2C_GetFlagStatus(I2C1,I2C_FLAG_TXIS) == RESET)
  {
      if((timeout--) == 0)
      {
        printf ("\r\nError 7-1");
        break;
      }
  }    

  I2C_SendData(I2C1, 0x00);  
  timeout=2000;
  while(I2C_GetFlagStatus(I2C1,I2C_FLAG_TXE) == RESET)
  {
      if((timeout--) == 0)
      {
        printf ("\r\nError 7-2");
        break;
      }
  }      

  I2C_GenerateSTOP(I2C1, ENABLE);
  timeout=2000;
  while(I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY))
  {
      if((timeout--) == 0)
      {
        printf ("\r\nError 8");
        break;
      }
  }
 

  I2C_TransferHandling(I2C1, TempAddr, 6, I2C_AutoEnd_Mode, I2C_Generate_Start_Read);

  while (!I2C_GetFlagStatus(I2C1, I2C_FLAG_RXNE));
  received_data[0] = I2C_ReceiveData(I2C1); // read one byte and request another byte
  while (!I2C_GetFlagStatus(I2C1, I2C_FLAG_RXNE));
  received_data[1] = I2C_ReceiveData(I2C1); // read one byte and request another byte
  while (!I2C_GetFlagStatus(I2C1, I2C_FLAG_RXNE));
  received_data[2] = I2C_ReceiveData(I2C1); // read one byte and request another byte
  while (!I2C_GetFlagStatus(I2C1, I2C_FLAG_RXNE));
  received_data[3] = I2C_ReceiveData(I2C1); // read one byte and request another byte
  while (!I2C_GetFlagStatus(I2C1, I2C_FLAG_RXNE));
  received_data[4] = I2C_ReceiveData(I2C1); // read one byte and request another byte
  while (!I2C_GetFlagStatus(I2C1, I2C_FLAG_RXNE));  
  received_data[5] = I2C_ReceiveData(I2C1); // read one byte and request another byte

  I2C_GenerateSTOP(I2C1, ENABLE);
  while(I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY));        

  temp_val = (175*(received_data[0]*256+received_data[1]));
  temp_val = (temp_val/65535)-45;
  Rhum_val = 100*(received_data[3]*256+received_data[4]);
  Rhum_val = Rhum_val/65535;
  printf ("\r\nTemp : %.1f ",temp_val);            
  printf ("\r\nHumi : %.1f ",Rhum_val);              
  printf ("\r\n\r\n");
  //printf ("   (%d, %d, %d, %d, %d, %d) \r\n\r\n",received_data[0],received_data[1],received_data[2],received_data[3],received_data[4],received_data[5]);            
}

void ReadTempCmd()
{
  int timeout;

  timeout=2000;
  while(I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY) == SET)
  {
      if((timeout--) == 0)
      {
        printf ("\r\nError 1");
        break;
      }
  }  

  I2C_TransferHandling(I2C1, TempAddr, 2, I2C_SoftEnd_Mode, I2C_Generate_Start_Write);
  timeout=2000;
  while(I2C_GetFlagStatus(I2C1,I2C_FLAG_TXIS) == RESET)
  {
     if((timeout--) == 0)
      {
        printf ("\r\nError 2");
        break;
      }
  }      
  
  
  I2C_SendData(I2C1, 0x20);  
  timeout=2000;
  while(I2C_GetFlagStatus(I2C1,I2C_FLAG_TXIS) == RESET)
  {
      if((timeout--) == 0)
      {
        printf ("\r\nError 3-1");
        break;
      }
  }    

  I2C_SendData(I2C1, 0x32);  
  timeout=2000;
  while(I2C_GetFlagStatus(I2C1,I2C_FLAG_TXE) == RESET)
  {
      if((timeout--) == 0)
      {
        printf ("\r\nError 3-2");
        break;
      }
  }      

  I2C_GenerateSTOP(I2C1, ENABLE);
  timeout=2000;
  while(I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY))
  {
      if((timeout--) == 0)
      {
        printf ("\r\nError 4");
        break;
      }
  }
}


void ReadCO2()
{
  uint8_t received_data[9];
  unsigned int Co2_val;

  while(I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY) == SET);

  I2C_TransferHandling(I2C1, CO2Addr, 9, I2C_AutoEnd_Mode, I2C_Generate_Start_Read);

  while (!I2C_GetFlagStatus(I2C1, I2C_FLAG_RXNE));
  received_data[0] = I2C_ReceiveData(I2C1); // read one byte and request another byte
  while (!I2C_GetFlagStatus(I2C1, I2C_FLAG_RXNE));
  received_data[1] = I2C_ReceiveData(I2C1); // read one byte and request another byte
  while (!I2C_GetFlagStatus(I2C1, I2C_FLAG_RXNE));
  received_data[2] = I2C_ReceiveData(I2C1); // read one byte and request another byte
  while (!I2C_GetFlagStatus(I2C1, I2C_FLAG_RXNE));
  received_data[3] = I2C_ReceiveData(I2C1); // read one byte and request another byte
  while (!I2C_GetFlagStatus(I2C1, I2C_FLAG_RXNE));
  received_data[4] = I2C_ReceiveData(I2C1); // read one byte and request another byte
  while (!I2C_GetFlagStatus(I2C1, I2C_FLAG_RXNE));
  received_data[5] = I2C_ReceiveData(I2C1); // read one byte and request another byte
  while (!I2C_GetFlagStatus(I2C1, I2C_FLAG_RXNE));
  received_data[6] = I2C_ReceiveData(I2C1); // read one byte and request another byte
  while (!I2C_GetFlagStatus(I2C1, I2C_FLAG_RXNE));
  received_data[7] = I2C_ReceiveData(I2C1); // read one byte and request another byte
  while (!I2C_GetFlagStatus(I2C1, I2C_FLAG_RXNE));
  received_data[8] = I2C_ReceiveData(I2C1); // read one byte and request another byte

  I2C_GenerateSTOP(I2C1, ENABLE);
  while(I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY));        

  if (0 == received_data[2])
  {
    Co2_val = received_data[0]*256 + received_data[1];
    printf ("\r\nCo2 Value : %d",Co2_val);
  }
  else
  {
    printf ("\r\nCo2 Value : Not Ready");
  }
    
  //printf ("   (%d, %d, %d, %d, %d, %d, %d, %d, %d) \r\n\r\n",received_data[0],received_data[1],received_data[2],received_data[3],received_data[4],received_data[5],received_data[6],received_data[7],received_data[8]);            
  
}


//////////////////////////////////////////////////////////////////////////////////////////////

