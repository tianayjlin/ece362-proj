#include <stdio.h>
#include "stm32f0xx.h"

//insert function definitions here 
int ind = 0;
uint16_t msg = 0;
char test;

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


// void init_keyboard(){//configure gpio pins accordingly with ps2 --> NEED TO REWIRE PS-2
//     RCC -> AHBENR |= RCC_AHBENR_GPIOAEN;
//     GPIOA -> MODER &= ~(GPIO_MODER_MODER8_0 | GPIO_MODER_MODER0); //setting PA0 as input
//     GPIOA -> MODER |= GPIO_MODER_MODER8_1; //setting alt fxn for PA8
//     GPIOA -> AFR[1] |= GPIO_AFRH_AFRH2; //setting af2 (tim1_ch1) for PA8
//     //PA8-> clock, PA0-> data
// }

void setup_tim1(){
    RCC -> APB2ENR |= RCC_APB2ENR_TIM1EN;
     RCC -> AHBENR |= RCC_AHBENR_GPIOAEN;
    GPIOA -> MODER &= ~(GPIO_MODER_MODER8_0 | GPIO_MODER_MODER0); //setting PA0 as input
    GPIOA -> MODER |= GPIO_MODER_MODER8_1; //setting alt fxn for PA8
    GPIOA -> AFR[1] &= ~0xF;
    GPIOA -> AFR[1] |= 2; //setting af2 (tim1_ch1) for PA8
    //PA8-> clock, PA0-> data 
    // TIM1 -> CCMR1 &= ~TIM_CCMR1_CC1S_1; //CC1S:01 for input , mapped to TI1
    // TIM1 -> CCMR1 |= TIM_CCMR1_CC1S_0;
    // TIM1 -> PSC = 10 -1;//480kHz
    // TIM1 -> ARR = 10-1; 
    // TIM1 -> SMCR &= ~TIM_SMCR_TS; //trigger selection for internal trigger
    // TIM1 -> CCMR1 &= ~TIM_CCMR1_IC1F;//clearing filter
    // TIM1 -> CCMR1 |= TIM_CCMR1_IC1F_1;//setting filter with 4 clock cycles
    // TIM1 -> CCER &= ~TIM_CCER_CC1NP; //CC1NP/CC1P = 01 for falling edge
    // TIM1 -> CCER |= TIM_CCER_CC1P;
    // TIM1 -> CCMR1 &= ~(TIM_CCMR1_IC1PSC); //enable capture at each valid transition
    // TIM1 -> CCER |= TIM_CCER_CC1E; //enable capture from counter into the capture register
    // TIM1 -> DIER |= TIM_DIER_CC1IE; //enable interrupt
    // NVIC -> ISER[0] = 1 <<  TIM1_CC_IRQn;
    // TIM1 -> CR1 |= TIM_CR1_CEN;
//     // //IDK how to configure the tim1 to tim1_ch1 or if that's even needed
//     // //Ask about using a timer in capture mode to trigger an interrupt on the falling edge of a gpio pin
//     // //Also need to find the input filter duration, PS-2, takes 5-25 microseconds to transition from low to high/vice versa
    TIM1 -> CR1 &= ~TIM_CR1_CEN;//disable
    TIM1 -> CCMR1 &= ~TIM_CCMR1_CC1S; //CC1S: 01, input, TI1
    TIM1 -> CCMR1 |= TIM_CCMR1_CC1S_0;
    TIM1 -> CCER &= ~TIM_CCER_CC1NP;
    TIM1 -> CCER |= TIM_CCER_CC1P;//setting trigger to falling edge: CC1NP/CC1P = 01
    TIM1 -> CCER |= TIM_CCER_CC1E; //enable capture from counter into the capture register
    
    TIM1 -> PSC = 15 -1;//480kHz
    TIM1 -> ARR = 10-1;
    TIM1 -> SMCR &= ~TIM_SMCR_SMS;
    TIM1 -> SMCR |= TIM_SMCR_TS; //setting TS: 111 External trigger input
    TIM1 -> CCMR1 &= ~(TIM_CCMR1_IC1PSC); //enable capture at each valid transition??
    TIM1 -> SMCR &= ~TIM_SMCR_ETF;
    TIM1 -> SMCR |= TIM_SMCR_ETF_2 | TIM_SMCR_ETF_0;//setting external trigger filter to 0101: 8 cycles
    TIM1 -> DIER |= TIM_DIER_CC1IE; //enable interrupt
    NVIC -> ISER[0] = 1 <<  TIM1_CC_IRQn;
    TIM1 -> CR1 |= TIM_CR1_CEN;//enable

   
}

void keyChar(uint16_t msg){
    switch (msg){
        case 56:
            test = 'a';
            break;
    }
        
}

void TIM1_CC_IRQHandler(){
    TIM1 -> SR &= ~ TIM_SR_CC1IF; //clearing interrupt flag
    // TIM1 -> SR &= ~TIM_SR_UIF;
    ind += 1;
    msg = (msg << 1) | (GPIOA -> IDR & 0b1);//input of PA0
    if(ind % 33 == 0){
        msg = (msg & 0b11111111100) >> 2; //isolating data bits
        keyChar(msg);
    }
    
}

