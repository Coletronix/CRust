#ifndef LEDS_H
#define LEDS_H

#include "msp.h" 
#include "Common.h"

#define LED1_PORT P1
#define LED1_PIN 0

#define LED2_RED_PORT P2
#define LED2_RED_PIN 0
#define LED2_GREEN_PORT P2
#define LED2_GREEN_PIN 1
#define LED2_BLUE_PORT P2
#define LED2_BLUE_PIN 2

// color defines
#define RED 0x01
#define GREEN 0x02
#define BLUE 0x04
#define CYAN 0x06
#define MAGENTA 0x05
#define YELLOW 0x03
#define WHITE 0x07

#define getLedState(port, pin) ((port)->OUT & (1 << (pin)))
#define setLedHigh(port, pin) (port)->OUT |= (1 << (pin));
#define setLedValue(port, pin, value) do { (port) -> OUT &= ~(1 << (pin)); (port)->OUT |= (((value) != 0) << (pin)); } while (0);
#define setLedLow(port, pin) (port)->OUT &= ~(1 << (pin));
#define toggleLed(port, pin) (port)->OUT ^= (1 << (pin));

void LED1_Init(void);
void LED2_Init(void);

#endif