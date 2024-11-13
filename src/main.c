#include "stm32f0xx.h"
#include "diskio.h"
#include "sdcard.h"
#include "interrupts.h"
#include "timer.h"
#include "ps2.h"
// #include <stdint.h>
// #include <stdio.h>
// #include <fifo.h>
// #include <tty.h>

void internal_clock();

void init_all(){
    //put all gpio and peripheral initilizations in here  
}

int main (){

    //INITIALIZATIONS
    internal_clock();

    init_all();
    

    init_spi1();
    spi1_init_oled();
    //spi1_display1("hello");
    
    return 0; 
}