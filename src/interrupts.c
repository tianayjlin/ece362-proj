#include "stm32f0xx.h"
#include <stdio.h>
#include <stdint.h>
#include "interrupts.h"
#include "timers.h"
#include "ps2.h"

//insert function definitions here


void TIM7_IRQHandler(){
  TIM7->SR &= ~TIM_SR_UIF;
  GAMEOVER = 1;
  }