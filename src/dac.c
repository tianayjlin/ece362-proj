#include "dac.h"
#include "stm32f0xx.h"
#include <math.h>

#define N 1000
#define RATE 20000
short int wavetable[N];
int step0 = (1000 * N / RA
TE) * (1<<16);
int offset0 = 0;

//============================
//plays a beep noise, called when wrong key is pressed
void wrong_keypress_sound(void) {
    init_wavetable(); //main
    setup_dac(); //main
    init_tim6();
    
    //DAC->DHR12R1 = 0;

    //waits for .1 seconds
    nano_wait(100000000);

    //turns off timer 6 enable to stop sine wave
    NVIC -> ISER[0] &= ~(1 << TIM6_IRQn);
    TIM6 -> CR1 &= ~(TIM_CR1_CEN);
    //DAC->DHR12R1 = 0;
}


//===========================================================================
// init_wavetable()
// Creates the pattern for a complete cycle of a sine wave into the
// wavetable[] array.
//===========================================================================
void init_wavetable(void) {
    for(int i=0; i < N; i++)
        wavetable[i] = 32767 * sin(2 * M_PI * i / N);
}

//============================================================================
// setup_dac()
// Initial DAC setup stuff
//============================================================================
void setup_dac(void) {
  // Enable the clock to GPIO Port A.
  RCC -> AHBENR |= RCC_AHBENR_GPIOAEN;

// Set the configuration for analog operation only for the pin that is associated with DAC_OUT1.
  GPIOA -> MODER |= GPIO_MODER_MODER4_1 | GPIO_MODER_MODER4_0;
// Enable the RCC clock for the DAC.
  RCC -> APB1ENR |= RCC_APB1ENR_DACEN;
// Select a TIM6 TRGO trigger for the DAC with the TSEL field of the CR register.
  DAC -> CR &= ~(DAC_CR_TSEL1);

// Enable the trigger for the DAC.
  DAC -> CR |= DAC_CR_TEN1;

// Enable the DAC.
  DAC -> CR |= DAC_CR_EN1;

}

//============================================================================
// Timer 6 ISR
// Moves through sine wave at rate of timer 6.
//============================================================================
// Write the Timer 6 ISR here.  Be sure to give it the right name.
void TIM6_DAC_IRQHandler() {
  //acknowledge interrupt
  TIM6 -> SR &= ~TIM_SR_UIF;
// increment offset0 by step0
  offset0 += step0;

// if offset0 is >= (N << 16) decrement offset0 by (N << 16)
  if (offset0 >= (N << 16)) {
    offset0 -= (N << 16);
  }

// int samp = sum of wavetable[offset0>>16] and wavetable[offset1>>16]
  int samp = wavetable[offset0>>16];
// multiply samp by volume
  samp *= 2000; //0-4095

// shift samp right by 17 bits to ensure it's in the right format for `DAC_DHR12R1` 
  samp = samp >> 17;

// increment samp by 2048
  samp += 2048;

// copy samp to DAC->DHR12R1
  DAC->DHR12R1 = samp;
}

//============================================================================
// Timer 6 Setup
// Moves through sine wave at rate of timer 6.
//============================================================================
void init_tim6(void) {
  RCC -> APB1ENR |= RCC_APB1ENR_TIM6EN;

  TIM6 -> PSC = 2 - 1;
  TIM6 -> ARR = 48000000/(2*RATE) - 1;

  TIM6 -> DIER |= TIM_DIER_UIE;
  NVIC -> ISER[0] |= 1 << TIM6_IRQn;
  TIM6 -> CR1 |= TIM_CR1_CEN;


  TIM6 -> CR2 |= TIM_CR2_MMS_1;
  TIM6 -> CR2 &= ~(TIM_CR2_MMS_0 | TIM_CR2_MMS_2);

}