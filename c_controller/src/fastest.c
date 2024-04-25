#include <stdio.h>
#include "msp.h"
#include "uart.h"
#include "switches.h"
#include "leds.h"
#include "Timer32.h"
#include "CortexM.h"
#include "Common.h"
#include "Motors.h"
#include "ServoMotor.h"
#include "ControlPins.h"
#include "ADC14.h"
#include "pid.h"
#include "oled.h"
#include "offsetCalc.h"
#include <math.h>

extern uint32_t SystemCoreClock;
extern uint16_t line[128];
extern BOOLEAN g_sendData;
extern unsigned char OLED_clr_data[1024];
extern unsigned char OLED_TEXT_ARR[1024];
extern unsigned char OLED_GRAPH_ARR[1024];

// 1 second
#define MAX_FRAMES_OFF_TRACK (50)
// from ControlPins.c
#define UPDATE_DT (1.0/50.0) 

volatile uint32_t MillisecondCounter3 = 0;

void Timer32_2_ISR_3(void) {
    MillisecondCounter3++;
}

void fastest() {
    Timer32_2_Init(&Timer32_2_ISR_3, CalcPeriodFromFrequency(1000), T32DIV1); // initialize Timer A32-1;

    const float diff = 0.008;
    
    float motor1Power = 0.0;
    float motor2Power = 0.0;

    BOOLEAN running = TRUE;
    int numFramesOffTrack = 0;
    float fastSpeed = .6;
    float lastCorrection = 0;
    
    int straightCounter = 0;
    
    PID frontPID = {54.0, 0.0, 10000.0, 0, 0, 10.0};

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
            if (numFramesOffTrack > MAX_FRAMES_OFF_TRACK) {
                running = FALSE;
            }
            
            setLedLow(LED2_BLUE_PORT, LED2_BLUE_PIN);
            
            float correction = PIDUpdate(&frontPID, centerOffset, UPDATE_DT);
            
            // float speed = .45 + fabs(correction) * (-.3-.45)/64.0;
            float speed = fastSpeed + fabs(centerOffset) * (-.5-fastSpeed);
            // float speed = fmin(fabs(.5*correction), .45);
            
            motor1Power = speed + diff * correction;
            motor2Power = speed - diff * correction;

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