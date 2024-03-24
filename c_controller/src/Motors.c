#include "TimerA.h"
#include "msp.h"

#define MOTOR1_FREQ 10000

void motor1Init() {
    // make pin P3.7 an output, and set it low
    P3->DIR |= BIT7;
    P3->OUT &= ~BIT7;
    P3->SEL0 &= ~BIT7;
    P3->SEL1 &= ~BIT7;
    TIMER_A0_PWM_Init((uint16_t)((double)SystemCoreClock/MOTOR1_FREQ), 0.0, 1);
    TIMER_A0_PWM_Init((uint16_t)((double)SystemCoreClock/MOTOR1_FREQ), 0.0, 2);
}

// -1 to 1 power
void setMotor1Power(double power) {
    P3->OUT |= BIT7;
    if (power < -1) {
        power = -1;
    } else if (power > 1) {
        power = 1;
    }
    
    if (power < 0) {
        TIMER_A0_PWM_DutyCycle(0, 1);
        TIMER_A0_PWM_DutyCycle(-power, 2);
    } else {
        TIMER_A0_PWM_DutyCycle(power, 1);
        TIMER_A0_PWM_DutyCycle(0, 2);
    }
}

void motor2Init() {
    // make pin P3.6 an output, and set it low
    P3->DIR |= BIT6;
    P3->OUT &= ~BIT6;
    P3->SEL0 &= ~BIT6;
    P3->SEL1 &= ~BIT6;
    TIMER_A0_PWM_Init((uint16_t)((double)SystemCoreClock/MOTOR1_FREQ), 0.0, 3);
    TIMER_A0_PWM_Init((uint16_t)((double)SystemCoreClock/MOTOR1_FREQ), 0.0, 4);
}

// -1 to 1 power
void setMotor2Power(double power) {
    P3->OUT |= BIT6;
    if (power < -1) {
        power = -1;
    } else if (power > 1) {
        power = 1;
    }
    
    if (power < 0) {
        TIMER_A0_PWM_DutyCycle(0, 3);
        TIMER_A0_PWM_DutyCycle(-power, 4);
    } else {
        TIMER_A0_PWM_DutyCycle(power, 3);
        TIMER_A0_PWM_DutyCycle(0, 4);
    }
}