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

// 1.5 second
#define MAX_FRAMES_OFF_TRACK (150)
// from ControlPins.c
// #define UPDATE_DT (1.0/50.0) 
#define UPDATE_DT (1.0/100.0) 


void driveBySerial() {
    while (TRUE) {
        if (uart_dataAvailable()) {
            char c = uart_getchar();
            
            const float speed = 0.5;
            if (c == 'w') {
                setMotor1Power(speed);
                setMotor2Power(speed);
                setServoAngle(0);
            } else if (c == 's') {
                setMotor1Power(-speed);
                setMotor2Power(-speed);
                setServoAngle(0);
            } else if (c == 'a') {
                setMotor1Power(speed);
                setMotor2Power(speed);
                setServoAngle(20);
            } else if (c == 'd') {
                setMotor1Power(speed);
                setMotor2Power(speed);
                setServoAngle(-20);
            } else if (c == 'x') {
                setMotor1Power(0);
                setMotor2Power(0);
            }
        }
    }
}