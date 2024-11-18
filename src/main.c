#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "stm32f0xx.h"
#include "ff.h"
#include "diskio.h"
#include "sdcard.h"
#include "timer.h"
#include "ps2.h"
#include "interrupts.h"
#include "gamelogic.h"


#define BUFFER_SIZE 4096
#define FONT_SIZE 16
#define MAX_LINES LCD_H / FONT_SIZE
#define LINE_HEIGHT FONT_SIZE
#define CHAR_WIDTH FONT_SIZE / 2
#define NUM_CHARS_PER_LINE lcddev.width - CHAR_WIDTH


void internal_clock();

void init_all(){
    //TODO change with navya's systick timer seed
    srand(time(NULL));
    //put all gpio and peripheral initilizations in here
}

int main (){

    return 0;
}