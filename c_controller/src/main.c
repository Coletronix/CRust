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

// setup everything needed to run the camera
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
    // initialize everything the system use
    Switch1_Init();
    servoInit();
    motor1Init();
    motor2Init();
    INIT_Camera();
    
    while(1) {
        // wait until a button is pressed, and run the program
        if (Switch1_Pressed()) {
            fastest(); // only mode :-)
        }
    }
}
