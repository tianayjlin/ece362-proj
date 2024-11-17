#include <stdio.h>
#include "stm32f0xx.h"

//insert function definitions here 
void init_spi(){
    RCC->AHBENR |= RCC_AHBENR_GPIOBEN;//enable clock for gpiob
    GPIOB -> MODER |= GPIO_MODER_MODER12_1 | GPIO_MODER_MODER13_1|GPIO_MODER_MODER15_1;//setting 12,13,15 for alt fxn
    GPIOB -> MODER &= ~(GPIO_MODER_MODER12_0 | GPIO_MODER_MODER13_0|GPIO_MODER_MODER15_0);//setting 12,13,15 for alt fxn
    GPIOB -> AFR[1] &= ~(0xF0FF0000); //setting af0 for pb12,13,15

    RCC -> APB1ENR |= RCC_APB1ENR_SPI2EN; //enabling clock to spi2
    SPI2 -> CR1 &= ~(SPI_CR1_SPE); //clearing cr1_spe
    SPI2 -> CR1 |= SPI_CR1_BR;//setting BR as low as possible 
    SPI2 -> CR1 &= ~SPI_CR1_SSI; //FORCING nss TO 0 AND IGNORING NSS
    SPI2 -> CR2 = 1010; //setting word size to 11
    SPI2 -> CR1 &= ~SPI_CR1_MSTR; //setting to SLAVE
    // SPI2 ->CR2 |= SPI_CR2_SSOE;//setting ss output enable bit
    // SPI2 -> CR2 |= SPI_CR2_NSSP; //turning nssp on
    SPI2 -> CR2 |= SPI_CR2_TXDMAEN; //enable dma
    SPI2 -> CR1 |= SPI_CR1_SPE; //enable spi
}