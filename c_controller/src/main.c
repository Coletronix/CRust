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

volatile int colorIndex = 0;
BYTE colors[7] = { RED, GREEN, BLUE, CYAN, MAGENTA, YELLOW, WHITE };

// led blinking?
volatile BOOLEAN LED1RunningFlag = TRUE;

volatile unsigned long MillisecondCounter = 0;

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

volatile double powerTarget = 0;
volatile double currentPower = 0;

#define MAX_ACCELERATION 15

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
    setMotor1Power(currentPower);
    setMotor2Power(currentPower);
}

void setServoAngle(double angle) {
    double position = STEERING_CENTER + angle * STEERING_GRADIANS_PER_DEGREE;
    setServoPosition(position);
}

//
// main
//
int main(void) {
    // initializations
    uart0_init();
    uart0_put("\r\n CRust \r\n");

    // Set the Timer32-1 to 2Hz (0.5 sec between interrupts)
    Timer32_1_Init(&Timer32_1_ISR, CalcPeriodFromFrequency(2), T32DIV1); // initialize Timer A32-1;
    
    
    // Setup Timer32-2 with a .001 second timeout.
    // So use DEFAULT_CLOCK_SPEED/(1/0.001) = SystemCoreClock/1000
    Timer32_2_Init(&Timer32_2_ISR, CalcPeriodFromFrequency(1000), T32DIV1); // initialize Timer A32-1;
    
    LED1_Init();
    LED2_Init();
    servoInit();
    motor1Init();
    motor2Init();
    INIT_Camera();
    
    // turn led2 off
    setLedLow(LED2_RED_PORT, LED2_RED_PIN);
    setLedLow(LED2_GREEN_PORT, LED2_GREEN_PIN);
    setLedLow(LED2_BLUE_PORT, LED2_BLUE_PIN);
    
    setServoAngle(25);
    powerTarget = 0;
    
    EnableInterrupts();

    while(1) {
        // WaitForInterrupt();
        // if (MillisecondCounter > 3000) {
        //     // powerTarget = .26;
        //     powerTarget = .45;
        //     // currentPower = 0.6;
        // }
        if (g_sendData == TRUE) {
            // find first and last maximum value in line array
            int maxFirstIndex = 0;
            int maxLastIndex = 127;
            const int maxValue = 16383;
            for (int i = 0; i < 128; i++) {
                if (line[i] == maxValue) {
                    maxFirstIndex = i;
                    break;
                }
            }
            for (int i = 127; i >= 0; i--) {
                if (line[i] == maxValue) {
                    maxLastIndex = i;
                    break;
                }
            }
            int middleIndex = (maxFirstIndex + maxLastIndex) / 2;
            
            int centerOffset = middleIndex - 64;
            
            // if (abs(centerOffset) > 10) {
            //     powerTarget = 0.26;
            // } else {
            //     powerTarget = .4;
            // }
            
            // map centerOffset to powerTarget continuously
            powerTarget = 0.4 - (double)(abs(centerOffset)) / 64.0 * 0.2;
            
            // set servo target to be the angle of the maxIndex mapped from -47 to 47
            double angle = (double)(centerOffset) / 64.0 * 47.0;
            setServoAngle(-angle);
            char buffer[1024];
            // sprintf(buffer, "Middle Index: %d, Angle: %d\r\n", middleIndex, (int)angle);
            // uart0_put(buffer);
            // sprintf(buffer, "line data: [%d, %d, %d, %d, %d, %d, %d, %d]\r\n", line[0], line[1], line[2], line[3], line[4], line[5], line[6], line[7]);
            // uart0_put(buffer);

            
            g_sendData = FALSE;
        }
    }
}
