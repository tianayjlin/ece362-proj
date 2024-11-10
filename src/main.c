#include "stm32f0xx.h"
#include "ff.h"
#include "diskio.h"
#include "sdcard.h"
#include "timer.h"
#include "ps2.h"
#include "interrupts.h"
#include "gamelogic.h"

#define BUFFER_SIZE 2048
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

    char buffer [BUFFER_SIZE];
    const char* filename = "handmaids_tale.txt";

    get_file(filename, buffer);
    print_tft(buffer);

    int high_score = get_high_score();

    return 0; 
}