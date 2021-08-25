#include "pwm.h"
#include "timer.h"

#define TA1 27
#define TA0 13

static char state, timer;


void PWM_init(void) {
    
    timer = TIMER_getTimer();
    state = 0;
    
    TRISAbits.TRISA0 = 0; //output
    
    TIMER_resetTics(timer);
    PWM_On(); 
}

void PWM_motor(void) {
    switch(state) {
        case 0:
            if (TIMER_getTics(timer) >= TA1) {
                PWM_Off();
                TIMER_resetTics(timer);
                state = 1;
            }
            break;
        case 1:
            if (TIMER_getTics(timer) >= TA0) {
                PWM_On();
                TIMER_resetTics(timer);
                state = 0;
            }
            break;
        case 2: //openDoor
            PWM_Off();
            state = 4; // reposo
            break;
            
        case 3:
            TIMER_resetTics(timer);
            PWM_On();
            state = 0;
            break;
        case 4:
            break;
            
    }
}

void OpenDoor(void) {
    state = 2;
}

void CloseDoor(void) {
    state = 3;
}
