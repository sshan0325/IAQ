/* Includes ------------------------------------------------------------------*/
#include "usart.h"
#include "RF_KEY.h"
#include "stm32f0xx.h"
#include "subfunction.h"
#include "platform_config.h"


/* Private variables ---------------------------------------------------------*/
USART_InitTypeDef                   USART_InitStructure;
extern unsigned char  Tx_LENGTH;
extern unsigned char  U2_Tx_Buffer[128];
extern unsigned char  U1_Tx_Buffer[128];
extern unsigned char  U2_Rx_Buffer[U2_RX_BUFFER_SIZE];  
extern unsigned char  U1_Paket_Type;
extern unsigned char  Reg_key_Value_Receive_Flag ;
extern unsigned char  RF_DATA_RQST_Flag;
extern unsigned char U2_Rx_DataPosition;

void USART_Configuration(void)
{
  /*******************************************************
  USART2 configured as follow:
  - BaudRate = 9600 baud  
  - Word Length = 8 Bits
  - Stop Bit = 1 Stop Bit
  - Parity = No Parity
  - Hardware flow control disabled (RTS and CTS signals)
  - Receive and transmit enabled
  ********************************************************/
  USART_InitStructure.USART_BaudRate = 9600;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  USART_Init(USART2, &USART_InitStructure);
    
  USART_Cmd(USART2, ENABLE);
      
  USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);

  
  /******************************************************* 
  USART1 configured as follow:
  - BaudRate = 57600 baud  
  - Word Length = 8 Bits
  - Stop Bit = 1 Stop Bit
  - Parity = No Parity
  - Hardware flow control disabled (RTS and CTS signals)
  - Receive and transmit enabled
  ********************************************************/
  USART_InitStructure.USART_BaudRate = 57600;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  USART_Init(USART1, &USART_InitStructure);
    
  USART_Cmd(USART1, ENABLE);
      
  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
}


/////////////////////////////////////////////////////////////////////////////////////////////////
/******************** 월패드 패킷에 대한 응답 패킷 송신  함수  *******************/
/////////////////////////////////////////////////////////////////////////////////////////////////
void USART2_TX(void)            //현관 카메라 -> 월패드 전송 함수 
{
      for(unsigned char i = 0 ; i < Tx_LENGTH ; i++)
      {
           USART_SendData(USART2,U2_Tx_Buffer[i]);  
           while(USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET); // wait for trans
           #ifdef Consol_LOG 
           if (i==5 && Reg_key_Value_Receive_Flag != SET && RF_DATA_RQST_Flag != SET && U2_Tx_Buffer[i]!=0)
           {
              if ( (U2_Tx_Buffer[i]&0x80) == 0x80 )
                printf ("\r\n[System                ] RF Communication Error\r\n");     
              if ( (U2_Tx_Buffer[i]&0x01) == 0x01 )
                printf ("\r\n[System                ] Command Tx to  WallPad(CallButton or Indicator)\r\n");     
           }
           #endif    
      }
      RS485TX_DISABLE;
 }
/////////////////////////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////////////////
/******************** RF 모듈로 패킷 송신 하는  함수 *******************/
//////////////////////////////////////////////////////////////////////////////////////////
void USART1_TX(void)
{
        unsigned int TempDataPosition=0;
        U1_Tx_Buffer[0] = U1_Paket_Type;
        for( unsigned char i =5 ; i<14 ; i++ )
        {
            TempDataPosition= U2_Rx_DataPosition+i;
            if (TempDataPosition>255)           TempDataPosition-=256;
            U1_Tx_Buffer[i+1] = U2_Rx_Buffer[TempDataPosition];
        }
        U1_Tx_Buffer[15] = 0x00;              // key type
        U1_Tx_Buffer[16] = 0x00;              // dummy
        
        for(unsigned char i = 0 ; i < 17 ; i++)
        {
             USART_SendData(USART1,U1_Tx_Buffer[i]);  
              
             while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET); // wait for trans
        }

        #ifdef U1_DATA_MONITOR
        int tmp=0;
        extern unsigned char    U1_Rx_DataPosition;
        printf ("[CAM -> RF / Position : %d  -  ]",U1_Rx_DataPosition) ;      
        for (tmp=0 ; tmp<17 ; tmp++)
        {
          printf ("%x  ",U1_Tx_Buffer[tmp]) ;
        }
        printf ("\r\n");
        #endif  
}
//////////////////////////////////////////////////////////////////////////////////////////
