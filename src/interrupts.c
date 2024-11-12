#include "stm32f0xx.h"
#include <stdio.h>
#include <stdint.h>
#include "interrupts.h"
#include "timers.h"
#include "ps2.h"

//insert function definitions here
int GAMETIME = 15; // needs to be set to 15 when the game starts

void TIM7_IRQHandler(){
  TIM7->SR &= ~TIM_SR_UIF;
  GAMETIME -= 1;
  write_display();
  }

void write_display() {
  if(GAMETIME >= 0){
    snprintf(GAMETIME, sizeof(GAMETIME), "%d\n");
    return;
  }  
  else
  {
    snprintf(0, sizeof(GAMETIME), "%d\n");
    return;
  }

}

