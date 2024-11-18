#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#include "stm32f0xx.h"
#include "ff.h"
#include "diskio.h" 
#include "fifo.h"
#include "lcd.h"
#include "diskio.h"
#include "sdcard.h"
#include "gamelogic.h"

#define BUFFER_SIZE 2048
#define FONT_SIZE 16
#define MAX_LINES LCD_H / FONT_SIZE
#define LINE_HEIGHT FONT_SIZE
#define CHAR_WIDTH FONT_SIZE / 2
#define NUM_CHARS_PER_LINE lcddev.width - CHAR_WIDTH //minus to account for space
#define NUM_QUOTES 5

/**
 * @fn start_screen(): display initial user prompt
 */
void start_screen(){
    char* start_buffer = malloc(sizeof('a') * 32);
    const char* filename = "start.txt";

    get_file(filename, start_buffer);
    memset(start_buffer + 20, 0, sizeof(start_buffer)); //force everything to be a null terminator bc im sick of this shit 
    
    print_tft(start_buffer); 
    free(start_buffer); 
}

/**
 * @fn end_screen(): displays end screen with user speed
 */
int end_screen(int total_chars, int s){
    char* buffer = malloc(sizeof('a') * 64); 
    const char* filename = "end.txt"; 

    get_file(filename, buffer);

    float wpm = wpm_calculation(total_chars, s); 

    sprintf(buffer + 32, "%d", (int)(wpm));
    
    memset(buffer + 35, 0, sizeof(buffer));
    print_tft(buffer); 
    free(buffer); 

    return (int) wpm; 
}

/**
 * @fn pick_quote: randomly picks a quote out of quote_bank 
 */
char* pick_quote(){

    char* quote_bank[NUM_QUOTES] = {
        "top200_1.txt",
        "top200_2.txt",
        "top200_3.txt",
        "top200_4.txt",
        "top200_5.txt"
    };

    //srand has been called alr in main
    return quote_bank[rand() % NUM_QUOTES];
}

/**
 * @fn get_high_score(): reads sd card for high score
 */
int get_high_score(){
    
    char* buffer = malloc(sizeof(3) * 3); 
    const char* filename = "hs.txt";

    get_file(filename, buffer);
    
    //convert string to int
    free(buffer);
    return atoi(buffer);
}

/**
 * @fn update_high_score(): saves updated high score
 */
void update_high_score(int high_score){
 
    FIL fp;
    FATFS fs; 
    char* buffer = malloc(sizeof('a') * 8);

    // load file 
    FRESULT mount = f_mount(&fs, "", 1);

    FRESULT open = f_open(&fp, "hs.txt", FA_WRITE);

    // store the high score in an external buffer
    sprintf(buffer, "%d", high_score);

    UINT bw; // need this just in case you write to write more data than the buffer itself
    FRESULT result = f_write(&fp, buffer, strlen(buffer), &bw);
    
    
    f_close(&fp);
    f_mount(NULL, "", 1);
    
    free(buffer); 
}

/**
 * @fn wpm_calculation: calculates wpm after finishing
 */
float wpm_calculation(int total_chars, int s){
    return (total_chars / 5.0) / (s / 60.0);
}

