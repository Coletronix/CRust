#include <stdio.h>
#include "msp.h"
#include "Timer32.h"
#include "CortexM.h"
#include "Common.h"
#include "Motors.h"
#include "ServoMotor.h"
#include "ControlPins.h"
#include "ADC14.h"
#include "pid.h"
#include "offsetCalc.h"
#include <math.h>

extern uint32_t SystemCoreClock;
extern uint16_t line[128];
extern BOOLEAN g_sendData;

volatile uint32_t MillisecondCounter3 = 0;

// from ControlPins.c
#define UPDATE_DT (1.0/50.0) 

// Interrupt to keep track of milliseconds that have passed
void Timer32_2_ISR_3(void) {
    MillisecondCounter3++;
}

// track following code
void fastest() {
    // configure the interrupt to run at 1000 Hz
    Timer32_2_Init(&Timer32_2_ISR_3, CalcPeriodFromFrequency(1000), T32DIV1); // initialize Timer A32-1;

    // what percentage of the applied correction should be applied as differential turning
    const float diff = 0.006;
    
    // whether the program is running
    BOOLEAN running = TRUE;
    
    // self explainatory
    int numFramesOffTrack = 0;
    
    // last applied correction
    float lastCorrection = 0;
    
    // How many frames we've been applying a "too low" motor power
    int numFramesUnderThreshold = 0;
    
    // experimentally determined PID values
    PID straightPID = {50.0, 0.0, 10000.0, 0, 0, 10.0};

    while (running) {
        // only do work if new camera data is available
        if (getCameraDataAvailable()) {
            BOOLEAN noTrack;
            float centerOffset = getTrackCenterOffset(&noTrack);
            if (noTrack) {
                numFramesOffTrack++;
            } else {
                numFramesOffTrack = 0;
            }
            // if we've been off track for a while, stop
            if (numFramesOffTrack > 150) {
                running = FALSE;
            }
            
            // apply PID
            float correction = PIDUpdate(&straightPID, centerOffset, UPDATE_DT);
            
            // apply speed curve
            float speed = fmin(.4, .45 + fabs(centerOffset) * (-.15-.45));
             
            if (fabs(speed) < 0.2) {
                numFramesUnderThreshold++;
            } else {
                numFramesUnderThreshold = 0;
            }
            
            // if we've been applying power less than the minimum required to turn the motors
            // for a while, we're probably stuck, so give it a kick
            if (numFramesUnderThreshold > 30) {
                speed = 0.25;
            }
            
            // differential turning
            float turnAmt = diff * correction;
            setMotor1Power(speed + turnAmt);
            setMotor2Power(speed - turnAmt);
            
            // If we're fully off the track while driving, keep turning in the same direction
            if (noTrack) {
                setServoAngle(-lastCorrection);
            } else {
                setServoAngle(-correction);
                lastCorrection = correction;
            }
        }
    }
    
    setMotor1Power(0);
    setMotor2Power(0);
}