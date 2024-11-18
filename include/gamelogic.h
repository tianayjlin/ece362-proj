#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "stm32f0xx.h"
#include "ff.h"
#include "diskio.h" 
#include "fifo.h"
#include "lcd.h"
#include "diskio.h"


/**
 * @fn startscreen(): displays start scren 
 * @fn pick_quote(): picks a random quote from the sd card to display on screen 
 * @fn end_screen(): displays end screen 
 * @fn get_high_score(): reads sd card for high score 
 * @fn update_high_score(): updates the high score 
 * @fn wpm_calculation(): calculates wpm for the test just taken 
 */

void start_screen();
char* pick_quote();
int end_screen(int total_chars, int s);
int get_high_score();
void update_high_score(int high_score); 
float wpm_calculation(int total_chars, int s);

