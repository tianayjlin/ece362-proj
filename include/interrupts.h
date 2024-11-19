#ifndef INTERRUPTS_H
#define INTERRUPTS_H 

#include "timer.h"
#include "ps2.h"


//INSERT FUNCTION DECLARATIONS HERE 
void TIM7_IRQHandler(void);
void write_display(void);//int total_chars, int s
void init_spi2(void);
void spi_cmd(unsigned int data);
void spi_data(unsigned int data);
void spi2_init_oled(void);
void spi2_display1(const char *string);
void spi2_setup_dma(void);
void spi2_enable_dma(void);

#endif