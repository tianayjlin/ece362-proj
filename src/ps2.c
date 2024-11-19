#include <stdio.h>
#include "stm32f0xx.h"

//insert function definitions here 
void init_gpio (){
    RCC -> AHBENR |= RCC_AHBENR_GPIOCEN;//enable clk for gpioc
    GPIOC -> MODER &= ~(GPIO_MODER_MODER0 |GPIO_MODER_MODER1); //input for pc0,pc1
}

