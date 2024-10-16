/**
  ******************************************************************************
  * @file    main.c
  * @author  Weili An, Niraj Menon
  * @date    Feb 7, 2024
  * @brief   ECE 362 Lab 7 student template
  ******************************************************************************
*/

/*******************************************************************************/

// Fill out your username!  Even though we're not using an autotest, 
// it should be a habit to fill out your username in this field now.
const char* username = "lin1542";

/*******************************************************************************/ 

#include "stm32f0xx.h"
#include <stdint.h>

void internal_clock();

// Uncomment only one of the following to test each step
// #define STEP1
// #define STEP2
// #define STEP3
#define STEP4

void init_usart5() {
    //^ you are configuring the MICROCONTROLLER to transmit and receive =/= ftdi 
    
    //enable rcc clocks for GPIOC and D 
    RCC -> AHBENR |= RCC_AHBENR_GPIOCEN | RCC_AHBENR_GPIODEN;

    //^ configure PC12 to be routed to USART5_TX aka alternate functions! 
    GPIOC -> MODER &= ~GPIO_MODER_MODER12; //clear entire pin
    GPIOC -> MODER |= GPIO_MODER_MODER12_1; 

    GPIOC -> AFR[1] &= ~(GPIO_AFRH_AFRH4);
    GPIOC -> AFR[1] |= 2 << GPIO_AFRH_AFRH4_Pos;//shift in bits to afrh (afr[2] by amount denoted by af#)
    

    //^ configure PD2 to be routed to USART5_Rx aka another function lmao 
    GPIOD -> MODER &= ~GPIO_MODER_MODER2; 
    GPIOD -> MODER |= GPIO_MODER_MODER2_1; 
    
    GPIOD -> AFR[0] &= ~(GPIO_AFRL_AFRL2); ///clear position 2 first, in afr[0] because it is pins 0:7
    GPIOD -> AFR[0] |= 2 << GPIO_AFRL_AFRL2_Pos; //af2 = 2;


    //^ UART config
    RCC -> APB1ENR |= RCC_APB1ENR_USART5EN; //turn on usart clock to make changes to register
    USART5 -> CR1 &= ~USART_CR1_UE; //disable peripheral to avoid unwanted output 

    //8 bit word size is 0b00, m1 and m0 are separate 
    //16x over sampling
    //disable parity control 0b0 
    USART5 -> CR1 &= ~(USART_CR1_M1 | USART_CR1_M0 | USART_CR1_OVER8 | USART_CR1_PCE);

    //stop bit 
    USART5 -> CR2 &= ~USART_CR2_STOP;

    //baud rate = 115200 baud 
    USART5 -> BRR  = 0x1A1; 

    //enable transmitter and receiver 
    USART5 -> CR1 |= USART_CR1_TE | USART_CR1_RE;
    
    //^ end uart config 
    //enable usart 
    USART5 -> CR1 |= USART_CR1_UE;

    //wait for teack and reack to turn on 
    while(!(USART5 -> ISR & USART_ISR_TEACK) | !(USART5 -> ISR & USART_ISR_REACK));
}

#ifdef STEP1
int main(void){
    internal_clock();
    init_usart5();
    for(;;) {
        while (!(USART5->ISR & USART_ISR_RXNE)) { }
        char c = USART5->RDR;
        while(!(USART5->ISR & USART_ISR_TXE)) { }
        USART5->TDR = c;
    }
}
#endif

#ifdef STEP2
#include <stdio.h>

// TODO Resolve the echo and carriage-return problem

int __io_putchar(int c) {
    // TODO

    if(c == '\n'){
        while(!(USART5->ISR & USART_ISR_TXE)); //always transmit before you write 
        USART5 -> TDR = '\r';  
    }

    while(!(USART5->ISR & USART_ISR_TXE));
    USART5->TDR = c;
    return c;
}

int __io_getchar(void) {

    while (!(USART5->ISR & USART_ISR_RXNE));
    char c = USART5->RDR;
    
    //TODO 
    if(c == '\r'){
        while (!(USART5->ISR & USART_ISR_RXNE));
        c = '\n';
    }

    __io_putchar(c);
    return c;
}

int main() {
    internal_clock();
    init_usart5();
    setbuf(stdin,0);
    setbuf(stdout,0);
    setbuf(stderr,0);
    printf("Enter your name: ");
    char name[80];
    fgets(name, 80, stdin);
    printf("Your name is %s", name);
    printf("Type any characters.\n");
    for(;;) {
        char c = getchar();
        putchar(c);
    }
}
#endif

#ifdef STEP3
#include <stdio.h>
#include "fifo.h"
#include "tty.h"
int __io_putchar(int c) {
    // TODO Copy from your STEP2

    if(c == '\n'){
        while(!(USART5->ISR & USART_ISR_TXE)); //always transmit before you write 
        USART5 -> TDR = '\r';  
    }

    while(!(USART5->ISR & USART_ISR_TXE));
    USART5->TDR = c;
}

int __io_getchar(void) {
    // TODO
    return line_buffer_getchar(); 
}

int main() {
    internal_clock();
    init_usart5();
    setbuf(stdin,0);
    setbuf(stdout,0);
    setbuf(stderr,0);
    printf("Enter your name: ");
    char name[80];
    fgets(name, 80, stdin);
    printf("Your name is %s", name);
    printf("Type any characters.\n");
    for(;;) {
        char c = getchar();
        putchar(c);
    }
}
#endif

#ifdef STEP4

#include <stdio.h>
#include "fifo.h"
#include "tty.h"

// TODO DMA data structures
#define FIFOSIZE 16
char serfifo[FIFOSIZE];
int seroffset = 0;

void enable_tty_interrupt(void) {
    // TODO

    //raise an interrupt when the data register becomes not empty 
}

// Works like line_buffer_getchar(), but does not check or clear ORE nor wait on new characters in USART
char interrupt_getchar() {
    // TODO
}

int __io_putchar(int c) {
    // TODO copy from STEP2
}

int __io_getchar(void) {
    // TODO Use interrupt_getchar() instead of line_buffer_getchar()
}

// TODO Copy the content for the USART5 ISR here
// TODO Remember to look up for the proper name of the ISR function

int main() {
    internal_clock();
    init_usart5();
    enable_tty_interrupt();

    setbuf(stdin,0); // These turn off buffering; more efficient, but makes it hard to explain why first 1023 characters not dispalyed
    setbuf(stdout,0);
    setbuf(stderr,0);
    printf("Enter your name: "); // Types name but shouldn't echo the characters; USE CTRL-J to finish
    char name[80];
    fgets(name, 80, stdin);
    printf("Your name is %s", name);
    printf("Type any characters.\n"); // After, will type TWO instead of ONE
    for(;;) {
        char c = getchar();
        putchar(c);
    }
}
#endif