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

extern uint32_t SystemCoreClock;
extern uint16_t line[128];
extern BOOLEAN g_sendData;

// 1 second
#define MAX_FRAMES_OFF_TRACK (133)
// from ControlPins.c
#define UPDATE_DT (0.0075) 

volatile uint32_t MillisecondCoutner = 0;

BOOLEAN getCameraDataAvailable() {
    if (g_sendData == TRUE) {
        g_sendData = FALSE;
        return TRUE;
    }
    return FALSE;
}

float getTrackCenterOffset(BOOLEAN* noTrack) {
    noTrack = FALSE;
    // find first and last maximum value in line array
    int maxFirstIndex = 0;
    int maxLastIndex = 127;
    const int maxValue = 16383;
    int i;
    for (i = 0; i < 128; i++) {
        if (line[i] == maxValue) {
            maxFirstIndex = i;
            break;
        }
    }
    if (i == 128) {
        *noTrack = TRUE;
        return 0;
    }
    for (i = 127; i >= 0; i--) {
        if (line[i] == maxValue) {
            maxLastIndex = i;
            break;
        }
    }
    int middleIndex = (maxFirstIndex + maxLastIndex) / 2;
    float centerOffset = (middleIndex - 64)/64.0;
    return centerOffset;
}

void Timer32_2_ISR_1(void) {
    MillisecondCoutner++;
}

void slow() {
    Timer32_2_Init(&Timer32_2_ISR_1, CalcPeriodFromFrequency(1000), T32DIV1); // initialize Timer A32-1;

    int numFramesOffTrack = 0;
    BOOLEAN running = TRUE;
    
    float robotSpeed = .3;
    

    // pid struct
    // PID pid = {57.0, 0.1, 0.0, 0, 0};
    PID pid = {47, 0, 0, 0, 0};
    
    setMotor1Power(robotSpeed);
    setMotor2Power(robotSpeed);
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
            float correction = PIDUpdate(&pid, centerOffset);
            // apply steering
            setServoAngle(correction);
        }
    }
}