#ifndef PID_H
#define PID_H

// struct to hold PID
typedef struct {
    float P;
    float I;
    float D;
    float integral;
    float lastVal;
    float integralLimit;
} PID;

float PIDUpdate(PID* pid, float error, float dt);

#endif