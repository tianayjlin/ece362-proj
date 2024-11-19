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
}

int main (){

    internal_clock();

    init_all();
    
    
    return 0; 
}