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

volatile uint32_t MillisecondCounter4 = 0;

enum State {
    STRAIGHT,
    TURN,
    ENTERING_TURN
};

// 1.5 second
#define MAX_FRAMES_OFF_TRACK (150)
// from ControlPins.c
#define UPDATE_DT (1.0/100.0) // this is wrong...

// meters
#define WHEEL_BASE (5.5 * 0.0254)

void Timer32_2_ISR_4(void) {
    MillisecondCounter4++;
}

void fasterist() {
    Timer32_2_Init(&Timer32_2_ISR_4, CalcPeriodFromFrequency(1000), T32DIV1); // initialize Timer A32-1;
    
    float motor1Power = 0.0;
    float motor2Power = 0.0;

    BOOLEAN running = TRUE;
    int numFramesOffTrack = 0;
    float fastSpeed = .55;
    float turnSpeed = .33;
    float lastCorrection = 0;
    
    float numFramesStraight = 0;

    uint32_t startAcceleratingTime = MillisecondCounter4;
    uint32_t endAcceleratingTime = startAcceleratingTime;
    uint32_t startDeceleratingTime = startAcceleratingTime;
    uint32_t startTurnTime = MillisecondCounter4;
    
    BOOLEAN firstEnteredDecel = FALSE;
    
    enum State state = STRAIGHT;
    
    PID turnPID = {57.0, 0.0, 100000.0, 0, 0, 10.0};
    PID straightPID = {30.0, 0.0, 10000.0, 0, 0, 10.0};

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
            
            // loop through and print out all line data
            // for (int i = 0; i < 128; i++) {
            //     char buf[512];
            //     sprintf(buf, "%d ", line[i]);
            //     uart_put(buf);
            // }
            // uart_put("\r\n");
            
            // continue;

            float speed = 0.0;
            float correction;
            float diff;
            switch (state) {
                case STRAIGHT:
                    setLedLow(LED2_RED_PORT, LED2_RED_PIN);
                    setLedHigh(LED2_GREEN_PORT, LED2_GREEN_PIN);
                    setLedLow(LED2_BLUE_PORT, LED2_BLUE_PIN);
                    
                    correction = PIDUpdate(&straightPID, centerOffset, UPDATE_DT);
                    
                    float timeStraight = (float)(MillisecondCounter4 - startAcceleratingTime);
                    
                    // speed = (1.0 - fastSpeed) * exp(-0.006 * timeStraight) + fastSpeed;
                    speed = (1.0 - fastSpeed) * exp(-0.0051 * timeStraight) + fastSpeed;

                    motor1Power = speed;
                    motor2Power = speed;
                    
                    
                    if (noTrack) {
                        setServoAngle(-lastCorrection);
                    } else {
                        setServoAngle(-correction);
                        lastCorrection = correction;
                    }
                    setMotor1Power(motor1Power);
                    setMotor2Power(motor2Power);
                    
                    // state change case
                    if (fabs(centerOffset) > .24) {
                        state = ENTERING_TURN;
                        endAcceleratingTime = MillisecondCounter4;
                        startDeceleratingTime = MillisecondCounter4;
                    }
                    
                    // uart_put("In Straight mode");
                    
                    break;
                    
                case ENTERING_TURN:
                    setLedHigh(LED2_RED_PORT, LED2_RED_PIN);
                    setLedLow(LED2_GREEN_PORT, LED2_GREEN_PIN);
                    setLedLow(LED2_BLUE_PORT, LED2_BLUE_PIN);
                    
                    // calculate total time spent accelerating
                    uint32_t timeAccelerating = endAcceleratingTime - startAcceleratingTime;
                    
                    // char buf[30];
                    // sprintf(buf, "Time accelerating: %d\r\n", timeAccelerating);
                    // uart_put(buf);

                    // calculate time to decelerate
                    // uint32_t timeToDecelerate = 100.0 * sqrt(sqrt((float)timeAccelerating));
                    // uint32_t timeToDecelerate = -500*exp(-0.001 * (float)timeAccelerating) + 500;
                    // uint32_t timeToDecelerate = 500.0 / (1 + exp(-.01 * (timeAccelerating - 500)));
                    // uint32_t timeToDecelerate = 500.0 / (1 + exp(-.003 * (timeAccelerating - 700)));
                    uint32_t timeToDecelerate = 0.26 * timeAccelerating;
                    // uint32_t timeToDecelerate = 500;
                    float decelAmt = -.2;

                    correction = PIDUpdate(&turnPID, centerOffset, UPDATE_DT);
                    
                    if (noTrack) {
                        setServoAngle(-lastCorrection);
                    } else {
                        setServoAngle(-correction);
                        lastCorrection = correction;
                    }
                    
                    setMotor1Power(decelAmt);
                    setMotor2Power(decelAmt);

                    // uart_put("In Straight mode");
                    
                    if (MillisecondCounter4 - startDeceleratingTime > timeToDecelerate) {
                        state = TURN;
                        startTurnTime = MillisecondCounter4;
                    }
                    
                    break;
                
                case TURN:
                    setLedLow(LED2_RED_PORT, LED2_RED_PIN);
                    setLedLow(LED2_GREEN_PORT, LED2_GREEN_PIN);
                    setLedHigh(LED2_BLUE_PORT, LED2_BLUE_PIN);
                    
                    correction = PIDUpdate(&turnPID, centerOffset, UPDATE_DT);
                    diff = 0.003;
                    
                    float turnTime = (float)(MillisecondCounter4 - startTurnTime);
                    
                    speed = (1.0 - turnSpeed) * exp(-0.04 * turnTime) + turnSpeed;

                    motor1Power = speed + diff * correction;
                    motor2Power = speed - diff * correction;
                    
                    if (fabs(centerOffset) < .2) {
                        numFramesStraight++;
                    } else {
                        numFramesStraight = 0;
                    }
                    
                    if (numFramesStraight > 40) {
                        state = STRAIGHT;
                        startAcceleratingTime = MillisecondCounter4;
                    }
                    
                    // when to switch to straight
                    
                    if (noTrack) {
                        setServoAngle(-lastCorrection);
                    } else {
                        setServoAngle(-correction);
                        lastCorrection = correction;
                    }
                    setMotor1Power(motor1Power);
                    setMotor2Power(motor2Power);
                    break;
            }
            // uart_put("centerOffset: ");
            // sendFloatBLE(centerOffset);
            // uart_put(" correction: ");
            // sendFloatBLE(correction);
            // uart_put(" motor1Power: ");
            // sendFloatBLE(motor1Power);
            // uart_put(" motor2Power: ");
            // sendFloatBLE(motor2Power);
            // uart_put("\r\n");
        }
    }
    
    setMotor1Power(0);
    setMotor2Power(0);
}