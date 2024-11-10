#include "stm32f0xx.h"
#include <stdio.h>
#include <stdint.h>
#include "interrupts.h"
#include "timers.h"
#include "ps2.h"

//insert function definitions here
int GAMETIME; // needs to be set to 15 when the game starts

void TIM7_IRQHandler(){
  TIM7->SR &= ~TIM_SR_UIF;
  GAMETIME -= 1;

    if(GAMETIME == 1){
      //end of game function
    }
  }

void write_display() {
  snprintf(GAMETIME, sizeof(GAMETIME), "%d\n");
}

