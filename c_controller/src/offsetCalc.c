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

const int minValOnTrack = 2200;
const float threshPercentage = 0.75;

// const int brightnessCorrection[128] = {1413, 2055, 2038, 2075, 2129, 2210, 2224, 2332, 2337, 2451, 2484, 2630, 2759, 2864, 3021, 3365, 3606, 3952, 4312, 4843, 5274, 5930, 6471, 7145, 7570, 8337, 8819, 9371, 9830, 10694, 11127, 11472, 12097, 12413, 11550, 12919, 13166, 13553, 13420, 13622, 13252, 12374, 13585, 13316, 11127, 13318, 13167, 13098, 12300, 12307, 12092, 11811, 11644, 11379, 11217, 10976, 10719, 10707, 10394, 10352, 10392, 10446, 10460, 10507, 9973, 10547, 10478, 10542, 10299, 10475, 10494, 10327, 10032, 10032, 9828, 9661, 9382, 8981, 8714, 8368, 8028, 7473, 7246, 6661, 6467, 5958, 5607, 5423, 5099, 4828, 4547, 4322, 4192, 3939, 3785, 3614, 3497, 3307, 3192, 3093, 2970, 2869, 2780, 2751, 2638, 2600, 2491, 2489, 2435, 2416, 2335, 2272, 2172, 2244, 2196, 2164, 2115, 2091, 2048, 2073, 2021, 1990, 1978, 1956, 1939, 1964, 1912, 2734};

BOOLEAN getCameraDataAvailable() {
    if (g_sendData == TRUE) {
        g_sendData = FALSE;
        return TRUE;
    }
    return FALSE;
}

float getTrackCenterOffset(BOOLEAN* noTrack) {
    // find maximum value
    int maxVal = 0;
    for (int i = 0; i < 128; i++) {
        if (line[i] > maxVal) {
            maxVal = line[i];
        }
    }
    
    // if (maxVal < minValOnTrack) {
    //     *noTrack = TRUE;
    //     return 0;
    // }
    
    // int threshVal = (int)((float)maxVal * threshPercentage);
    
    *noTrack = FALSE;
    // find first and last maximum value in line array
    int maxFirstIndex = 0;
    int maxLastIndex = 127;
    // const int threshVal = 16383;
    // const int threshVal = 3000;
    const int threshVal = 8000;
    // const int threshVal = 12000;
    // const int threshVal = 5500;
    // const int threshVal = 5000;
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