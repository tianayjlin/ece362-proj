#include "stm32f0xx.h"
#include "ff.h"
#include "diskio.h"
#include "sdcard.h"
#include "timer.h"
#include "ps2.h"
#include "interrupts.h"
// #include "gamelogic.h"

#define BUFFER_SIZE 2048

void internal_clock();

void init_all(){
    //put all gpio and peripheral initilizations in here
    init_keyboard();     
    init_exti();
}

int main (){

    //INITIALIZATIONS
    internal_clock();

    init_all();

    for(;;){
        int clock = 0;

    }
    // while (1) {
    //     if ((GPIOC->IDR & GPIO_IDR_1) == 0) { // Check if PC1 (clock) is low
    //         // Action when clock goes low (e.g., read PC0 for data)
    //         clock++; 

    //         int data_bit = (GPIOC->IDR & GPIO_IDR_0) ? 1 : 0;
    //     }
    // }

    
    return 0; 
}