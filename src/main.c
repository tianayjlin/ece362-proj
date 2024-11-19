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
#include "lcd.h"


#define BUFFER_SIZE 4096
#define FONT_SIZE 16
#define MAX_LINES LCD_H / FONT_SIZE
#define LINE_HEIGHT FONT_SIZE
#define CHAR_WIDTH FONT_SIZE / 2
#define NUM_CHARS_PER_LINE lcddev.width - CHAR_WIDTH

// #define TEST_START_SCREEN
// #define TEST_PRINT
#define TEST_SCROLLING
// #define TEST_HIGH_SCORE
// #define TEST_OLED_LCD

extern int volatile GAMETIME;
extern int correct;

void internal_clock();

void init_all(){
    //TODO change with navya's systick timer seed
    srand(time(NULL));
    // LCD_Clear(BLACK);

    #ifdef TEST_OLED_LCD

    init_spi2();
    spi2_init_oled();
    setup_tim7();

    #endif
}

int main (){

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
    const char* filename = pick_quote();
    get_file(filename, buffer);
    print_tft(buffer);

    char* p = buffer;
    int offset = 0;
    u16 x = 0;
    u16 y = 0;

    //top200_txt, for example, has 593 chars 
    while (*p && *p != '\0'){

         //show that i can force every other key to be wrong or right. very modular! heheh
        if ((x / 8) % 2){
            wrong_key(x, y, *p);
        } else {
            right_key(x, y, *p);
        }

        increment(&x, &y, buffer, &p, &offset);
    } //take care of potentially garbage values, but this isn't what this test case is for
    #endif

    #ifdef TEST_HIGH_SCORE
    //start with 80 wpm 
    int i = end_screen(100, 15); 
    update_high_score(i);
    nano_wait(1000000000);
    
    //show that you can update sd card and the wpm
    int curr_wpm = end_screen(200, 15);  
    int curr_hs = get_high_score();

    if(curr_wpm > curr_hs){
        update_high_score(curr_wpm);
    }   
    #endif

    #ifdef TEST_OLED_LCD 
        correct = 100;
    #endif

    free(buffer);
    return 0; 

}