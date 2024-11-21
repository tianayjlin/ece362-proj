#include <stdio.h>
#include "stm32f0xx.h"

#include "stm32f0xx.h"
#include <stdint.h>
#include "ff.h"
#include "diskio.h" 
#include "fifo.h"
#include "lcd.h"
#include "diskio.h"

//insert function declarations here

void setup_tim7() {
  RCC->APB1ENR |= RCC_APB1ENR_TIM7EN;
  TIM7->PSC = 48000-1;
  TIM7->ARR = 1000-1;
  TIM7->DIER |= TIM_DIER_UIE;
  NVIC->ISER[0] |= 1 << TIM7_IRQn;
}

void init_tim15(void) {
  RCC -> APB2ENR |= RCC_APB2ENR_TIM15EN;
  TIM15 -> PSC = 48000-1;
  TIM15 -> ARR = 1000 - 1;
  TIM15 -> DIER |= TIM_DIER_UDE;
  TIM15 -> CR1 |= TIM_CR1_CEN;

}

void enable_tim7(){
  TIM7 -> CR1 |= TIM_CR1_CEN;
}

void disable_tim7(void){
  TIM7 -> CR1  &= ~TIM_CR1_CEN;
}

