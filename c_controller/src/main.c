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

extern uint32_t SystemCoreClock;
extern uint16_t line[128];
extern BOOLEAN g_sendData;

#define STEERING_CENTER 0.55
#define STEERING_GRADIANS_PER_DEGREE ((0.55 - 0.2) / 20.0)
#define STEERING_MAX 25
#define STEERING_MIN (-25)

#define MIN_RADIUS (914.4/2.0)

#define MAX_ACCELERATION 15

volatile float powerTarget = 0;
volatile float currentPower = 0;
volatile float currentAngle = 0;

volatile int colorIndex = 0;
BYTE colors[7] = { RED, GREEN, BLUE, CYAN, MAGENTA, YELLOW, WHITE };

volatile unsigned long MillisecondCounter = 0;

// struct to hold PID
typedef struct {
    float P;
    float I;
    float D;
    float integral;
    float lastVal;
} PID;

float PIDUpdate(PID* pid, float error) {
    pid->integral += error;
    float derivative = error - pid->lastVal;
    pid->lastVal = error;
    return pid->P * error + pid->I * pid->integral + pid->D * derivative;
}

// meters
#define WHEEL_BASE 0.127

float calculateTurnRadius(float angle) {
    float tanAngle = tan(angle);
    if (tanAngle == 0) {
        return 1000000; // random big number
    }
    return WHEEL_BASE / tanAngle;
}

void delay_ms(int del){
    volatile int i;
    for (i=0; i<del*50000; i++){
        ;// Do nothing
    }
}

//
// Interrupt Service Routine for Timer32-1
//
void Timer32_1_ISR(void) {
}


void INIT_Camera(void) {
    g_sendData = FALSE;
    ControlPin_SI_Init();
    ControlPin_CLK_Init();
    ADC0_InitSWTriggerCh6();
}

//
// Interrupt Service Routine
//
void Timer32_2_ISR(void) {
    MillisecondCounter++;
    
    // 1ms due to 1000Hz frequency
    const double deltaT = 1.0/1000.0;
    
    // char buffer[64];
    // sprintf(buffer, "powerTarget: %d, currentPower: %d, Time: %d\r\n", (int)(powerTarget*100), (int)(currentPower*100), MillisecondCounter);
    // uart0_put(buffer);
    
    // Trapazoidal motion profile for motor power using powerTarget
    if (powerTarget > currentPower) {
        currentPower += MAX_ACCELERATION * deltaT;
        if (currentPower > powerTarget) {
            currentPower = powerTarget;
        }
    } else if (powerTarget < currentPower) {
        currentPower -= MAX_ACCELERATION * deltaT;
        if (currentPower < powerTarget) {
            currentPower = powerTarget;
        }
    }

    float thing = 0.003;
    float turnRadius = calculateTurnRadius(currentAngle);
    float maxCorrection = fabs(sqrt(turnRadius));
    // float correction = currentAngle * thing;
    float correction = 0.0;
    // constrain correction
    // if (correction > maxCorrection) {
    //     correction = maxCorrection;
    // } else if (correction < -maxCorrection) {
    //     correction = -maxCorrection;
    // }
    setMotor1Power(currentPower + correction);
    setMotor2Power(currentPower - correction);
}

void setServoAngle(double angle) {
    double position = STEERING_CENTER + angle * STEERING_GRADIANS_PER_DEGREE;
    setServoPosition(position);
}

void slowAndReliable() {
    powerTarget = 0;
    int numFramesOffTrack = 0;
    const int maxFramesOffTrack = 10;
    BOOLEAN running = FALSE;
    // pid struct
    // PID pid = {57.0, 0.1, 0.0, 0, 0};
    PID pid = {25.0, 0.1, 0.0, 0, 0};
    
    EnableInterrupts();

    while(1) {
        if (g_sendData == TRUE && running) {
            // find first and last maximum value in line array
            int maxFirstIndex = 0;
            int maxLastIndex = 127;
            const int maxValue = 16383;
            int i;
            for (i = 0; i < 128; i++) {
                if (line[i] == maxValue) {
                    maxFirstIndex = i;
                    numFramesOffTrack = 0;
                    break;
                }
            }
            if (i == 128) {
                // no max value found
                numFramesOffTrack++;
            }
            for (i = 127; i >= 0; i--) {
                if (line[i] == maxValue) {
                    maxLastIndex = i;
                    numFramesOffTrack = 0;
                    break;
                }
            }
            if (i == -1) {
                // no max value found
                numFramesOffTrack++;
            }
            if (numFramesOffTrack > maxFramesOffTrack) {
                running = FALSE;
            }

            int middleIndex = (maxFirstIndex + maxLastIndex) / 2;
            
            float centerOffset = (middleIndex - 64)/64.0;
            
            // map centerOffset to powerTarget continuously
            // powerTarget = fmin(0.5, fmax(0.25, 1/(10.0*fabs(centerOffset)+2.3)));
            powerTarget = 0.25;
            
            // set servo target to be the angle of the maxIndex mapped from -47 to 47
            currentAngle = PIDUpdate(&pid, centerOffset);
            
            setServoAngle(-currentAngle);
            
            g_sendData = FALSE;
        }
        if (!running) {
            powerTarget = 0;
            currentPower = 0;
            setServoAngle(0);
            
            // check if button is pressed to start car moving again
            if (Switch1_Pressed()) {
                running = TRUE;
                numFramesOffTrack = 0;
            }
        }
    }
}

