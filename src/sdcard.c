#include <stdio.h>
#include <string.h>
#include <math.h>

#include "stm32f0xx.h"
#include "ff.h"
#include "diskio.h" 
#include "fifo.h"
#include "lcd.h"
#include "diskio.h"
#include "dac.h"

#define BUFFER_SIZE 2048
#define FONT_SIZE 16
#define MAX_LINES LCD_H / FONT_SIZE
#define LINE_HEIGHT FONT_SIZE
#define CHAR_WIDTH FONT_SIZE / 2
#define X_MAX_CHARS_PIX LCD_W - CHAR_WIDTH //minus to account for space
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

void init_spi1_slow(){
    //configure SPI1 AND GPIOB MODER AND AFR registers for pins 
    //PB3 (SCK), PB4 (MISO), and PB5 (MOSI).
    RCC -> AHBENR |= RCC_AHBENR_GPIOBEN;
    GPIOB -> MODER &= ~(GPIO_MODER_MODER3 | GPIO_MODER_MODER4 | GPIO_MODER_MODER5);
    GPIOB -> MODER |= GPIO_MODER_MODER3_1 | GPIO_MODER_MODER4_1 | GPIO_MODER_MODER5_1;

    //alternate functions
    GPIOB -> AFR[0] &= ~(GPIO_AFRL_AFRL3 | GPIO_AFRL_AFRL4 | GPIO_AFRL_AFRL5); 

    //SPI configurations
    RCC -> APB2ENR |= RCC_APB2ENR_SPI1EN; 
    SPI1 -> CR1 &= ~SPI_CR1_SPE; 

    SPI1 -> CR1 |= SPI_CR1_BR | SPI_CR1_MSTR | SPI_CR1_SSM | SPI_CR1_SSI; 
    SPI1 -> CR2 |= SPI_CR2_DS_2 | SPI_CR2_DS_1 | SPI_CR2_DS_0 | SPI_CR2_FRXTH; 
    SPI1 -> CR2 &= ~SPI_CR2_DS_3;

    SPI1 -> CR1 |= SPI_CR1_SPE; 
}

void enable_sdcard(){   
    //set pb2 low to enable SD CARD 
    GPIOB -> BSRR |= GPIO_BSRR_BR_2;
}

void disable_sdcard(){
    //set pb2 high to enable SD CARD
    GPIOB -> BSRR |= GPIO_BSRR_BS_2;
}

void init_sdcard_io(){
    
    init_spi1_slow(); 
    
    // //configure PB2 as an output
    //clock has already been enabled in init_spi1_slow() and not disabled
    //set as output
        GPIOB -> MODER |= GPIO_MODER_MODER2_0;

    disable_sdcard();
}

void sdcard_io_high_speed(){
    SPI1 -> CR1 &= ~SPI_CR1_SPE; 
    
    // 48 MHz -> 12 MHz
    SPI1 -> CR1 |= SPI_CR1_BR_0;
    SPI1 -> CR1 &= ~(SPI_CR1_BR_1 | SPI_CR1_BR_2);

    SPI1 -> CR1 |= SPI_CR1_SPE;
}

void init_lcd_spi(){
    
    //PB8, 11, 14 as GPIO outputs
    RCC -> AHBENR |= RCC_AHBENR_GPIOBEN; 
    GPIOB -> MODER &= ~(GPIO_MODER_MODER8 | GPIO_MODER_MODER11 | GPIO_MODER_MODER14);
    GPIOB -> MODER |= GPIO_MODER_MODER8_0 | GPIO_MODER_MODER11_0 | GPIO_MODER_MODER14_0; 


    init_spi1_slow(); 
    sdcard_io_high_speed(); 
    
}


/**
 * ^Getting Words + Display
 * @fn get_file(): loads <buffer> with information from <filename>
 * @fn print_tft(): print the file in 
 */ 

/**
 * @example 
 * 
 *  char buffer [BUFFER_SIZE];
    const char* filename = "abc.txt";

    get_file(filename, buffer);
 *
 *
 */
void get_file(const char* filename, char* buffer){
    FIL fp;
    FATFS fs; 
    FRESULT mount = FR_NO_FILE; 

    while(mount != FR_OK){
        mount = f_mount(&fs, "", 1);
        nano_wait(200000000);
    }

    FRESULT open = f_open(&fp, filename, FA_READ);


    UINT br; //records the total amount of bytes read
    FRESULT read = f_read(&fp, buffer, BUFFER_SIZE, &br);
    
    //"fclose"
    FRESULT close = f_close(&fp);
    FRESULT unmount = f_unmount("");
}

void print_tft(char* buffer){  
    LCD_Setup();
    LCD_Clear(BLACK);     
    
    LCD_DrawTXT(0, 0, WHITE , BLACK, buffer, FONT_SIZE, 0);
}



/**
 * ^Invoke within keyboard IRQ Handlers 
 * @fn wrong_key
 * @fn right_key
 */

void wrong_key(u16 x, u16 y, char c){
    LCD_DrawChar(x, y, WHITE, RED, c, FONT_SIZE, 0);
    wrong_keypress_sound();
}

void right_key(u16 x, u16 y, char c){
    LCD_DrawChar(x, y, WHITE, GRAY, c, FONT_SIZE, 0);
}

int calculate_word_length(const char* p){
    
    const char* word_end = p; 

    while(*word_end && *word_end != ' ' && *word_end != '\0'){
        word_end++;
    }

    //++to account for trailing space.
    return ++word_end - p;
}
/**
 * @example 
 * incrment(&x, &y, buffer, p, &offset), where p initially points to buffer
 */
void increment(u16* x, u16* y, char* buffer, char** p, int* offset){

    //increment handles letters, but you have to implement wrap around for the next word
    if(**p == ' '){
        int word_length = calculate_word_length(*p + 1);
        *x = *x + (word_length * CHAR_WIDTH) > X_MAX_CHARS_PIX ? 0 : *x + CHAR_WIDTH;
    } else { 
        *x += CHAR_WIDTH;
    }

    //new line
    if(*x == 0){
        
        //out of bounds, increment OR scroll
        if  (*y > lcddev.height - FONT_SIZE){
        
            //THIS WILL BE BUGGGGY ALL WE CAN DO IS PRAYYYY THAT NO ONE REACHES THIS CONDITINO AHAHAHAA
            // scroll(buffer, *p, offset); 
            *y -= LINE_HEIGHT; 
        } 
        
        else{
            
            //if the beginning of the new line
            if(!(*offset)){
                *offset = *p + 1 - buffer;
            }

            *y += LINE_HEIGHT;
        }
    }

    //increment the file ptr
    (*p)++;    
}





