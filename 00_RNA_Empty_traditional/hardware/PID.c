#include "PID.h"
#include <stdlib.h>

/**
 * @brief Initialize a PID controller
 * 
 * @param Kp        Proportional gain
 * @param Ki        Integral gain
 * @param Kd        Derivative gain
 * @param tau       Derivative low-pass filter time constant
 * @param limMin    Minimum output value
 * @param limMax    Maximum output value
 * @param limMinInt Minimum integral windup value
 * @param limMaxInt Maximum integral windup value
 * @param T         Sampling time
 * @return PIDController* `                                                                                                                                         
 */
inline PIDController *PIDController_Init(float Kp, float Ki, float Kd, float tau, float limMin, float limMax, float limMinInt, float limMaxInt, float T)
{
    // MAP_FPU_enableModule();
    // MAP_FPU_enableLazyStacking();
    PIDController *pid = (PIDController *)malloc(sizeof(PIDController));
    pid->tgt = 0;
    pid->Kp = Kp;
    pid->Ki = Ki;
    pid->Kd = Kd;
    pid->tau = tau;
    pid->limMin = limMin;
    pid->limMax = limMax;
    pid->limMinInt = limMinInt;
    pid->limMaxInt = limMaxInt;
    pid->T = T;

    /* Clear controller variables */
    pid->integrator = 0.0f;
    pid->prevError = 0.0f;

    pid->differentiator = 0.0f;
    pid->prevMeasurement = 0.0f;

    pid->out = 0.0f;
    return pid;
}

inline void PIDController_SetTgt(PIDController *pid, float tgt){
    pid->tgt = tgt;
    pid->integrator = 0.0f;
    pid->prevError = 0.0f;
    pid->differentiator = 0.0f;
    pid->prevMeasurement = 0.0f;
    pid->out = 0.0f;
}

float PIDController_Update(PIDController *pid, float setpoint, float measurement)
{

    /*
     * Error signal
     */
    float error = setpoint - measurement;

    /*
     * Proportional
     */
    float proportional = pid->Kp * error;

    /*
     * Integral
     */
    pid->integrator = pid->integrator + 0.5f * pid->Ki * pid->T * (error + pid->prevError);

    /* Anti-wind-up via integrator clamping */
    if (pid->integrator > pid->limMaxInt)
    {

        pid->integrator = pid->limMaxInt;
    }
    else if (pid->integrator < pid->limMinInt)
    {

        pid->integrator = pid->limMinInt;
    }

    /*
     * Derivative (band-limited differentiator)
     */

    pid->differentiator = -(2.0f * pid->Kd * (measurement - pid->prevMeasurement) /* Note: derivative on measurement, therefore minus sign in front of equation! */
                            + (2.0f * pid->tau - pid->T) * pid->differentiator) /
                          (2.0f * pid->tau + pid->T);

    /*
     * Compute output and apply limits
     */
    pid->out = proportional + pid->integrator + pid->differentiator;

    if (pid->out > pid->limMax)
    {

        pid->out = pid->limMax;
    }
    else if (pid->out < pid->limMin)
    {

        pid->out = pid->limMin;
    }

    /* Store error and measurement for later use */
    pid->prevError = error;
    pid->prevMeasurement = measurement;

    // Control slave device if callback is available
    if (pid->s_callback != NULL) {
        pid->s_callback(pid->s_instance, pid->out);
    }

    /* Return controller output */
    return pid->out;
}

inline void PIDController_AssumeControl(PIDController *pid, void (*callback)(void *instance, float value), void *instance) {
    pid->s_callback = callback;
    pid->s_instance = instance;
}

inline void PIDController_ReleaseControl(PIDController *pid) {
    pid->s_callback = NULL;
    pid->s_instance = NULL;
}
