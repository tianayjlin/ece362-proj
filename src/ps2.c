#include <stdio.h>

#include "stm32f0xx.h"
#include "ff.h"
#include "diskio.h"
#include "sdcard.h"
#include "timer.h"
#include "ps2.h"
#include "interrupts.h"
// #include "gamelogic.h"

//insert function definitions here 
uint16_t msg = 0; //data msg of 11 bits? for the char
int ind = 0;
char keyPress;
int cor = -1;
int int_count =0;

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
    GPIOC -> PUPDR &= ~(GPIO_PUPDR_PUPDR1);
    GPIOC -> PUPDR |= GPIO_PUPDR_PUPDR1_0;//setting as pull up for clock
    //PC1-> clock, PC0-> data
}

    
// }
void init_exti (){ //interrupt when clock goes low
    RCC -> APB2ENR |= RCC_APB2ENR_SYSCFGCOMPEN;
    SYSCFG -> EXTICR[0] &= ~SYSCFG_EXTICR1_EXTI1; //clearing
    SYSCFG -> EXTICR[0] |= SYSCFG_EXTICR1_EXTI1_PC;//selecting GPIO C for pin 1
    EXTI -> FTSR |= EXTI_FTSR_FT1; //set falling edge trigger for PC1
    EXTI -> IMR |= EXTI_IMR_IM1; //unmasking interrupt
    NVIC -> ISER[0] |= 1 << EXTI0_1_IRQn; //enabling interrupt
}


uint16_t bit_bang_data(){ // bit bang 11 bits of data and output keycode?
    uint32_t portc = (GPIOC-> IDR & 0b1); //getting input data from PC0?
    uint16_t data;//11 bits of data
    //shifting in new data/overflow
    if(ind < 10){
        msg = (msg << 1) | portc; //shifting in new bit from data
        ind += 1;
    }
    else{//overflow
        ind = 0;
        data = (msg << 1) | portc; //setting 11 bits of data to check parity
        uint16_t chars = data & 0x3FC; //isolating the 8 data bits
        uint16_t reversed = reverseEnd(chars);
        int numOnes = 0;
        for (int i = 2; i< 10; i++){ //counting number of ones
            if ((data >> i) & 0x1){ //if digit at i is 1
                numOnes  = numOnes ^ 0x1; //makes numOnes 1 if odd number, 0 if even number
            }
        }
        if(numOnes ^ ((data >> 1) & 0x1)){ //if numOnes is opposite of odd parity
            //good data
            return reversed;
        }
        msg = 0;
    }
        return -1;
}

// uint16_t bit_bang_data(){ // bit bang 11 bits of data and output keycode?
//     uint16_t data = 0;
//     for(int i = 0; i<12; i++){
//         data |= (GPIOC -> IDR & 0b1) << i;
//         nano_wait(62000);
//     }
//     return data;
// }

void dummyTest(char key){
    if(key == 'a'){
        cor = 1;
    }
    else{
        cor = 0;
    }
}

uint16_t reverseEnd(uint16_t num){ //switching from lsb first to msb
    uint16_t result = 0;
    for(int i  = 0; i < 8; i++){
        result += (num && (0b1 << i) << (7-i));
    }
    return result;
}

void adjust_priorities() {
  NVIC_SetPriority(EXTI0_1_IRQn, 0x0);

}

void EXTI0_1_IRQHandler(){
    EXTI -> PR = EXTI_PR_PR1; //acknowledge pending on PC1
    //call fxn to bit bang from data line
    // int16_t key = bit_bang_data();
    // if(key > 0){
    //     keyPress = keycodes[key];
    // }
    ind += 1;
    msg = GPIOC -> IDR & 0b1; //input of pc0 (data)
    // msg = bit_bang_data();
    // dummyTest(keyPress);
    
    //call game logic fxn for key presses?
    
}




