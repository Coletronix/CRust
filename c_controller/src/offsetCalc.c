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

const int threshVal = 8000;

// is there any data available from the camera, and if so, reset the flag
BOOLEAN getCameraDataAvailable() {
    if (g_sendData == TRUE) {
        g_sendData = FALSE;
        return TRUE;
    }
    return FALSE;
}

// get the center offset of the track as a value from -1 to 1
float getTrackCenterOffset(BOOLEAN* noTrack) {
    *noTrack = FALSE;

    // find first and last maximum value in line array
    int maxFirstIndex = 0;
    int maxLastIndex = 127;
    int i;
    for (i = 0; i < 128; i++) { // first
        if (line[i] >= threshVal) {
            maxFirstIndex = i;
            break;
        }
    }
    if (i == 128) {
        *noTrack = TRUE;
        return 0;
    }
    for (i = 127; i >= 0; i--) { // last
        if (line[i] >= threshVal) {
            maxLastIndex = i;
            break;
        }
    }
    
    // do I really need to explain this?
    int middleIndex = (maxFirstIndex + maxLastIndex) / 2;
    float centerOffset = (middleIndex - 64)/64.0;
    return centerOffset;
}