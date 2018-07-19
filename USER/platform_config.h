/* Includes ------------------------------------------------------------------*/
#ifndef _PLATFORM_CONFIG_H
#define _PLATFORM_CONFIG_H
#include "stm32f0xx.h"
#include "stdio.h"
#endif 

/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/
#define Consol_LOG              
//#define DataValication_Check_LOG      
//#define U1_DATA_MONITOR
//#define U2_DATA_MONITOR_1

#define SET                     1
#define RESET                   0

#define STX                     0x02
#define RF_Camera_ID            0xA2

#define EQUIP_INFOR_RQST        0x01
#define EQUIP_INFOR_RSPN        0X81
#define RF_STATUS_RQST          0x11
#define RF_STAUS_RSPN           0x91
#define RF_STATUS_CLR_RQST      0x12
#define RF_STAUS_CLR_RSPN       0x92
#define RF_DATA_RQST            0x21
#define RF_DATA_RSPN            0xA1
#define RF_DATA_CONFIRM_RQST    0x22
#define RF_DATA_CONFIRM_RSPN    0xA2
#define REG_MODE_START_RQST     0x31
#define REG_MODE_START_RSPN     0xB1
#define REG_KEY_DATA_RQST       0x32
#define REG_KEY_DATA_RSPN       0xB2
#define REG_MODE_END_RQST       0x33
#define REG_MODE_END_RSPN       0xB3

/* Private function prototypes -----------------------------------------------*/
void TIM_Config(void);
void GPIO_Config(void);
void NVIC_Config(void);
void RCC_Configuration(void);
void EXTI4_15_Config(void);
void Packet_handler(void); 
void RF_Key_Packet_handler(void);
void Delay(__IO uint32_t nTime);
void TimingDelay_Decrement(void);
void CMD(void);
void Clear_Tx_Buffer(void);
void RF_Data_Confirm(unsigned char CNT);
void RF_Data_Save(unsigned char CNT, unsigned char *U1_Rx);
unsigned char CMD_Check(unsigned char *CMD, unsigned char CNT);
unsigned char Key_Info_Compare(void);
void Data_Save_71(unsigned char RF_CNT, unsigned char *U1_71);
void WatchDog_Init(void);
void WatchDog_Reset(void);
void WatchDogTask(void);
void Response(void);
void SysInit(void);
void ReadCO2();
void ReadTempCmd();
void ReadTemp();
/***************************************************************END OF FILE****/
