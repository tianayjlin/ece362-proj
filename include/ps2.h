#ifndef PS2_H
#define PS2_H
#include <stdio.h>

#include "stm32f0xx.h"
#include "ff.h"
#include "diskio.h"
#include "sdcard.h"
#include "timer.h"
#include "ps2.h"
#include "interrupts.h"
// #include "gamelogic.h"

#define BUFFER_SIZE 2048

//INSERT FUNCTION DECLARATIONS HERE 
void init_keyboard();
// void init_ps2();
void init_exti ();
void EXTI0_1_IRQHandler();
void adjust_priorities();
uint16_t bit_bang_data();
void dummyTest(char key);
uint16_t reverseEnd(uint16_t num);
#endif