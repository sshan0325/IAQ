/* Includes ------------------------------------------------------------------*/
#include "passing.h"
#include "usart.h"
#include "RF_KEY.h"
#include "subfunction.h"
#include "platform_config.h"

/* Private variables ---------------------------------------------------------*/
//Seungshin Using
////////////////// UART CH 1 ////////////////////
extern unsigned char U1_Rx_Buffer[U1_RX_BUFFER_SIZE] ;
extern unsigned char    U1_Rx_DataPosition;
extern unsigned char    U1_Rx_Count ;
////////////////// UART CH 2 ////////////////////
extern unsigned char    U2_Rx_Buffer[U2_RX_BUFFER_SIZE]; 
unsigned char               U2_Tx_Buffer[128] = {0} ;  
extern unsigned char    U2_Rx_DataPosition;
extern unsigned char    U2_Rx_DataSavePosition;

///////////////// DEVICE STATE /////////////////
unsigned char Key_Reg_RQST_Flag = RESET;
extern unsigned char    KeyActiveState;

///////////////// Flag /////////////////////////////
extern unsigned char RFKey_Detected;

//Need to Check
extern unsigned char U2_Rx_Compli_Flag ;
unsigned char Temp_buffer[17] ={0};
extern unsigned char U2_Rx_Count ;
unsigned char TX_CMD = 0x00 ;
unsigned char RF_Data_Confirm_Flag = RESET;
unsigned char Key_Reg_End_Flag = RESET;
unsigned char RF_DATA_RQST_Flag = RESET;
unsigned char Reg_Mode_Start_Flag = RESET;
extern unsigned char Key_Reg_End_Button_Flag ;
extern unsigned char RF_Key_CNT;
unsigned char Tx_LENGTH = 22;
extern unsigned char Reg_key_Value_Receive_Flag ;
unsigned char U1_Tx_Buffer[128]= {0};
unsigned char Key_Reg_Timeout_flag = RESET;
extern unsigned int Key_Reg_Timeout_CNT ;
extern unsigned char Reg_Fail_Flag;
extern unsigned char RF_Communi_Fail ;
unsigned char Key_Reg_U1_Send_Flag = RESET;
unsigned char Reg_Compli_Flag = RESET;
unsigned char Key_Save_Flag = RESET;
unsigned char U1_Paket_Type = 0x00;
unsigned char RF_Comm_Time_Out_Flag = RESET;
unsigned char Status_Value_Clear_Flag = RESET;
extern unsigned char RF_Key_Data[128];
extern unsigned char CNT ;
unsigned char CMD_Buffer[8] = { RF_STATUS_RQST , RF_STATUS_CLR_RQST , RF_DATA_RQST , RF_DATA_CONFIRM_RQST ,
                                                REG_MODE_START_RQST , REG_KEY_DATA_RQST , REG_MODE_END_RQST , EQUIP_INFOR_RQST};
/* Private function prototypes -----------------------------------------------*/

/* Private functions ---------------------------------------------------------*/


// 10 바이트 전송시간 1.04 ms /

