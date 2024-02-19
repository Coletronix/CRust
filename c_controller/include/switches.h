#ifndef SWITCHES_H
#define SWITCHES_H


#include "msp.h" 
#include "Common.h"

#define SWITCH_PORT P1
#define SWITCH1_PIN 1
#define SWITCH2_PIN 4

void Switches_Init(void);
void Switch1_Init(void);
void Switch2_Init(void);

BOOLEAN Switch1_Pressed(void);
BOOLEAN Switch2_Pressed(void);
#endif