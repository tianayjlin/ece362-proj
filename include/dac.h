#ifndef DAC_H
#define DAC_H
#include "stm32f0xx.h"


void wrong_keypress_sound(void);
void init_wavetable(void);
void setup_dac(void);
void TIM6_DAC_IRQHandler();
void init_tim6(void);
int tim6_seed(void);

#endif