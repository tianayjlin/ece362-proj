#ifndef PS2_H
#define PS2_H

//INSERT FUNCTION DECLARATIONS HERE 
void setup_tim1();
void keyChar(uint16_t msg);
void TIM1_CC_IRQHandler();
#endif