/*
* Rochester Institute of Technology
* Department of Computer Engineering
* CMPE 460  Interfacing Digital Electronics
* LJBeato
* 1/14/2021
*
* Filename: main_timer_template.c
*/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "msp.h"
#include "Timer32.h"
#include "CortexM.h"
#include "Common.h"
#include "Motors.h"
#include "ServoMotor.h"
#include "ControlPins.h"
#include "ADC14.h"
#include "pid.h"
#include "switches.h"
#include "drivingAlgos.h"

extern BOOLEAN g_sendData;

void INIT_Camera(void) {
    g_sendData = FALSE;
    ControlPin_SI_Init();
    ControlPin_CLK_Init();
    ADC0_InitSWTriggerCh6();
}

//
// main
//
int main(void) {
    servoInit();
    motor1Init();
    motor2Init();
    INIT_Camera();
    
    // wait until a button is pressed, and run the corresponding program
    while(1) {
        if (Switch1_Pressed()) {
            fastest();
        }
    }
}
