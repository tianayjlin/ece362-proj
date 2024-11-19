#ifndef INTERRUPTS_H
#define INTERRUPTS_H 

#include "timer.h"
#include "ps2.h"


//INSERT FUNCTION DECLARATIONS HERE 
void TIM7_IRQHandler(void);
void write_display(int total_chars, int s);
void init_spi1(void);
void spi_cmd(unsigned int data);
void spi_data(unsigned int data);
void spi1_init_oled(void);
void spi1_display1(const char *string);
void spi1_setup_dma(void);
void spi1_enable_dma(void);

#endif