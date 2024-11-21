#include "stm32f0xx.h"

#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "interrupts.h"
#include "timer.h"
#include "ps2.h"
#include "ff.h"
#include "diskio.h" 
#include "fifo.h"
#include "lcd.h"
#include "diskio.h"
#include "sdcard.h"
#include "gamelogic.h"

#define GAME_LENGTH 15

//insert function definitions here

int volatile GAMETIME = 16; // needs to be set to 15 when the game starts, takes one second to initialize
char buf[34];
int correct; //TODO: FIX THIS

void TIM7_IRQHandler(){

  TIM7->SR &= ~TIM_SR_UIF;
  GAMETIME -= 1;
  write_display(correct, GAME_LENGTH); //TODO: FIX THIS
  spi2_display1(buf);

}

/**
 * @param total_chars: total number of characters typed correctly 
 * @param s: length of typing test, in seconds
 */
void write_display(int total_chars, int s) { //

  if(GAMETIME > 0){
    snprintf(buf, sizeof('a'), "%d ", GAMETIME);
    return;
  }

  else
  {
    //finish timer
    // snprintf(buf, sizeof("a"), "%d", 0);

    memset(buf, ' ', 34);

    //display high score
    int hs = get_high_score();
    int wpm = wpm_calculation(total_chars, s);

    if(hs < wpm){
        update_high_score(wpm);
    }
    
    hs = wpm;
    
    int hs0 = hs % 10;
    int hs1 = (hs / 10) % 10;
    int hs2 = hs / 100;

    nano_wait(100000000);
    snprintf(buf, sizeof(buf), "high score: %d%d%d", hs2, hs1, hs0);

    end_screen(total_chars, s);

    return;
  }
}



void init_spi2() {
    RCC -> AHBENR |= RCC_AHBENR_GPIOBEN;
    //Configure NSS, SCK and MOSI signals of SPI2 to pins PA15, PA5 and PA7, respectively.
    GPIOB -> MODER &= ~(GPIO_MODER_MODER12_0 | GPIO_MODER_MODER13_0| GPIO_MODER_MODER15_0);
    GPIOB -> MODER |= (GPIO_MODER_MODER12_1 | GPIO_MODER_MODER13_1 | GPIO_MODER_MODER15_1);
    GPIOB -> AFR[1] &= ~(GPIO_AFRH_AFRH4 | GPIO_AFRH_AFRH5 | GPIO_AFRH_AFRH7);
    RCC -> APB1ENR |= RCC_APB1ENR_SPI2EN;

    //Ensure that the CR1_SPE bit is clear first. 
    SPI2 -> CR1 &= ~SPI_CR1_SPE;
    //Set the baud rate as low as possible (maximum divisor for BR).
    SPI2 -> CR1 |= SPI_CR1_BR_0 | SPI_CR1_BR_1 | SPI_CR1_BR_2;

    //Configure the interface for a 10-bit word size.
    SPI2 -> CR2 |= SPI_CR2_DS;
    SPI2 -> CR2 &= ~(SPI_CR2_DS_1 | SPI_CR2_DS_2);


    // Configure the SPI channel to be in "master configuration".
    SPI2 -> CR1 |= SPI_CR1_MSTR;
    // Set the SS Output enable bit and enable NSSP.
    SPI2 -> CR2 |= SPI_CR2_NSSP | SPI_CR2_SSOE;

    // Set the TXDMAEN bit to enable DMA transfers on transmit buffer empty
    SPI2 -> CR2 |= SPI_CR2_TXDMAEN;

    // Enable the SPI channel.
    SPI2 -> CR1 |= SPI_CR1_SPE;
}


void spi_cmd(unsigned int data) {
    while((SPI2->SR & SPI_SR_TXE) == 0)
        ; // wait for the transmit buffer to be empty
    SPI2->DR = data;

}
void spi_data(unsigned int data) {
    spi_cmd(data | 0x200);
    
}
void spi2_init_oled() {
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
void spi2_display1(const char *string) {
    spi_cmd(0x02);
    while (*string != '\0') {
        spi_data(*string);
        string+= 1;

    }
}

//===========================================================================
// This is the 34-entry buffer to be copied into SPI2.
// Each element is a 16-bit value that is either character data or a command.
// Element 0 is the command to set the cursor to the first position of line 1.
// The next 16 elements are 16 characters.
// Element 17 is the command to set the cursor to the first position of line 2.
//===========================================================================
// uint16_t display[34] = {
//         0x002, // Command to set the cursor at the first position line 1
//         0x200+'E', 0x200+'C', 0x200+'E', 0x200+'3', 0x200+'6', + 0x200+'2', 0x200+' ', 0x200+'i',
//         0x200+'s', 0x200+' ', 0x200+'t', 0x200+'h', + 0x200+'e', 0x200+' ', 0x200+' ', 0x200+' ',
//         0x0c0, // Command to set the cursor at the first position line 2
//         0x200+'c', 0x200+'l', 0x200+'a', 0x200+'s', 0x200+'s', + 0x200+' ', 0x200+'f', 0x200+'o',
//         0x200+'r', 0x200+' ', 0x200+'y', 0x200+'o', + 0x200+'u', 0x200+'!', 0x200+' ', 0x200+' ',
// };

//===========================================================================
// Configure the proper DMA channel to be triggered by SPI2_TX.
// Set the SPI2 peripheral to trigger a DMA when the transmitter is empty.
//===========================================================================
void spi2_setup_dma(void) {
    //enable clock to dma controller
    RCC -> AHBENR |= RCC_AHBENR_DMA1EN;

    //turn off enable bit
    DMA1_Channel5 -> CCR &= ~DMA_CCR_EN;

    //set cmar to address of msg array
    DMA1_Channel5 -> CMAR = (uint32_t)&buf;//display;

    //set cpar to addess of odr array
    DMA1_Channel5 -> CPAR = (uint32_t)&(SPI2 -> DR);

    //Set CNDTR to 8. (the amount of LEDs.) 
    DMA1_Channel5 -> CNDTR = 34;

    //Set the DIRection for copying from-memory-to-peripheral.
    DMA1_Channel5 -> CCR |= DMA_CCR_DIR;
        
    //Set the MINC to increment the CMAR for every transfer. (Each LED will have something different on it.)
    DMA1_Channel5 -> CCR |= DMA_CCR_MINC;
    DMA1_Channel5 -> CCR &= ~DMA_CCR_PINC;

    //Set the Memory datum SIZE to 16-bit.
    DMA1_Channel5 -> CCR |= DMA_CCR_MSIZE_0;
    DMA1_Channel5 -> CCR &= ~DMA_CCR_MSIZE_1;

    //Set the Peripheral datum SIZE to 16-bit.
    DMA1_Channel5 -> CCR |= DMA_CCR_PSIZE_0;
    DMA1_Channel5 -> CCR &= ~DMA_CCR_PSIZE_1;

    //Set the channel for CIRCular operation. 
    DMA1_Channel5 -> CCR |= DMA_CCR_CIRC;
}

//===========================================================================
// Enable the DMA channel triggered by SPI2_TX.
//===========================================================================
void spi2_enable_dma(void) {
    DMA1_Channel5 -> CCR |= DMA_CCR_EN;

}

