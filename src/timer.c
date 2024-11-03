#include <stdio.h>

//insert function declarations here





  void setup_tim7() {
  RCC->APB1ENR |= RCC_APB1ENR_TIM7EN;
  TIM7->PSC = 48000-1;
  TIM7->ARR = 999;
  TIM7->DIER |= TIM_DIER_UIE;
  NVIC->ISER[0] |= 1 << TIM7_IRQn;
  TIM7->CR1 |= TIM_CR1_CEN;
}
