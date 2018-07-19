/* Include -------------------------------------------------------------------*/
#include "usart.h"

/* Private define ------------------------------------------------------------*/
extern unsigned char Tx_LENGTH;
extern unsigned char U2_Rx_Buffer[U2_RX_BUFFER_SIZE];  

/* Private function prototypes -----------------------------------------------*/
void BuzzerRun(unsigned char Freq, unsigned char BuzzerCount, unsigned char Ontime, unsigned char Offtime);
void Key_Polling(void);
void Check_Key_State(void);
unsigned char Make_Checksum(void);
unsigned char Check_Checksum(void);

/* KEY STATE -----------------------------------------------------------------*/
#define KEY_PUSHED                      0
#define KEY_RELEASED                    1
#define KEY_POLLING_CHECKCOUNT          50

/* KEY ACTIVE ----------------------------------------------------------------*/
#define KEY_ACTIVE                      1
#define KEY_INACTIVE                    0