#include "TimerA.h"
#include "msp.h"

#define MIN_DUTY_CYCLE 0.05
#define MAX_DUTY_CYCLE 0.1
#define SERVO_FREQ 50

void servoInit() {
    TIMER_A2_PWM_Init((uint16_t)((double)SystemCoreClock/(SERVO_FREQ*64)), (MIN_DUTY_CYCLE + MAX_DUTY_CYCLE)/2.0, 1);
}

// 0 to 1 position
void setServoPosition(double position) {
    if (position < 0) {
        position = 0;
    } else if (position > 1) {
        position = 1;
    }
    
    double dutyCycle = MIN_DUTY_CYCLE + position * (MAX_DUTY_CYCLE - MIN_DUTY_CYCLE);
    
    TIMER_A2_PWM_DutyCycle(dutyCycle, 1);
}