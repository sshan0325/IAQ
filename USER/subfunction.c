/* Includes ------------------------------------------------------------------*/
#include "subfunction.h"
#include "stm32f0xx_tim.h"
#include "platform_config.h"
#include "usart.h"

/* Private variables ---------------------------------------------------------*/
unsigned char                   Key_Polling_Count=0;
unsigned char                   Key_State=KEY_RELEASED;
unsigned char                   KeyActiveState=KEY_INACTIVE;
static __IO uint32_t               TimingDelay;
extern unsigned char        U2_Tx_Buffer[128];
extern unsigned char U2_Rx_DataPosition;

/*************************** Flag ********************************/
extern unsigned char    Reg_Mode_Start_Flag;
extern unsigned char    Key_Reg_RQST_Flag ;
unsigned char                Key_Reg_End_Button_Flag = RESET;



void BuzzerRun(unsigned char Freq, unsigned char BuzzerCount, unsigned char Ontime, unsigned char Offtime)
{
   unsigned char CurrntBuzzerCount;
   
   for (CurrntBuzzerCount = 0 ; CurrntBuzzerCount<BuzzerCount ; CurrntBuzzerCount++)
   {
        TIM_SetCompare1(TIM3,Freq);
        GPIO_WriteBit(GPIOB,GPIO_Pin_15,(BitAction) Bit_RESET);
        Delay(Ontime);
        TIM_SetCompare1(TIM3,0);
        GPIO_WriteBit(GPIOB,GPIO_Pin_15,(BitAction) Bit_SET);
        Delay(Offtime);     
   }
}

///////////////////////////////////////////////////////////////////////////////
/******************** 호출 버튼 제어 함수 *******************/
///////////////////////////////////////////////////////////////////////////////
void Key_Polling(void)
{
    /* KEY STATE CHECK -------------------------------------------------------*/
    Check_Key_State();
    
    /* Change KEY ACTIVE STATE -> INACTIVE -----------------------------------*/
    if (Key_State == KEY_PUSHED && KeyActiveState==KEY_INACTIVE)
    {
      #ifdef Consol_LOG 
      printf ("\r\n[System                ] Call Button is Pushed");           
      #endif
      
      U2_Tx_Buffer[5] |= 0x01;
      KeyActiveState = KEY_ACTIVE;

      if( Reg_Mode_Start_Flag == SET)  // 등록 모드 시작시 호출 버튼 누름
      {
         #ifdef Consol_LOG 
         printf ("\r\n[System                ] KEY Registration Mode Start Request by call Buttom");     
         #endif
         GPIO_WriteBit(GPIOB,GPIO_Pin_15,(BitAction) Bit_SET);
         BuzzerRun(100, 1,80,10);
      }    

      if((Key_Reg_RQST_Flag == SET) && ( Reg_Mode_Start_Flag == RESET ))      
      { 
         #ifdef Consol_LOG 
         printf ("\r\n[System                ] KEY Registration Mode Stop Request by call Buttom");     
         #endif
         U2_Tx_Buffer[5] &= 0xFE;
         Key_Reg_End_Button_Flag = SET;
         GPIO_WriteBit(GPIOB,GPIO_Pin_15,(BitAction) Bit_RESET);
      }    
    }

    /* Change KEY ACTIVE STATE -> INACTIVE -----------------------------------*/
    if((Key_State == KEY_RELEASED) && (KeyActiveState == KEY_ACTIVE)) 
    {
       #ifdef Consol_LOG 
       printf ("\r\n[System                ] Call Button is Relesed");     
       #endif
       KeyActiveState = KEY_INACTIVE;
    }    
} 

void Check_Key_State(void)
{
    if( !GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_3) ) 
    {
      Key_Polling_Count++;
      if (Key_Polling_Count > KEY_POLLING_CHECKCOUNT)
      {
        Key_State = KEY_PUSHED;
      }
    }
    else
    {
      Key_Polling_Count=0;
      Key_State = KEY_RELEASED;
    }
}


void SysInit(void)
{
        RCC_Configuration();
        GPIO_Config();
        NVIC_Config();
        USART_Configuration();
        TIM_Config();  
        WatchDog_Init(); 
}


void Delay(__IO uint32_t nTime)
{ 
  TimingDelay = nTime;

  while(TimingDelay != 0);
}


void TimingDelay_Decrement(void)
{
  if (TimingDelay != 0x00)
  { 
    TimingDelay--;
  }
}

///////////////////////////////////////////////////////////////////////////
/******************** 체크섬 만드는 함수 *******************/
//////////////////////////////////////////////////////////////////////////
unsigned char Make_Checksum(void)                       //  
{
      unsigned char Checksum = 0x02;
      for(unsigned int i = 1 ; i< (Tx_LENGTH - 1) ; i++)
      {
         Checksum ^= U2_Tx_Buffer[i];
         Checksum ++;
      }
      return Checksum;
}
//////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
/******************** 체크섬 검사 함수 *******************/
//////////////////////////////////////////////////////////////////////////
unsigned char Check_Checksum(void)                      // 
{
      unsigned char Checksum = 0x02;
      unsigned char Rx_Length = 0;
      unsigned int     TempDataPosition;
      TempDataPosition= U2_Rx_DataPosition+2;
      if (TempDataPosition > 255)             TempDataPosition-=256;
      Rx_Length = U2_Rx_Buffer[TempDataPosition];
      
      for(unsigned char i = 1 ; i< (Rx_Length -1) ; i++)
      {        
        TempDataPosition = U2_Rx_DataPosition+i;
        if (TempDataPosition > 255)             TempDataPosition-=256;
        Checksum ^= U2_Rx_Buffer[TempDataPosition];
        Checksum ++;
      }
      return Checksum;
}
//////////////////////////////////////////////////////////////////////////