#ifndef SDCARD_H
#define SDCARD_H

#include <stdio.h>

#include "stm32f0xx.h"
#include "ff.h"
#include "diskio.h" 
#include "fifo.h"
#include "lcd.h"
#include "diskio.h"

//insert related function declarations here 
/**
 * ^Hardware Configurations
 * @fn init_spi1_slow(): confgigures SPI1, GPIOMODER/AFR regs
 * @fn enable_sdcard()
 * @fn disable_sdcard()
 * @fn init_sdcard_io(): init spi1 slow, disable sd card
 * @fn sdcard_io_highspeed(): /256 baud rate to /4 baud rate (12MHz)
 * 
 * @fn init_lcd_spit(): lcd pin config, calls init_spi1_slow() an sdcard_io_highspeed()
 */

void init_spi1_slow();
void enable_sdcard();
void disable_sdcard(); 
void init_sdcard_io(); 
void sdcard_io_high_speed();
void init_lcd_spit();

/**
 * ^Display
 * @fn get_file(): loads filename 
 * @fn print_tft(): prints contents of a file loaded into buffer
 */
void get_file(const char* filename, char* buffer); 
void print_tft(const char* buffer); 

/**
 * ^Keyboard Interaction
 * @fn wrong_key(): changes the current character to red 
 * @fn right_key(): greys out correct stuff
 */
void wrong_key(u16 x, u16 y, char c);
void right_key(u16 x, u16 y, char c);

#endif