void faster() {
    powerTarget = 0;
    int numFramesOffTrack = 0;
    const int maxFramesOffTrack = 10;
    BOOLEAN running = FALSE;
    // pid struct
    // PID pid = {57.0, 0.1, 0.0, 0, 0};
    PID pid = {10.0, 0.1, 0.0, 0, 0};
    powerTarget = 1;
    
    unsigned long startTime = MillisecondCounter;
    
    EnableInterrupts();

    while(1) {
        if (g_sendData == TRUE && running) {
            // find first and last maximum value in line array
            int maxFirstIndex = 0;
            int maxLastIndex = 127;
            const int maxValue = 16383;
            int i;
            for (i = 0; i < 128; i++) {
                if (line[i] == maxValue) {
                    maxFirstIndex = i;
                    numFramesOffTrack = 0;
                    break;
                }
            }
            if (i == 128) {
                // no max value found
                numFramesOffTrack++;
            }
            for (i = 127; i >= 0; i--) {
                if (line[i] == maxValue) {
                    maxLastIndex = i;
                    numFramesOffTrack = 0;
                    break;
                }
            }
            if (i == -1) {
                // no max value found
                numFramesOffTrack++;
            }
            if (numFramesOffTrack > maxFramesOffTrack) {
                // running = FALSE;
            }

            int middleIndex = (maxFirstIndex + maxLastIndex) / 2;
            
            float centerOffset = (middleIndex - 64)/64.0;
            
            // map centerOffset to powerTarget continuously
            // powerTarget = fmin(0.5, fmax(0.25, 1/(10.0*fabs(centerOffset)+2.3)));
            
            // if (fabs(centerOffset) > 0.9) {
            //     powerTarget = 0;
            // }
            if (MillisecondCounter - startTime < 1000) {
                powerTarget = 1;
            } else {
                powerTarget = 0;
            }
            
            
            // set servo target to be the angle of the maxIndex mapped from -47 to 47
            currentAngle = PIDUpdate(&pid, centerOffset);
            
            setServoAngle(-currentAngle);
            
            g_sendData = FALSE;
        }
        if (!running) {
            powerTarget = 0;
            currentPower = 0;
            setServoAngle(0);
            
            // check if button is pressed to start car moving again
            if (Switch2_Pressed()) {
                running = TRUE;
                numFramesOffTrack = 0;
            }
        }
    }
}
//
// main
//
int main(void) {
    // initializations
    uart_init();
    uart_put("\r\n CRust \r\n");

    // Set the Timer32-1 to 2Hz (0.5 sec between interrupts)
    Timer32_1_Init(&Timer32_1_ISR, CalcPeriodFromFrequency(2), T32DIV1); // initialize Timer A32-1;
    
    
    // Setup Timer32-2 with a .001 second timeout.
    // So use DEFAULT_CLOCK_SPEED/(1/0.001) = SystemCoreClock/1000
    Timer32_2_Init(&Timer32_2_ISR, CalcPeriodFromFrequency(1000), T32DIV1); // initialize Timer A32-1;
    
    LED1_Init();
    LED2_Init();
    Switch1_Init();
    servoInit();
    motor1Init();
    motor2Init();
    INIT_Camera();
    
    // turn led2 off
    setLedLow(LED2_RED_PORT, LED2_RED_PIN);
    setLedLow(LED2_GREEN_PORT, LED2_GREEN_PIN);
    setLedLow(LED2_BLUE_PORT, LED2_BLUE_PIN);
    
    
    // wait until a button is pressed, and run the corresponding program
    while(1) {
        if (Switch1_Pressed()) {
            slowAndReliable();
        }
        if (Switch2_Pressed()) {
            faster();
        }
    }
}
