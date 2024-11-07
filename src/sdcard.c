#include <stdio.h>

#include "stm32f0xx.h"
#include "ff.h"
#include "diskio.h" 
#include "fifo.h"
#include "lcd.h"
#include "diskio.h"

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
    // RCC -> AHBENR |= RCC_AHBENR_GPIOBEN; 
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

void print_tft(void){  
    
    //lcd init is not defined in the lcd.h
    

    FIL fp;  //jfile pointer
    FATFS fs; //file structure

    //load abc.txt into fs, mounts sd card readable
    FRESULT mount = f_mount(&fs, "", 1);

    //load file into file pointer fp
    FRESULT open = f_open(&fp, "top200_1.txt", FA_READ);

    long int size = 10000;
    char buffer[size]; //makes sure that this matches the number of bytes btr (each element in an array is a byte)
    UINT br; //records the total amount of bytes read
    FRESULT read = f_read(&fp, buffer, size, &br);
    
    //"fclose"
    FRESULT unmount = f_mount(&fs, NULL, 0);
    LCD_Setup(); 
    // enable_sdcard();
    LCD_Clear(BLACK);     
    LCD_DrawTXT(0, 0, WHITE, BLACK, buffer, 16, 0);
}