#include <stdio.h>
#include <stdlib.h>

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


void start_screen(){
    //TODO
}

void end_screen(){
    //TODO
}

int get_high_score(){
    
    char buffer [BUFFER_SIZE];
    const char* filename = "high_score.txt";

    get_file(filename, buffer);
    
    //convert string to int
    return atoi(buffer);
}

void update_high_score(int high_score){
    FIL fp;
    FATFS fs; 
    char buffer[8];

    //load file into .txt
    FRESULT mount = f_mount(&fs, "", 1);

    FRESULT open = f_open(&fp, "high_score.txt", FA_WRITE);

    //store the high score in an external buffer
    sprintf(buffer, "%d", high_score);

    UINT bw; //need this just in case you write to write more data than the buffer itself
    FRESULT result = f_write(&fp, buffer, strlen(buffer), &bw);
    
    
    f_close(&fp);
    
    //unmount file system
    f_mount(NULL, "", 1);

}



float wpm_calculation(int total_chars, int s){

    //check int/float calculation
    return (total_chars / 5.0) / (s / 60.0);
}

