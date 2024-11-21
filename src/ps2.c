#include <stdio.h>
#include "stm32f0xx.h"

//insert function definitions here 
int ind = 0;
uint16_t msg = 0;
char test;

void setup_tim1(){
    RCC -> APB2ENR |= RCC_APB2ENR_TIM1EN;
     RCC -> AHBENR |= RCC_AHBENR_GPIOAEN;
    GPIOA -> MODER &= ~(GPIO_MODER_MODER8_0 | GPIO_MODER_MODER0); //setting PA0 as input
    GPIOA -> MODER |= GPIO_MODER_MODER8_1; //setting alt fxn for PA8
    GPIOA -> AFR[1] &= ~0xF;
    GPIOA -> AFR[1] |= 2; //setting af2 (tim1_ch1) for PA8
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

void keyChar(uint16_t msg){ //translating data to ascii char
    switch (msg){
        case 56:
            test = 'a';
            break;
        case 76:
            test = 'b';
            break;
        case 132:
            test = 'c';
            break;
        case 196:
            test = 'd';
            break;
        case 36:
            test = 'e';
            break;
        case 212:
            test = 'f';
            break;
        case 44:
            test = 'g';
            break;
        case 204:
            test = 'h';
            break;
        case 194:
            test = 'i';
            break;
        case 220:
            test = 'j';
            break;
        case 66:
            test = 'k';
            break;
        case 210:
            test = 'l';
            break;
        case 92:
            test = 'm';
            break;
        case 140:
            test = 'n';
            break;
        case 34:
            test = 'o';
            break;
        case 178:
            test = 'p';
            break;
        case 168:
            test = 'q';
            break;
        case 180:
            test = 'r';
            break;
        case 216:
            test = 's';
            break;
        case 52:
            test = 't';
            break;
        case 60:
            test = 'u';
            break;
        case 84:
            test = 'v';
            break;
        case 184:
            test = 'w';
            break;
        case 68:
            test = 'x';
            break;
        case 172:
            test = 'y';
            break;
        case 88:
            test = 'z';
            break;
        case 148:
            test = ' ';
            break;
    }

        
}

void TIM1_CC_IRQHandler(){
    TIM1 -> SR &= ~ TIM_SR_CC1IF; //clearing interrupt flag
    ind += 1;
    msg = (msg << 1) | (GPIOA -> IDR & 0b1);//input of PA0
    if(ind % 33 == 0){ //when letter is pressed
        msg = (msg & 0b11111111100) >> 2; //isolating data bits
        keyChar(msg);
    }
    
}

