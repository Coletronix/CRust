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

void Timer32_2_ISR_3(void) {
    MillisecondCounter3++;
}

void fastest() {
    Timer32_2_Init(&Timer32_2_ISR_3, CalcPeriodFromFrequency(1000), T32DIV1); // initialize Timer A32-1;

    const float diff = 0.006;
    
    float motor1Power = 0.0;
    float motor2Power = 0.0;

    BOOLEAN running = TRUE;
    int numFramesOffTrack = 0;
    float fastSpeed = .44;
    float lastCorrection = 0;
    
    int straightCounter = 0;
    
    int numFramesUnderThreshold = 0;
    
    PID straightPID = {50.0, 0.0, 10000.0, 0, 0, 10.0};

    motor1Power = fastSpeed;
    motor2Power = fastSpeed;
    while (running) {
        if (getCameraDataAvailable()) {
            BOOLEAN noTrack;
            float centerOffset = getTrackCenterOffset(&noTrack);
            if (noTrack) {
                numFramesOffTrack++;
            } else {
                numFramesOffTrack = 0;
            }
            if (numFramesOffTrack > 150) {
                running = FALSE;
            }
            
            float correction = PIDUpdate(&straightPID, centerOffset, UPDATE_DT);
            
            float speed = .45 + fabs(centerOffset) * (-.15-.45);
            
            speed = fmin(.4, speed);
             
            if (fabs(speed) < 0.2) {
                numFramesUnderThreshold++;
            } else {
                numFramesUnderThreshold = 0;
            }
            
            if (numFramesUnderThreshold > 30) {
                speed = 0.25;
            }
            
            float turnAmt = diff * correction;
            motor1Power = speed + turnAmt;
            motor2Power = speed - turnAmt;

            straightCounter++;
            
            if (noTrack) {
                setServoAngle(-lastCorrection);
            } else {
                setServoAngle(-correction);
                lastCorrection = correction;
            }
            setMotor1Power(motor1Power);
            setMotor2Power(motor2Power);
        }
    }
    
    setMotor1Power(0);
    setMotor2Power(0);
}