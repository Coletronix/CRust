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
#include "drivingAlgos.h"

extern BOOLEAN g_sendData;

void INIT_Camera(void) {
    g_sendData = FALSE;
    ControlPin_SI_Init();
    ControlPin_CLK_Init();
    ADC0_InitSWTriggerCh6();
}

// void faster() {
//     powerTarget = 0;
//     int numFramesOffTrack = 0;
//     const int maxFramesOffTrack = 10;
//     BOOLEAN running = FALSE;
//     // pid struct
//     // PID pid = {57.0, 0.1, 0.0, 0, 0};
//     PID pid = {100, 0.0, 100.0, 0, 0};
    
//     unsigned long startTime = MillisecondCounter;
    
//     EnableInterrupts();

//     while(1) {
//         if (g_sendData == TRUE && running) {
//             // find first and last maximum value in line array
//             int maxFirstIndex = 0;
//             int maxLastIndex = 127;
//             const int maxValue = 16383;
//             int i;
//             for (i = 0; i < 128; i++) {
//                 if (line[i] == maxValue) {
//                     maxFirstIndex = i;
//                     numFramesOffTrack = 0;
//                     break;
//                 }
//             }
//             if (i == 128) {
//                 // no max value found
//                 numFramesOffTrack++;
//             }
//             for (i = 127; i >= 0; i--) {
//                 if (line[i] == maxValue) {
//                     maxLastIndex = i;
//                     numFramesOffTrack = 0;
//                     break;
//                 }
//             }
//             if (i == -1) {
//                 // no max value found
//                 numFramesOffTrack++;
//             }
//             if (numFramesOffTrack > maxFramesOffTrack) {
//                 running = FALSE;
//             }

//             int middleIndex = (maxFirstIndex + maxLastIndex) / 2;
            
//             float centerOffset = (middleIndex - 64)/64.0;
            
//             // map centerOffset to powerTarget continuously
//             // powerTarget = fmin(0.5, fmax(0.25, 1/(10.0*fabs(centerOffset)+2.3)));
            
//             powerTarget = 0.4;
            
//             // set servo target to be the angle of the maxIndex mapped from -47 to 47
//             currentAngle = PIDUpdate(&pid, centerOffset);
            
//             setServoAngle(-currentAngle);
            
//             g_sendData = FALSE;
//         }
//         if (!running) {
//             powerTarget = 0;
//             currentPower = 0;
//             setServoAngle(0);
            
//             // check if button is pressed to start car moving again
//             if (Switch1_Pressed()) {
//                 running = TRUE;
//                 numFramesOffTrack = 0;
//             }
//         }
//     }
// }
 
volatile uint32_t MillisecondCounter = 0;

void Timer32_2_ISR_Main(void) {
    MillisecondCounter++;
}

//
// main
//
int main(void) {
    // initializations
    uart_init();
    uart_put("\r\n CRust \r\n");
    
    // init timer for millisecond counter
    Timer32_2_Init(&Timer32_2_ISR_Main, CalcPeriodFromFrequency(1000), T32DIV1); // initialize Timer A32-1;
    EnableInterrupts();
    
    LED1_Init();
    LED2_Init();
    Switches_Init();
    servoInit();
    motor1Init();
    motor2Init();
    INIT_Camera();
    
    // turn led2 off
    setLedLow(LED2_RED_PORT, LED2_RED_PIN);
    setLedLow(LED2_GREEN_PORT, LED2_GREEN_PIN);
    setLedLow(LED2_BLUE_PORT, LED2_BLUE_PIN);
    
    // turn led1 off
    setLedLow(LED1_PORT, LED1_PIN);
    
    int selectedSpeedMode = 0;
    
    int previouslyPressed = 0;
    
    const uint32_t triggerSwitchTime = 2000;
    int startSwitchTime = 0;
    BOOLEAN startSwitchTiming = FALSE;
    
    // wait until a button is pressed, and run the corresponding program
    while(1) {
        // if (Switch1_Pressed()) {
        //     setLedHigh(LED2_RED_PORT, LED2_RED_PIN);
        //     slow();
        // }
        // if (Switch2_Pressed()) {
        //     setLedHigh(LED2_GREEN_PORT, LED2_GREEN_PIN);
        //     fast();
        // }
        
        if (Switch2_Pressed() && previouslyPressed == 0) {
            selectedSpeedMode = (selectedSpeedMode + 1) % 3;
            startSwitchTime = MillisecondCounter;
            startSwitchTiming = TRUE;
        }
        
        previouslyPressed = Switch2_Pressed();
        
        switch (selectedSpeedMode) {
            case 0:
                setLedHigh(LED2_RED_PORT, LED2_RED_PIN);
                setLedLow(LED2_GREEN_PORT, LED2_GREEN_PIN);
                setLedLow(LED2_BLUE_PORT, LED2_BLUE_PIN);
                break;
            case 1:
                setLedLow(LED2_RED_PORT, LED2_RED_PIN);
                setLedHigh(LED2_GREEN_PORT, LED2_GREEN_PIN);
                setLedLow(LED2_BLUE_PORT, LED2_BLUE_PIN);
                break;
            case 2:
                setLedLow(LED2_RED_PORT, LED2_RED_PIN);
                setLedLow(LED2_GREEN_PORT, LED2_GREEN_PIN);
                setLedHigh(LED2_BLUE_PORT, LED2_BLUE_PIN);
                break;
        }
        
        uint32_t diff = MillisecondCounter - startSwitchTime;
        
        if (startSwitchTiming && diff > triggerSwitchTime/2) {
            setLedHigh(LED1_PORT, LED1_PIN);
        } else {
            setLedLow(LED1_PORT, LED1_PIN);
        }
        
        if (startSwitchTiming && diff > triggerSwitchTime) {
            switch (selectedSpeedMode) {
                case 0:
                    slow();
                    break;
                case 1:
                    fast();
                    break;
                case 2:
                    fastest();
                    break;
            }
            startSwitchTiming = FALSE;
            startSwitchTime = 0;
            Timer32_2_Init(&Timer32_2_ISR_Main, CalcPeriodFromFrequency(1000), T32DIV1); // initialize Timer A32-1;
        }
        
        // setLedLow(LED2_RED_PORT, LED2_RED_PIN);
        // setLedLow(LED2_GREEN_PORT, LED2_GREEN_PIN);
        // setLedLow(LED2_BLUE_PORT, LED2_BLUE_PIN);
        setMotor1Power(0);
        setMotor2Power(0);
        setServoAngle(0);
    }
}
