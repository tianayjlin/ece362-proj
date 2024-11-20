// LIBRARIES
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// INCLUDE
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

// CONSTANTS
#define BUFFER_SIZE 4096
#define FONT_SIZE 16
#define MAX_LINES LCD_H / FONT_SIZE
#define LINE_HEIGHT FONT_SIZE
#define CHAR_WIDTH FONT_SIZE / 2
#define NUM_CHARS_PER_LINE lcddev.width - CHAR_WIDTH

// TEST CASES
// #define TEST_START_SCREEN // tests to make sure start screen is appropriately displaying
// #define TEST_PRINT // rand select prompt to print to tft 
// #define TEST_SCROLLING // increment()
// #define TEST_HIGH_SCORE // display and update high score
// #define TEST_OLED_LCD // integration with OLED 
// #define TEST_DAC //integraTion with dac for wrong keypress 
#define DEMO1 

extern int volatile GAMETIME;
extern int correct;

void internal_clock();

void init_all(){

    //make sure srand is 
    srand(tim6_seed());
    
    //figure out how to clear screen without infinite loop,
    #if defined(TEST_OLED_LCD) || defined(DEMO1)

    init_spi2();
    spi2_init_oled();
    setup_tim7();
    update_high_score(80);
    LCD_Setup();
    LCD_Clear(BLACK);
    correct = 0; 

    #endif
}

void demo1(){
    
    char *buffer = malloc(sizeof('a') * BUFFER_SIZE);
    char* filename = pick_quote();
    get_file(filename, buffer);

    char* user_buffer = malloc(sizeof('a') * 256); 
    filename = "demo1.txt";
    get_file(filename, user_buffer);
    

    start_screen();
    nano_wait(100000000000); //replace with polling space bar
    
    print_tft(buffer);
    enable_tim7(); //start game upon user space bar press


    /*TESTING 80 wpm user*/
    char* user = user_buffer; 
    char* p = buffer;
    int offset = 0;
    u16 x = 0;
    u16 y = 0;

    while (*user){

        if(*user == *p){
            right_key(x, y, *p);
            increment(&x, &y, buffer, &p, &offset);
            correct++;
        }
        else{
            wrong_key(x, y, *p); 
        }

        nano_wait(126000000); //emulates speed of 80 wpm
        user++; //next key press
    }
    
    //not calling necessary 
    free(user_buffer); 
    free(buffer);

    end_screen(correct, 15);
}

/*=============END DECLARATIONS===============*/

int main (){

    internal_clock();
    init_all();
    
    

    #ifdef TEST_START_SCREEN
        start_screen(); 
    #endif

    #if defined(TEST_PRINT) || defined(TEST_SCROLLING)

    char *buffer = malloc(sizeof('a') * BUFFER_SIZE);
    const char* filename = pick_quote();
    get_file(filename, buffer);
    print_tft(buffer);

    #endif

    #ifdef TEST_SCROLLING

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
        enable_tim7();
        correct = 100;
        // for(;;);
    #endif

    #ifdef TEST_DAC 
  
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

    #ifdef DEMO1
        demo1();
    #endif 


    #if defined(TEST_PRINT) || defined(TEST_SCROLLING)
        free(buffer);
    #endif


    return 0; 
}