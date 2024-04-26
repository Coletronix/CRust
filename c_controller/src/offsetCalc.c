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
#include <math.h>

extern uint32_t SystemCoreClock;
extern uint16_t line[128];
extern BOOLEAN g_sendData;

BOOLEAN getCameraDataAvailable() {
    if (g_sendData == TRUE) {
        g_sendData = FALSE;
        return TRUE;
    }
    return FALSE;
}

float getTrackCenterOffset(BOOLEAN* noTrack) {
    *noTrack = FALSE;
    // find first and last maximum value in line array
    int maxFirstIndex = 0;
    int maxLastIndex = 127;
    // const int threshVal = 16383;
    // const int threshVal = 3000;
    // const int threshVal = 8000;
    const int threshVal = 5500;
    int i;
    for (i = 0; i < 128; i++) {
        if (line[i] >= threshVal) {
            maxFirstIndex = i;
            break;
        }
    }
    if (i == 128) {
        *noTrack = TRUE;
        return 0;
    }
    for (i = 127; i >= 0; i--) {
        if (line[i] >= threshVal) {
            maxLastIndex = i;
            break;
        }
    }
    int middleIndex = (maxFirstIndex + maxLastIndex) / 2;
    float centerOffset = (middleIndex - 64)/64.0;
    return centerOffset;
}