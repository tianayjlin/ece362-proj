#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "stm32f0xx.h"
#include "ff.h"
#include "diskio.h" 
#include "fifo.h"
#include "lcd.h"
#include "diskio.h"

void start_screen();
char* pick_quote();
void end_screen(int total_chars, int s);
int get_high_score();
void update_high_score(); 
float wpm_calculation(int total_chars, int s);