///////////////////////////////////////////////////////////////////////////////
/******************** 월패드  패킷 처리  함수 *******************/
//////////////////////////////////////////////////////////////////////////////
void Packet_handler(void)       
{
    unsigned int IDField, LengthFiled;

    while(U2_Rx_Count>2)
    {
        IDField = U2_Rx_DataPosition+1;
          if (IDField > 255)              IDField-=256;
        LengthFiled =  U2_Rx_DataPosition+2;
          if (LengthFiled > 255)          LengthFiled-=256;
        if(U2_Rx_Buffer[U2_Rx_DataPosition] == STX && (U2_Rx_Buffer[IDField] == RF_Camera_ID))
        {
              if ( U2_Rx_Buffer[LengthFiled] <=  U2_Rx_Count )
              {
                  #ifdef  U2_DATA_MONITOR_1
                  unsigned int TempDataPosition;
                  printf ("\r\nRx Data Count :  %d :    , Data Position : %d    , DataSavePosition : %d", U2_Rx_Count, U2_Rx_DataPosition, U2_Rx_DataSavePosition);
                  printf ("\r\nU2  Rx Data(Length : %d) :", U2_Rx_Buffer[LengthFiled]);
                  for (unsigned char tmp=0 ; tmp< U2_Rx_Buffer[LengthFiled] ; tmp ++)
                  {
                      TempDataPosition = U2_Rx_DataPosition+tmp;
                      if (TempDataPosition > 255)             TempDataPosition-=256;
                      printf ("%x  ", U2_Rx_Buffer[TempDataPosition]);
                  }                  
                  #endif                
                  
                  if( PacketValidation() == VALID) 
                  {
                      CMD();
                      //Delay(12);                              //  idle time Delay 
                      Delay(4);                              //  idle time Delay 
                      Response();
                  }
                  
                  U2_Rx_Count-=U2_Rx_Buffer[LengthFiled];
                  U2_Rx_DataPosition+=U2_Rx_Buffer[LengthFiled];
              }
              
              else 
              {
                  break;
              }                              
        }
        else
        {
            U2_Rx_Count--;
            U2_Rx_DataPosition++;       
        }
    }        
}


////////////////////////////////////////////////////////////////////////////////
/******************** 월패드  패킷 검사 함수  *******************/
///////////////////////////////////////////////////////////////////////////////
unsigned char PacketValidation(void)   
{
    unsigned char Result=0;
    unsigned char Rx_Length=0;
    unsigned int IDField, LengthFiled, CMDFiled, CRCField;

    IDField = U2_Rx_DataPosition+1;
      if (IDField > 255)              IDField-=256;
    LengthFiled =  U2_Rx_DataPosition+2;
      if (LengthFiled > 255)          LengthFiled-=256;    
    CMDFiled = U2_Rx_DataPosition+3;
      if (CMDFiled > 255)          CMDFiled-=256;    
  
    Rx_Length = U2_Rx_Buffer[LengthFiled];

    for(unsigned char i = 0 ; i < 8 ; i ++)
    {
        if(U2_Rx_Buffer[CMDFiled] == CMD_Buffer[i])  
        {
            Result ++; 
        }
    }

    CRCField = U2_Rx_DataPosition+Rx_Length-1;
      if (CRCField > 255)          CRCField-=256;          
    
    if( U2_Rx_Buffer[CRCField] == Check_Checksum())        
    {
        Result ++; 
    }
    else
    {
    }

    if(Result != VALID )
    {
#if 1
          #ifdef DataValication_Check_LOG 
          unsigned int TempDataPosition;
          printf ("\r\n\r\n\r\n\r\n\r\n[System                ] U2_Rx_Data is Invalid!!!  Result : %d      ", Result);
          for (unsigned char tmp=0 ; tmp<U2_Rx_Buffer[LengthFiled] ; tmp++)
          {
            TempDataPosition = U2_Rx_DataPosition+tmp;
            if (TempDataPosition > 255)         TempDataPosition-=256;
            printf ("%x  ",U2_Rx_Buffer[TempDataPosition]) ;
          }          
          printf ("\r\nCMD Code is : %x  ",U2_Rx_Buffer[CMDFiled]) ;          
          printf ("\r\nChecksum : %x / Received Data : %x", Check_Checksum(), U2_Rx_Buffer[CRCField]) ;
          #endif                                   
#endif
    }

    return Result;
 }


///////////////////////////////////////////////////////////////////////////
/******************** 명령어 검사 함수 *******************/
//////////////////////////////////////////////////////////////////////////
unsigned char CMD_Check(unsigned char *CMD, unsigned char CNT)    
{
  char CMD_Check = 0;
  for(char i = 0 ; i < CNT ; i ++)
  {
      if(CMD[3] == CMD_Buffer[i])         CMD_Check++;
  }

  if(CMD_Check)
    return 1;
  else
    return 0;
}
//////////////////////////////////////////////////////////////////////////




