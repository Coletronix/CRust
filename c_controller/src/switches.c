#include "switches.h"

void Switches_Init(void) {
    // configure PortPin for Switch 1 and Switch2 as port I/O 
    uint8_t mask = (1 << SWITCH1_PIN) | (1 << SWITCH2_PIN);
    SWITCH_PORT->SEL0 &= ~mask;
    SWITCH_PORT->SEL1 &= ~mask;

    // configure as input
    SWITCH_PORT->DIR &= ~mask;
    
    // pullup resistor
    SWITCH_PORT->OUT |= mask;
    SWITCH_PORT->REN |= mask;
}

void Switch1_Init(void) {
    // configure PortPin for Switch 1 as port I/O 
    SWITCH_PORT->SEL0 &= ~(1 << SWITCH1_PIN);
    SWITCH_PORT->SEL1 &= ~(1 << SWITCH1_PIN);

    // configure as input
    SWITCH_PORT->DIR &= ~(1 << SWITCH1_PIN);
    
    // pullup resistor
    SWITCH_PORT->OUT |= (1 << SWITCH1_PIN);
    SWITCH_PORT->REN |= (1 << SWITCH1_PIN);
}

void Switch2_Init(void) {
    // configure PortPin for Switch 2 as port I/O 
    SWITCH_PORT->SEL0 &= ~(1 << SWITCH2_PIN);
    SWITCH_PORT->SEL1 &= ~(1 << SWITCH2_PIN);

    // configure as input
    SWITCH_PORT->DIR &= ~(1 << SWITCH2_PIN);
    
    // pullup resistor
    SWITCH_PORT->OUT |= (1 << SWITCH2_PIN);
    SWITCH_PORT->REN |= (1 << SWITCH2_PIN);
}

//------------Switch_Input------------
// Read and return the status of Switch1
// Input: none
// return: TRUE if pressed
//         FALSE if not pressed
BOOLEAN Switch1_Pressed(void) {
    // check if pressed
    return !(SWITCH_PORT->IN & (1 << SWITCH1_PIN));
}
//------------Switch_Input------------
// Read and return the status of Switch2
// Input: none
// return: TRUE if pressed
//         FALSE if not pressed
BOOLEAN Switch2_Pressed(void) {
    // check if pressed
    return !(SWITCH_PORT->IN & (1 << SWITCH2_PIN));
}