#include <stdio.h>

#include "stm32f0xx.h"
#include "ff.h"
#include "diskio.h" 
#include "fifo.h"
#include "lcd.h"
#include "diskio.h"

#define BUFFER_SIZE 2048
#define FONT_SIZE 16
#define MAX_LINES LCD_H / FONT_SIZE
#define LINE_HEIGHT FONT_SIZE
#define CHAR_WIDTH FONT_SIZE / 2
#define NUM_CHARS_PER_LINE lcddev.width - CHAR_WIDTH //minus to account for space


// void start_screen(){

// }

// void end_screen(){

// }

float wpm_calculation(int total_chars, int s){

    //check int/float calculation
    return (total_chars / 5.0) / (s / 60.0);
}

