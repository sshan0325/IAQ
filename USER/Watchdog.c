#include "platform_config.h"

void WatchDog_Init(void)
{
  RCC_LSICmd(ENABLE);
  
    /* Check if the system has resumed from IWDG reset */
  if (RCC_GetFlagStatus(RCC_FLAG_IWDGRST) != RESET)
  {
    /* Clear reset flags */
    
    RCC_ClearFlag();
  }
  
  IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
  // About 24sec  IWDG_Prescaler_256 : 4095
  // About 18sec  IWDG_Prescaler_256 : 2047
  // 
  // About 9sec  IWDG_Prescaler_256 : 1047
  //
  // 
  /* IWDG counter clock: LSI/32 */
  IWDG_SetPrescaler(IWDG_Prescaler_256);

  //IWDG_SetReload(4095);
  IWDG_SetReload(400);

  /* Reload IWDG counter */
  IWDG_ReloadCounter();

  /* Enable IWDG (the LSI oscillator will be enabled by hardware) */
  IWDG_Enable();
}

void WatchDog_Reset(void)
{
  IWDG_ReloadCounter();  
}