////////////////////////////////////////////////////////////////////////////////////////
/******************** 월패드  패킷에 대한 응답 처리 함수  *******************/
///////////////////////////////////////////////////////////////////////////////////////
void Response(void)                                                          
{
    int TempDataPosition;
    GPIO_WriteBit(GPIOB,  GPIO_Pin_0 , (BitAction) Bit_SET);  // 485 Trans pin Enable 
    U2_Tx_Buffer[0] = STX ;
    U2_Tx_Buffer[1] = RF_Camera_ID ;
    U2_Tx_Buffer[2] = Tx_LENGTH ;
    U2_Tx_Buffer[3] = TX_CMD;
    TempDataPosition = U2_Rx_DataPosition+4;
    if (TempDataPosition >255)                TempDataPosition-=256;
    U2_Tx_Buffer[4] = U2_Rx_Buffer[TempDataPosition] ;
    U2_Tx_Buffer[Tx_LENGTH-1] = Make_Checksum() ;

    /************* 평상시 RF 데이터 인식 시 응답패킷 저장 루틴 **************/
    if((RF_DATA_RQST_Flag == SET))
    {
          if(RF_Key_CNT>=5)       RF_Key_CNT=5;
          Tx_LENGTH = ( 16 * RF_Key_CNT ) + 7 ;
           for(unsigned char i = 6 ; i < ( Tx_LENGTH - 1 ) ; i ++ ) 
           {                                                                 // 패킷 길이 23 체크섭 전까지 버퍼  [23-1] = [22] ( 체크섬 자리) (<) 이므로 체크섬 자리바로 앞!
               U2_Tx_Buffer[i] = RF_Key_Data[i-6] ;
           }
           U2_Tx_Buffer[2] = Tx_LENGTH;                            // Tx_LENGTH 변수에만 넣고 버퍼에 넣지 않으면 값이 않들어 감
           U2_Tx_Buffer[Tx_LENGTH-1] = Make_Checksum();

           //RF_DATA_RQST_Flag = RESET; // 15.05.20 전광식.
           RF_Key_CNT = 0;
           Reg_key_Value_Receive_Flag = RESET;
    }
          
    /*************  스마트키 등록시 응답패킷 저장 루틴  **************/          
    if((Reg_key_Value_Receive_Flag == SET))
   {    
        if(RF_Key_CNT>=5)       RF_Key_CNT=5;
        Tx_LENGTH = ( 16 * RF_Key_CNT ) + 7 ;
        for(unsigned char i = 6 ; i < ( Tx_LENGTH - 1 ) ; i ++ )    // 22
        {                                                                              // 패킷 길이 23 체크섭 전까지 버퍼  [23-1] = [22] ( 체크섬 자리) (<) 이므로 체크섬 자리바로 앞!
            TempDataPosition = U1_Rx_DataPosition-RF_KEY_PACKET_SIZE+i-5;
            if (TempDataPosition < 0 ) TempDataPosition+=256;
            U2_Tx_Buffer[i] = U1_Rx_Buffer[TempDataPosition] ;                            //16까지 저장,  UART 1에서 들어오는 데이터 필드 [0] 은 버리고 전송
        }
        U2_Tx_Buffer[2] = Tx_LENGTH;                                         // Tx_LENGTH 변수에만 넣고 버퍼에 넣지 않으면 값이 않들어 감
        U2_Tx_Buffer[Tx_LENGTH-1] = Make_Checksum();

        RF_Key_CNT = 0;
        Reg_key_Value_Receive_Flag = RESET;
   }

   USART2_TX();
   
   if((RF_DATA_RQST_Flag == SET))
   {
        U2_Tx_Buffer[5]=0;
        U2_Tx_Buffer[6]=0;
        RF_DATA_RQST_Flag = RESET; 
   }

   #ifdef  U2_DATA_MONITOR_1
   printf ("\r\nU2  Tx Data(Length : %d) :", Tx_LENGTH);
   for (unsigned char tmp=0 ; tmp< Tx_LENGTH ; tmp ++)
   {
       printf ("%x  ", U2_Tx_Buffer[tmp]);
   }
   #endif      
          
          
   /*************  응답 패킷 송신 후  예외 처리  루틴  **************/     
   if(Reg_Fail_Flag == SET)                 // 등록모드에서  이미 등록된 키일 경우 시 플래그 데이터 비트 재설정
  {
        Reg_Fail_Flag = RESET;
        U2_Tx_Buffer[5] &= 0xFB;
   }
   if( RF_Communi_Fail == SET)          // 등록 모드에서  통신 실패 시 플래그 데이터 비트 재설정
   {
        RF_Communi_Fail = RESET;
        U2_Tx_Buffer[5] &= 0xF7;
   }

   U2_Rx_Compli_Flag = RESET;
   CNT = 0;
}




