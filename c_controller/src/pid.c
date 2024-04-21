#include "pid.h"

float PIDUpdate(PID* pid, float error, float dt) {
    pid->integral += error * dt;
    // clamp integral
    if (pid->integral > pid->integralLimit) {
        pid->integral = pid->integralLimit;
    } else if (pid->integral < -pid->integralLimit) {
        pid->integral = -pid->integralLimit;
    }

    float derivative = error - pid->lastVal;
    pid->lastVal = error;
    return pid->P * error + pid->I * pid->integral + pid->D * derivative * dt;
}