#ifndef __PID_H__
#define __PID_H__

typedef struct
{

    float tgt;

    /* Controller gains */
    float Kp;
    float Ki;
    float Kd;

    /* Derivative low-pass filter time constant */
    float tau;

    /* Output limits */
    float limMin;
    float limMax;

    /* Integrator limits */
    float limMinInt;
    float limMaxInt;

    /* Sample time (in seconds) */
    float T;

    /* Controller "memory" */
    float integrator;
    float prevError; /* Required for integrator */
    float differentiator;
    float prevMeasurement; /* Required for differentiator */

    /* Controller output */
    float out;

    void (*s_callback)(void *instance, float value);
    void *s_instance;

} PIDController;

PIDController *PIDController_Init(float Kp, float Ki, float Kd, float tau, float limMin, float limMax, float limMinInt, float limMaxInt, float T);
float PIDController_Update(PIDController *pid, float setpoint, float measurement);
void PIDController_SetTgt(PIDController *pid, float tgt);
void PIDController_AssumeControl(PIDController *pid, void (*callback)(void *instance, float value), void *instance);
void PIDController_ReleaseControl(PIDController *pid);

#endif