////////////////////////////////////////////////////////////////////////////////////
/******************** 각 명령어별 동작  함수  *******************/
///////////////////////////////////////////////////////////////////////////////////
void CMD(void)
{
    unsigned char Requested_CMD;
    unsigned int TempDataPosition;
    TempDataPosition = U2_Rx_DataPosition+3;
    if (TempDataPosition >255)          TempDataPosition-=256;
    Requested_CMD = U2_Rx_Buffer[TempDataPosition];

    switch(Requested_CMD)
    {
        /***************** 0x11 상태 값 요청 ****************/
        case RF_STATUS_RQST:  // 0x11 상태 값 요청 
        { 
              Tx_LENGTH = 8 ; 
              TX_CMD = RF_STAUS_RSPN ;
              
               if(KeyActiveState == KEY_ACTIVE)
               {
                      U2_Tx_Buffer[5] |= 0x01;
               }

               if(Key_Reg_RQST_Flag)                      // 등록 모드 중 평상 시 폴링시 RF 모듈를 등록 모드 종료시킴
               {                                                     // 중간에 월패트가  꺼지고 나서 다시 켰을때 
                      GPIO_WriteBit(GPIOB,GPIO_Pin_15,(BitAction) Bit_RESET);  //  LED OFF
                      U1_Paket_Type = 0xA0;                // 등록 종료 RF 모듈에 알림
                      USART1_TX();
               }         
               
              U1_Paket_Type = 0xD0;  
              U1_Tx_Buffer[1] = 0xD0;
              TempDataPosition = U2_Rx_DataPosition+5;
              if (TempDataPosition >255)          TempDataPosition-=256;              
              U1_Tx_Buffer[2] = U2_Rx_Buffer[TempDataPosition];
              TempDataPosition = U2_Rx_DataPosition+6;
              if (TempDataPosition >255)          TempDataPosition-=256;    
              U1_Tx_Buffer[3] = U2_Rx_Buffer[TempDataPosition];
              
              USART1_TX();

              RF_Comm_Time_Out_Flag = SET; //  타임 아웃
               
               if((RF_Data_Confirm_Flag == SET) || (RF_DATA_RQST_Flag == SET)|| (Status_Value_Clear_Flag == SET) 
                  || (Reg_Mode_Start_Flag == SET) || (Key_Reg_RQST_Flag == SET) ||  (Key_Reg_End_Flag == SET) ) 
               {                                                         //  각 기능 구현 중 다시 평상시 패킷 들어올때 평상시 패킷 전송 
                     Clear_Tx_Buffer();

                     RF_Data_Confirm_Flag = RESET;             // 각 기능에 대한 플래그 초기화
                     Key_Reg_End_Flag = RESET;
                     Reg_Mode_Start_Flag = RESET;
                     Key_Reg_RQST_Flag = RESET;
                     Key_Reg_End_Flag = RESET;
                     Status_Value_Clear_Flag = RESET;
                     Key_Reg_U1_Send_Flag = RESET;             // 등록 실패 후 다시 재등록이 안됨 
                     Reg_key_Value_Receive_Flag = RESET;
               } 

               RF_DATA_RQST_Flag = RESET;                        //전, 사이에 키인식 되면 키값이 들어오는것 방지
        }
        break;
        
        
        /***************** 0x12  상태값 해제 요청 ****************/      
        case RF_STATUS_CLR_RQST:  // 0x12  상태값 해제 요청
        {
              Tx_LENGTH = 7 ; 

              Status_Value_Clear_Flag = SET;

              TX_CMD = RF_STAUS_CLR_RSPN;
              
              TempDataPosition = U2_Rx_DataPosition+5;
              if (TempDataPosition >255)          TempDataPosition-=256;                    
              if((U2_Rx_Buffer[TempDataPosition] & 0x80 ) == 0x80)  // 상태 값 해제 패킷에 따라 비트 클리어 
              {
                      U2_Tx_Buffer[5] &= 0x7F;
              }
              
              if((U2_Rx_Buffer[TempDataPosition] & 0x01 ) == 0x01)
              {
                       U2_Tx_Buffer[5] &= 0xFE;
               }
         }
        break;
        
        /***************** 0x21 RF 데이터 요청  ****************/      
        case RF_DATA_RQST:      // 0x21 RF 데이터 요청 
        {
              #ifdef Consol_LOG        
              printf ("\r\n[System                ] RF Data is Requested.");     
              #if 0
              printf ("\r\nReceivce U2 Data :");
              for (unsigned char tmp=0 ; tmp< 7 ; tmp ++)
              {
                  TempDataPosition = U2_Rx_DataPosition+tmp;
                  if (TempDataPosition >255)          TempDataPosition-=256;                              
                  printf ("%x  ", U2_Rx_Buffer[TempDataPosition]);
              }              
              #endif         //0
              #endif         //Consol_LOG           
              
              TempDataPosition = U2_Rx_DataPosition+5;
              if (TempDataPosition >255)          TempDataPosition-=256;                              
              RF_Key_CNT = U2_Rx_Buffer[TempDataPosition];  // 요청한 데이터 패킷 갯수만 보내기 위함 
              
              TX_CMD = RF_DATA_RSPN; //  평상시 스마트 키 인식시 

              if(RF_Key_CNT>=5)       RF_Key_CNT=5;
              U2_Tx_Buffer[5] = RF_Key_CNT;
              
              RF_DATA_RQST_Flag = SET;

              if(RFKey_Detected == RESET)
              {
                    U2_Tx_Buffer[5] = 0x00;
                    RF_Key_CNT = 0;
                    Tx_LENGTH = 7;
                    RF_DATA_RQST_Flag = RESET;
              }
        }
        break;

        /***************** 0x22  스마트키 데이터 확인  ****************/      
        case RF_DATA_CONFIRM_RQST:  // 0x22  스마트키 데이터 확인
        {
                unsigned char KEY_Number_to_Confirm = 0;
                #ifdef Consol_LOG        
                printf ("\r\n[System                ] RF Data Confirm is Requested.");     
                #endif                    
                TempDataPosition = U2_Rx_DataPosition+5;
                if (TempDataPosition >255)          TempDataPosition-=256;                                        
                KEY_Number_to_Confirm = U2_Rx_Buffer[TempDataPosition];                   // 요청 갯수 저장
                RF_Data_Confirm(KEY_Number_to_Confirm);                 // 전송 데이터 확인 함수
                TX_CMD = RF_DATA_CONFIRM_RSPN ;                       
                U2_Tx_Buffer[5] = KEY_Number_to_Confirm;
                Tx_LENGTH = 9;
                RF_DATA_RQST_Flag = RESET;  
                RF_Data_Confirm_Flag = SET;
                RFKey_Detected = RESET;
         }
         break;
         
        /***************** 0x31  스마트키 등록 모드 시작  ****************/  
        case REG_MODE_START_RQST:  // 0x31  스마트키 등록 모드 시작
        {
                #ifdef Consol_LOG 
                printf ("\r\n[System                ] Regist mode Start Request");     
                #endif          
                TX_CMD = REG_MODE_START_RSPN ;    
                Key_Reg_RQST_Flag = SET;
                Tx_LENGTH = 7;
                
                RF_DATA_RQST_Flag = RESET;     //  동록 모드 시작  전, 사이에 키인식 되면 키값이 들어오는것 방지
                Reg_Mode_Start_Flag = SET;
         }
         break;  
         
         
        /************************** 0x32  스마트키 등록 요청 ********************************/          
        case REG_KEY_DATA_RQST:  // 0x32  스마트키 등록 요청
        {
               //#ifdef Consol_LOG        
               //printf ("\r\n[System                ] RF KEY Data is Requested.");     
               //#endif                    
               TX_CMD = REG_KEY_DATA_RSPN ;    
              
               Key_Reg_RQST_Flag = SET;
               Reg_Mode_Start_Flag = RESET;
               
               Key_Reg_Timeout_CNT =0; // 등록 모드 타임아웃 초기화 
               Key_Reg_Timeout_flag = SET;  // 등록 모드 타임아웃 세팅                
               
               GPIO_WriteBit(GPIOB,GPIO_Pin_15,(BitAction) Bit_SET);  // 중간에 현관카메라가 꺼져지고 나서 다시 켰을때 등록모드를 유지하기 위해서 
             
   
               if(Key_Info_Compare())                   // 이전 키값과 다르면 
               {
                      Key_Reg_U1_Send_Flag = RESET;
                      Reg_Compli_Flag = RESET;
                      Key_Save_Flag = RESET;
               }

               if(Key_Save_Flag == RESET)  // 월패드에서 등록 실패시 키정보 비교하기위해 임시저장 루틴
               {
                       unsigned int TempDataPosition;
                       for(char i = 5 ; i < 14 ; i++)
                       {
                            TempDataPosition = U2_Rx_DataPosition+i;
                            if (TempDataPosition >255)                TempDataPosition-=256;
                               Temp_buffer[i] = U2_Rx_Buffer[TempDataPosition];
                       }
                       
                       Key_Save_Flag = SET;
               }

               if(Key_Reg_U1_Send_Flag == RESET) 
               {
                     Key_Reg_U1_Send_Flag = SET;
                     U1_Paket_Type = 0xC0;                    // 등록 요청 RF 모듈에 알림 
                     USART1_TX();
                }
            
              if(Reg_key_Value_Receive_Flag == SET)  // 등록 키값 받았을때
              { 
                    if(Key_Reg_End_Button_Flag == RESET)             // 등록 종료버튼  안눌렀을 시
                    {
                            RF_Key_CNT = 1;
                            Tx_LENGTH = 23;
                             
                            U2_Tx_Buffer[5] = 0x01;

                            Reg_Compli_Flag = SET; 
                     }

                    if(Key_Reg_End_Button_Flag == SET)             // 등록 종료버튼 누를 시 받은 패킷 지워야!!
                    {
                          Reg_key_Value_Receive_Flag = RESET;

                          Tx_LENGTH = 7;
                          U2_Tx_Buffer[5] |= 0x02;
                         
                          Key_Reg_End_Button_Flag = RESET;  // -> 클리어 시점 다시 정하기 
                    }
              } // END of  if (등록 키값 받았을때 )

              if(Reg_key_Value_Receive_Flag == RESET)  // 등록 키값  안받았을때 
              {
                     if((!(Key_Info_Compare())) && (Reg_Compli_Flag == SET))  // 패킷 못가져 갔을때  이전 키값과 같을때 
                     {
                            RF_Key_CNT = 1;
                            Tx_LENGTH = 23;
                             
                            U2_Tx_Buffer[5] = 0x01;
                            
                            Reg_key_Value_Receive_Flag = SET;
                     }

                     if(Reg_Compli_Flag == RESET)
                     {
                              if(Key_Reg_End_Button_Flag == RESET)              // 등록 종료버튼  안눌렀을 시
                              {
                                    Tx_LENGTH = 7;
                                    U2_Tx_Buffer[5] &= 0xFC;
                              }
                              
                              if(Key_Reg_End_Button_Flag == SET)             // 등록 종료버튼 누를 시
                              {
                                   Tx_LENGTH = 7;
                                   U2_Tx_Buffer[5] |= 0x02;
                                    Key_Reg_End_Button_Flag = RESET;
                              }
                       }
              }  // END of if (등록 키값 안받았을때 )
         }// END of case
         break;  

        /*************************** 0x33  스마트키 등록 모드 종료  ********************************/
        case REG_MODE_END_RQST:  // 0x33  스마트키 등록 모드 종료 
        {
               #ifdef Consol_LOG        
               printf ("\r\n[System                ] RF Key Registrration Mode Stop is Requested.");     
               #endif                    
               TX_CMD = REG_MODE_END_RSPN ;    
               Tx_LENGTH = 7;
               U2_Tx_Buffer[5] = 0x01;
                RF_DATA_RQST_Flag = RESET;     //  동록 모드 시작  전에 키인식 되면 키값이 들어오는것 방지 

                Key_Reg_End_Flag = SET;
                Key_Reg_RQST_Flag = RESET;
                Reg_Mode_Start_Flag = RESET;
              
                BuzzerRun(100, 1,80,10);

               GPIO_WriteBit(GPIOB,GPIO_Pin_15,(BitAction) Bit_RESET);  //  LED OFF
               
                for(char i = 5 ; i < 14 ; i++)
                {
                        Temp_buffer[i] = 0;
                }  
               
               U1_Paket_Type = 0xA0;  // 등록 종료 RF 모듈에 알림
               USART1_TX();
         }
         break;  
         
         /*************************** 0x01 기기 정보 요청 ********************************/
        case EQUIP_INFOR_RQST:              // 0x01 기기 정보 요청
        {
              #ifdef Consol_LOG        
              printf ("\r\n[System                ] Device Information Request.");     
              #endif
              TX_CMD = EQUIP_INFOR_RSPN ; 
              Tx_LENGTH = 15;

              U2_Tx_Buffer[5] = 0x00;
              U2_Tx_Buffer[6] = 0x00;
              U2_Tx_Buffer[7] = 0x01;
              U2_Tx_Buffer[8] = 0x0D;
              U2_Tx_Buffer[9] = 0x0C;
              U2_Tx_Buffer[10] = 0x04;
              U2_Tx_Buffer[11] = 0x01; 
              U2_Tx_Buffer[12] = 0x00;
              U2_Tx_Buffer[13] = 0x60;
        }
        break;
     } //  END of switch 
} // END of CMD function
  


                  
/******************** 이전 키 정보 비교 함수 *******************/
unsigned char Key_Info_Compare(void)            //  
{
      char Compare_CNT = 0;
      unsigned int TempDataPosition;
      for(char i = 5 ; i < 14 ; i++)
      {
          TempDataPosition = U2_Rx_DataPosition+i;
          if (TempDataPosition> 255)            TempDataPosition-=256;
        
              if(Temp_buffer[i] == U2_Rx_Buffer[TempDataPosition])
                Compare_CNT ++;
      }  
     if(Compare_CNT == 9)       
       return 0 ;    
     else                              
       return 1 ;
}
                  
