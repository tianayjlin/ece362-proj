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

#include "dac.h"
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
// #define TEST_SCROLLING
// #define TEST_HIGH_SCORE
// #define TEST_OLED_LCD
// #define TEST_TYPING
#define FINAL_DEMO

// OLED TIMER VARS
extern int volatile GAMETIME;
extern int correct;

// PS2 VARS
extern int ind; 
extern char test;
extern u16 msg;
extern int nack;

void internal_clock();

void init_all(){
    
  //make sure srand is 
    srand(tim6_seed());
    setup_tim1();
  
    //figure out how to clear screen without infinite loop,
    #if defined(TEST_OLED_LCD) || defined(FINAL_DEMO)

    init_spi2();
    spi2_init_oled();
    setup_tim7();
    update_high_score(10);

    #endif
}

#ifdef FINAL_DEMO
    void demo(){

        start_screen();

        char* buffer = malloc(sizeof('a') * BUFFER_SIZE);
        
        int loaded = 0;

        while (!loaded){   
            
            if(test == ' '){
                
                //load the screen with random prompt
                char* filename = pick_quote();
                get_file(filename, buffer);
                print_tft(buffer); 

                //start test
                enable_tim7();
                
                loaded = 1;
            }
        }

        char* p = buffer; 
        int offset = 0; 
        u16 x = 0; 
        u16 y = 0;

        while(loaded && GAMETIME > 0){

            //if need to acknowledge a new keypress
            if(nack){

                if(test == *p){

                    correct++; 
                    right_key(x, y, *p);
                    increment(&x, &y, buffer, &p, &offset); 

                } else { 

                    wrong_key(x, y, *p);

                }

                nack = 0; //finished logic on current key press
            }
        }


        //timer handler automatically calls end screen
        
        free(buffer);
    }
    
#endif




int main (){

    internal_clock();
    init_all();

    // char *buffer = malloc(sizeof('a') * BUFFER_SIZE);

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
        nano_wait(1000000000);
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

    #ifdef TEST_TYPING 
    #ifdef TEST_TYPING
  
       const char* filename = pick_quote();
       get_file(filename, buffer);
       print_tft(buffer);

       char* p = buffer;
       int offset = 0;
       u16 x = 0;
       u16 y = 0;

       while (1) {
           int top = 7;
           for(int i = 0; i < top; i++){
               right_key(x, y, *p);
               increment(&x, &y, buffer, &p, &offset);
               nano_wait(100000000);
           }
           wrong_key(x, y, *p);
       }

       while (1) {
           wrong_keypress_sound();
           nano_wait(100000000);
       }
      
   #endif
        
    #endif

    #ifdef FINAL_DEMO
        demo();
    #endif

    // free(buffer);
    return 0; 
}
