#include "leds.h"

void LED1_Init(void) {
    // configure PortPin for LED1 as port I/O 
    LED1_PORT->SEL0 &= ~(1 << LED1_PIN);
    LED1_PORT->SEL1 &= ~(1 << LED1_PIN);

    // make built-in LED1 LED high drive strength
    LED1_PORT->DS |= (1 << LED1_PIN);

    // make built-in LED1 out     
    LED1_PORT->DIR |= (1 << LED1_PIN);
    
    // turn off LED
    setLedLow(LED1_PORT, LED1_PIN);
}

void LED2_Init(void) {
    // configure PortPin for LED1 as port I/O 
    LED2_RED_PORT->SEL0 &= ~(1 << LED2_RED_PIN);
    LED2_RED_PORT->SEL1 &= ~(1 << LED2_RED_PIN);
    LED2_BLUE_PORT->SEL0 &= ~(1 << LED2_BLUE_PIN);
    LED2_BLUE_PORT->SEL1 &= ~(1 << LED2_BLUE_PIN);
    LED2_GREEN_PORT->SEL0 &= ~(1 << LED2_GREEN_PIN);
    LED2_GREEN_PORT->SEL1 &= ~(1 << LED2_GREEN_PIN);

    // make built-in LED1 LED high drive strength
    LED2_RED_PORT->DS |= (1 << LED2_RED_PIN);
    LED2_GREEN_PORT->DS |= (1 << LED2_GREEN_PIN);
    LED2_BLUE_PORT->DS |= (1 << LED2_BLUE_PIN);

    // make built-in LED1 out     
    LED2_RED_PORT->DIR |= (1 << LED2_RED_PIN);
    LED2_GREEN_PORT->DIR |= (1 << LED2_GREEN_PIN);
    LED2_BLUE_PORT->DIR |= (1 << LED2_BLUE_PIN);
    
    // turn off LED
    setLedLow(LED2_RED_PORT, LED2_RED_PIN);
    setLedLow(LED2_GREEN_PORT, LED2_GREEN_PIN);
    setLedLow(LED2_BLUE_PORT, LED2_BLUE_PIN);
}