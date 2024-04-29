#include "TimerA.h"
#include "msp.h"

#define STEERING_CENTER 0.55
#define STEERING_GRADIANS_PER_DEGREE ((0.55 - 0.2) / 20.0)
#define STEERING_MAX 25
#define STEERING_MIN (-25)

#define MIN_DUTY_CYCLE 0.05
#define MAX_DUTY_CYCLE 0.1
#define SERVO_FREQ 50

// initialize the servo to run at 50 Hz at the middle position
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

// angle in degrees away from center position with positive values steering counter-clockwise
void setServoAngle(double angle) {
    double position = STEERING_CENTER + angle * STEERING_GRADIANS_PER_DEGREE;
    setServoPosition(position);
}