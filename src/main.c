#include "stm32f0xx.h"
#include "ff.h"
#include "diskio.h"
#include "sdcard.h"
#include "timer.h"
#include "ps2.h"
#include "interrupts.h"
// #include <stdint.h>
// #include <stdio.h>
// #include <fifo.h>
// #include <tty.h>

void internal_clock();

void init_all(){
    //put all gpio and peripheral initilizations in here
    setup_tim1();
}

int main (){

    //INITIALIZATIONS
    internal_clock();

    init_all();
    //TIM1_CC_IRQHandler();
    
    for(;;){
        int clock = 0;
    }
    
    return 0; 
}