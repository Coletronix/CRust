#include "pid.h"

float PIDUpdate(PID* pid, float error) {
    pid->integral += error;
    float derivative = error - pid->lastVal;
    pid->lastVal = error;
    return pid->P * error + pid->I * pid->integral + pid->D * derivative;
}