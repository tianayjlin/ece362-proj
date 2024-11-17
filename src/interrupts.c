#include "stm32f0xx.h"
#include <stdio.h>
#include <stdint.h>
#include "interrupts.h"
#include "timer.h"
#include "ps2.h"
#include "ff.h"
#include "diskio.h" 
#include "fifo.h"
#include "lcd.h"
#include "diskio.h"
#include <string.h>

//insert function definitions here

int volatile GAMETIME = 16; // needs to be set to 15 when the game starts, takes one second to initialize
char buf[19];
int HIGHSCORE = 180;  //for integration get_high_score();;

void TIM7_IRQHandler(){
  TIM7->SR &= ~TIM_SR_UIF;
  GAMETIME -= 1;
  write_display();
  spi1_display1(buf);
  }

void write_display() {
  if(GAMETIME > 0){
    snprintf(buf, sizeof(buf), "%d ", GAMETIME);
    return;
  }
  else
  {
    snprintf(buf, sizeof(1), "%d ", 0);
    //call high score function
    nano_wait(100000000);
    snprintf(buf, sizeof(buf), "high score: %d wpm", HIGHSCORE);
    //end of game logic goes here
    return;
  }

}

//change tim7 to tim15 to

//trigger dm UPDATE WITH TIM7

void init_spi1() {
    RCC -> AHBENR |= RCC_AHBENR_GPIOAEN;

    //Configure NSS, SCK and MOSI signals of SPI1 to pins PA15, PA5 and PA7, respectively.
    GPIOA -> MODER &= ~(GPIO_MODER_MODER15_0 | GPIO_MODER_MODER5_0| GPIO_MODER_MODER7_0);

    GPIOA -> MODER |= (GPIO_MODER_MODER15_1 | GPIO_MODER_MODER5_1 | GPIO_MODER_MODER7_1);

    GPIOA -> AFR[0] &= ~(GPIO_AFRL_AFRL5 | GPIO_AFRL_AFRL7 );
    GPIOA -> AFR[1] &= ~GPIO_AFRH_AFRH7;
    RCC -> APB2ENR |= RCC_APB2ENR_SPI1EN;

    //Ensure that the CR1_SPE bit is clear first. 
    SPI1 -> CR1 &= ~SPI_CR1_SPE;
    //Set the baud rate as low as possible (maximum divisor for BR).
    SPI1 -> CR1 |= SPI_CR1_BR_0 | SPI_CR1_BR_1 | SPI_CR1_BR_2;

    //Configure the interface for a 10-bit word size.
    SPI1 -> CR2 |= SPI_CR2_DS;
    SPI1 -> CR2 &= ~(SPI_CR2_DS_1 | SPI_CR2_DS_2);


    // Configure the SPI channel to be in "master configuration".
    SPI1 -> CR1 |= SPI_CR1_MSTR;
    // Set the SS Output enable bit and enable NSSP.
    SPI1 -> CR2 |= SPI_CR2_NSSP | SPI_CR2_SSOE;

    // Set the TXDMAEN bit to enable DMA transfers on transmit buffer empty
    SPI1 -> CR2 |= SPI_CR2_TXDMAEN;

    // Enable the SPI channel.
    SPI1 -> CR1 |= SPI_CR1_SPE;
}


void spi_cmd(unsigned int data) {
    while((SPI1->SR & SPI_SR_TXE) == 0)
        ; // wait for the transmit buffer to be empty
    SPI1->DR = data;

}
void spi_data(unsigned int data) {
    spi_cmd(data | 0x200);
    
}
void spi1_init_oled() {
    // wait 1 ms using nano_wait
    nano_wait(1000000);
    // call spi_cmd with 0x38 to do a "function set"
    spi_cmd(0x38);
    // call spi_cmd with 0x08 to turn the display off
    spi_cmd(0x08);

    // call spi_cmd with 0x01 to clear the display
    spi_cmd(0x01);

    // wait 2 ms using nano_wait
    nano_wait(2000000);
    // call spi_cmd with 0x06 to set the entry mode
    spi_cmd(0x06);

    // call spi_cmd with 0x02 to move the cursor to the home position
    spi_cmd(0x02);

    // call spi_cmd with 0x0c to turn the display on
    spi_cmd(0x0c);

    
}
void spi1_display1(const char *string) {
    spi_cmd(0x02);
    while (*string != '\0') {
        spi_data(*string);
        string+= 1;

    }
}

//===========================================================================
// This is the 34-entry buffer to be copied into SPI1.
// Each element is a 16-bit value that is either character data or a command.
// Element 0 is the command to set the cursor to the first position of line 1.
// The next 16 elements are 16 characters.
// Element 17 is the command to set the cursor to the first position of line 2.
//===========================================================================
uint16_t display[34] = {
        0x002, // Command to set the cursor at the first position line 1
        0x200+'E', 0x200+'C', 0x200+'E', 0x200+'3', 0x200+'6', + 0x200+'2', 0x200+' ', 0x200+'i',
        0x200+'s', 0x200+' ', 0x200+'t', 0x200+'h', + 0x200+'e', 0x200+' ', 0x200+' ', 0x200+' ',
        0x0c0, // Command to set the cursor at the first position line 2
        0x200+'c', 0x200+'l', 0x200+'a', 0x200+'s', 0x200+'s', + 0x200+' ', 0x200+'f', 0x200+'o',
        0x200+'r', 0x200+' ', 0x200+'y', 0x200+'o', + 0x200+'u', 0x200+'!', 0x200+' ', 0x200+' ',
};

//===========================================================================
// Configure the proper DMA channel to be triggered by SPI1_TX.
// Set the SPI1 peripheral to trigger a DMA when the transmitter is empty.
//===========================================================================
void spi1_setup_dma(void) {
    //enable clock to dma controller
    RCC -> AHBENR |= RCC_AHBENR_DMA1EN;

    //turn off enable bit
    DMA1_Channel3 -> CCR &= ~DMA_CCR_EN;

    //set cmar to address of msg array
    DMA1_Channel3 -> CMAR = (uint32_t)&display;

    //set cpar to addess of odr array
    DMA1_Channel3 -> CPAR = (uint32_t)&(SPI1 -> DR);

    //Set CNDTR to 8. (the amount of LEDs.) 
    DMA1_Channel3 -> CNDTR = 34;

    //Set the DIRection for copying from-memory-to-peripheral.
    DMA1_Channel3 -> CCR |= DMA_CCR_DIR;
        
    //Set the MINC to increment the CMAR for every transfer. (Each LED will have something different on it.)
    DMA1_Channel3 -> CCR |= DMA_CCR_MINC;
    DMA1_Channel3 -> CCR &= ~DMA_CCR_PINC;

    //Set the Memory datum SIZE to 16-bit.
    DMA1_Channel3 -> CCR |= DMA_CCR_MSIZE_0;
    DMA1_Channel3 -> CCR &= ~DMA_CCR_MSIZE_1;

    //Set the Peripheral datum SIZE to 16-bit.
    DMA1_Channel3 -> CCR |= DMA_CCR_PSIZE_0;
    DMA1_Channel3 -> CCR &= ~DMA_CCR_PSIZE_1;

    //Set the channel for CIRCular operation. 
    DMA1_Channel3 -> CCR |= DMA_CCR_CIRC;
}

//===========================================================================
// Enable the DMA channel triggered by SPI1_TX.
//===========================================================================
void spi1_enable_dma(void) {
    DMA1_Channel3 -> CCR |= DMA_CCR_EN;

}

