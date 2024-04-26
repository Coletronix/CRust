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

// bluetooth packet types
#define CENTER_OFFSET_P 0x01
#define CORRECTION_OFFSET_P 0x02
#define MOTOR1_P 0x03
#define MOTOR2_P 0x04
#define SERVO_P 0x05
#define BLE_CONTROLLED_P 0x06
#define PACKET_END_P 0xFF

// 1 second
#define MAX_FRAMES_OFF_TRACK (50)
// from ControlPins.c
// #define UPDATE_DT (1.0/50.0) 
#define UPDATE_DT (1.0/100.0) 

volatile uint32_t MillisecondCounter2 = 0;

enum State {
    STRAIGHT,
    TURN,
    ENTERING_TURN,
};

struct SimulatedMotor {
    float p;
    float currentSpeed;
};

void Timer32_2_ISR_2(void) {
    MillisecondCounter2++;
}

void sendFloatBLE(float data) {
    // Multiply it by 1000, then send the part larger than 1000
    int dataInt = (int)(data * 1000);
    int dataIntPart = dataInt / 1000;
    int dataDecimalPart = dataInt % 1000;
    char buf[24];
    sprintf(buf, "%d.%03d", dataIntPart, abs(dataDecimalPart));
    uart_put(buf);
}


void fast() {
    Timer32_2_Init(&Timer32_2_ISR_2, CalcPeriodFromFrequency(1000), T32DIV1); // initialize Timer A32-1;
    
    float motor1Power = 0.0;
    float motor2Power = 0.0;

    BOOLEAN running = TRUE;
    int numFramesOffTrack = 0;
    float fastSpeed = .4;
    float turnSpeed = .33;
    float lastCorrection = 0;
    
    float numFramesStraight = 0;
    
    enum State state = STRAIGHT;
    
    PID turnPID = {57.0, 0.0, 0.0, 0, 0, 10.0};
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
            // if (numFramesOffTrack > MAX_FRAMES_OFF_TRACK) {
            //     running = FALSE;
            // }
            
            float correction;
            switch (state) {
                case STRAIGHT:
                    setLedHigh(LED2_BLUE_PORT, LED2_BLUE_PIN);
                    
                    correction = PIDUpdate(&turnPID, centerOffset, UPDATE_DT);
                    float diff = 0.003;
                    
                    motor1Power = turnSpeed + diff * correction;
                    motor2Power = turnSpeed - diff * correction;
                    
                    if (fabs(centerOffset) < .1) {
                        numFramesStraight++;
                    } else {
                        numFramesStraight = 0;
                    }
                    
                    // if (numFramesStraight > 25) {
                    //     running = FALSE;
                    // }
                    
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
            uart_put("centerOffset: ");
            sendFloatBLE(centerOffset);
            uart_put(" correction: ");
            sendFloatBLE(correction);
            uart_put(" motor1Power: ");
            sendFloatBLE(motor1Power);
            uart_put(" motor2Power: ");
            sendFloatBLE(motor2Power);
            uart_put("\r\n");
        }
    }
    
    setMotor1Power(0);
    setMotor2Power(0);
}

void fast2() {
    Timer32_2_Init(&Timer32_2_ISR_2, CalcPeriodFromFrequency(1000), T32DIV1); // initialize Timer A32-1;

    const float diff = 0.006;
    
    float motor1Power = 0.0;
    float motor2Power = 0.0;

    BOOLEAN running = TRUE;
    int numFramesOffTrack = 0;
    float fastSpeed = .4;
    float lastCorrection = 0;
    
    int straightCounter = 0;
    
    int numFramesUnderThreshold = 0;
    
   PID straightPID = {54.0, 0.0, 10000.0, 0, 0, 10.0};

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
            
            float correction = PIDUpdate(&straightPID, centerOffset, UPDATE_DT);
            
            // float speed = .45 + fabs(correction) * (-.3-.45)/64.0;
            float speed = .45 + fabs(centerOffset) * (-.15-.45);
            // float speed = fmin(fabs(.5*correction), .45);
             
            if (fabs(speed) < 0.2) {
                numFramesUnderThreshold++;
            } else {
                numFramesUnderThreshold = 0;
            }
            
            if (numFramesUnderThreshold > 30) {
                speed = 0.25;
            }
            
            float turnAmt = diff * correction;
            
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

void fast_1() {
    Timer32_2_Init(&Timer32_2_ISR_2, CalcPeriodFromFrequency(1000), T32DIV1); // initialize Timer A32-1;

    const float diff = 0.003;
    const float straightToTurnCutoff = 15;
    const float turnToStraightCutoff = 7.5;
    const int framesToTurn = 5;
    const int minFastFramesNeededForSlowdown = 50;
    
    uint32_t frameCounter = 0;
    
    uint32_t straightStartTime = 0;
    
    float motor1Power = 0.0;
    float motor2Power = 0.0;

    BOOLEAN running = TRUE;
    int numFramesOffTrack = 0;
    float fastSpeed = .4;
    float slowSpeed = .25;
    BOOLEAN turning = FALSE;
    
    BOOLEAN specialSlowdown = FALSE;
    int specialSlowdownCounter = 0;
    int maxSpecialSlowdownCounter = 20;
    
    int straightCounter = 0;
    int turnCounter = 0;
    
    PID straightPID = {50.0, 0.0, 10000.0, 0, 0, 10.0};
    PID turnPID = {57.0, 0.0, 0.0, 0, 0, 10.0};

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
            
            float correction = 0;
            if (turning) {
                setLedHigh(LED2_BLUE_PORT, LED2_BLUE_PIN);
                straightCounter = 0;

                correction = PIDUpdate(&turnPID, centerOffset, UPDATE_DT);
                
                if (fabs(correction) < turnToStraightCutoff && turnCounter > framesToTurn) {
                    turning = FALSE;
                    straightStartTime = frameCounter;
                }

                setServoAngle(-correction);

                motor1Power = slowSpeed + diff * correction;
                motor2Power = slowSpeed - diff * correction;
                
                turnCounter++;
            } else { // straight
                setLedLow(LED2_BLUE_PORT, LED2_BLUE_PIN);
                turnCounter = 0;
                
                correction = PIDUpdate(&straightPID, centerOffset, UPDATE_DT);
                
                if (fabs(correction) > straightToTurnCutoff && straightCounter > framesToTurn) {
                    turning = TRUE;
                    if (straightCounter > minFastFramesNeededForSlowdown) {
                        specialSlowdown = TRUE;
                        
                        uint32_t timeStraight = frameCounter - straightStartTime;
                        
                        // Calculate how long to slow down for
                        maxSpecialSlowdownCounter = 90.0/(1+pow(2.71828, -timeStraight/20.0));
                    }
                }
                
                setServoAngle(-correction);
                motor1Power = fastSpeed;
                motor2Power = fastSpeed;

                straightCounter++;
            }
            
            if (specialSlowdown) {
                specialSlowdownCounter++;
                if (specialSlowdownCounter > maxSpecialSlowdownCounter) {
                    specialSlowdown = FALSE;
                    specialSlowdownCounter = 0;
                }
                motor1Power = -fastSpeed;
                motor2Power = -fastSpeed;
            }
            
            setMotor1Power(motor1Power);
            setMotor2Power(motor2Power);

            frameCounter++;
        }
    }
    
    setMotor1Power(0);
    setMotor2Power(0);
}