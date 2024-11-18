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


// #define TEST_START_SCREEN
#define TEST_PRINT
// #define TEST_SCROLLING
// #define TEST_HIGH_SCORE 

void internal_clock();

void init_all(){
    //TODO change with navya's systick timer seed
    srand(time(NULL));
    //put all gpio and peripheral initilizations in here
}

int main (){

    //INITIALIZATIONS
    internal_clock();

    init_all();
    char *buffer = malloc(sizeof('a') * BUFFER_SIZE);

    #ifdef TEST_START_SCREEN
        start_screen(); 
    #endif

    #ifdef TEST_PRINT

    const char* filename = pick_quote();
    get_file(filename, buffer);
    print_tft(buffer);

    #endif

    #ifdef TEST_SCROLLING
    char* p = buffer + 552;
    int offset = 27;
    u16 x = 206;
    u16 y = MAX_LINES;

    //top200_txt, for example, has 593 chars 
    for(int i = 0; i < 593; i++){
        increment(&x, &y, buffer, &p, &offset);
    }

    #endif

    #ifdef TEST_HIGH_SCORE

    //start with 80 wpm 
    int i = end_screen(100, 15); 
    update_high_score(i);

    nano_wait(5000000000);
    
    //show that you can update sd card and the wpm
    int curr_wpm = end_screen(200, 15);  
    if(curr_wpm > get_high_score()){
        update_high_score(curr_wpm);
    }   
    
    

    #endif

    free(buffer);
    return 0;
}