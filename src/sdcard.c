#include <stdio.h>

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

    
    //load file into file pointer fp
    // char *argv[20];
    // argv[1] = "top200_1.txt";
    // ls(0, argv);
    FRESULT open = f_open(&fp, filename, FA_READ);


    UINT br; //records the total amount of bytes read
    FRESULT read = f_read(&fp, buffer, BUFFER_SIZE, &br);
    
    //"fclose"
    FRESULT close = f_close(&fp);
    // FRESULT unmount = f_mount(NULL, "",  0);
    FRESULT unmount = f_unmount("");
}

void print_tft(const char* buffer){  
    LCD_Setup(); 
    // enable_sdcard();
    LCD_Clear(BLACK);     
    LCD_DrawTXT(0, 0, WHITE, BLACK, buffer, FONT_SIZE, 0);
}



/**
 * ^Invoke within keyboard IRQ Handlers 
 * @fn wrong_key
 * @fn right_key
 */

void wrong_key(u16 x, u16 y, char c){
    LCD_DrawChar(x, y, WHITE, RED, c, FONT_SIZE, 0);
}

void right_key(u16 x, u16 y, char c){
    LCD_DrawChar(x, y, WHITE, GRAY, c, FONT_SIZE, 0);
}

/**
 * @param u16 x: current lcd coordinate
 * 
 */
void scroll(char* buffer, char* p, int* line_offset){

    //char* p is the last thing fitting on the curr screen
    //force a null terminator into the buffer in order so all the completed text will still be greyed out
    char temp = *(p + 1);
    *(p + 1) = '\0'; 
    
    //start printing on offset. we know that this amount of data will fit with one line open
    LCD_DrawTXT(0, 0, WHITE, GRAY, buffer + *line_offset, FONT_SIZE, 0);

    //overwrite null terminator and continue printing from here
    *(p + 1) = temp; 

    //print as much as you can on the new line. 
    LCD_DrawTXT(0, MAX_LINES - 1, WHITE, BLACK, p + 1, FONT_SIZE, 0);
     
    *line_offset = 0;
}

/**
 * @example 
 * incrment(&x, &y, buffer, p, &offset), where p initially points to buffer
 */
void increment(u16* x, u16* y, char* buffer, char* p, int* offset){

    //wraparound for x (0 indexed)
    *x = *x < NUM_CHARS_PER_LINE - 1 ? *x + CHAR_WIDTH : 0; 

    //new line
    if(*x == 0){
        
        //out of bounds, increment OR scroll
        if  (*y > lcddev.height - FONT_SIZE){
            
            scroll(buffer, p, offset); 
            *y -= LINE_HEIGHT; 
        } 
        else{
            
            //if the beginning of the new ine
            if(!(*offset)){
                *offset = p + 1 - buffer;
            }

            *y += LINE_HEIGHT;
        }
    }

    //increment the file ptr
    p++;
}





