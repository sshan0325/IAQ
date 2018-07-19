/* Includes ------------------------------------------------------------------*/
#include "platform_config.h"

/* Private function prototypes -----------------------------------------------*/
void RF_Data_Confirm(unsigned char CNT);
void RF_Packet_Erase_handler(void);


/* Private define ------------------------------------------------------------*/
#define RF_KEY_PACKET_SIZE      17

#define RF_KEY_CHECK            0xDA
#define RF_KEY_REG_SUCC         0xCA
#define RF_KEY_REG_FAIL         0xBA
#define RF_KEY_RECOGNITION      0x71


/* MACRO ---------------------------------------------------------------------*/
#define RS485TX_ENABLE          GPIO_WriteBit(GPIOB,  GPIO_Pin_0 , (BitAction) Bit_SET)
#define RS485TX_DISABLE         GPIO_WriteBit(GPIOB,  GPIO_Pin_0 , (BitAction) Bit_RESET);
