#include <stdio.h>
#include "stm32f0xx.h"

//insert function definitions here 
static char keycodes[128] = {
    0,   27,  '1', '2', '3', '4', '5', '6', '7', '8', '9',  '0', '-',  '=',  '\b', '\t',                                                    /* <-- Tab */
    'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[',  ']', '\n', 0,                                                                   /* <-- control key */
    'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`', 0,    '\\', 'z',  'x',  'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0, '*', 0, /* Alt */
    ' ',                                                                                                                                    /* Space bar */
    0,                                                                                                                                      /* Caps lock */
    0,                                                                                                                                      /* 59 - F1 key ... > */
    0,   0,   0,   0,   0,   0,   0,   0,   0,                                                                                              /* < ... F10 */
    0,                                                                                                                                      /* 69 - Num lock*/
    0,                                                                                                                                      /* Scroll Lock */
    0,                                                                                                                                      /* Home key */
    0,                                                                                                                                      /* Up Arrow */
    0,                                                                                                                                      /* Page Up */
    '-', 0,                                                                                                                                 /* Left Arrow */
    0,   0,                                                                                                                                 /* Right Arrow */
    '+', 0,                                                                                                                                 /* 79 - End key*/
    0,                                                                                                                                      /* Down Arrow */
    0,                                                                                                                                      /* Page Down */
    0,                                                                                                                                      /* Insert Key */
    0,                                                                                                                                      /* Delete Key */
    0,   0,   0,   0,                                                                                                                       /* F11 Key */
    0,                                                                                                                                      /* F12 Key */
    0,                                                                                                                                      /* All other keys are undefined */
};


void init_keyboard(){//configure gpio pins accordingly with ps2
    RCC -> AHBENR |= RCC_AHBENR_GPIOCEN;
    GPIOC -> MODER &= ~(GPIO_MODER_MODER1 | GPIO_MODER_MODER0); //setting PC1, PC0 as inputs
    //GPIOC -> PUPDR &= ~(GPIO_PUPDR_PUPDR1);
    //GPIOC -> PUPDR |= GPIO_PUPDR_PUPDR1_0;//setting as pull up for clock
    //PC1-> clock, PC0-> data
}

void setup_tim1(){
    RCC -> APB2ENR |= RCC_APB2ENR_TIM1EN; 
    
    TIM1 -> CCMR1 &= ~(TIM_CCMR1_IC1PSC); //enable capture at each valid transition
    TIM1 -> CCER |= TIM_CCER_CC1E; //enable capture from counter into the capture register
    TIM1 -> DIER |= TIM_DIER_CC1IE; //enable interrupt
}

