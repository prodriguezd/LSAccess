#ifndef PWM_H_
#define PWM_H_

#include <xc.h>

#define PWM LATAbits.LATA0

#define PWM_On() LATAbits.LATA0 = 1
#define PWM_Off() LATAbits.LATA0 = 0

void PWM_init(void);
void PWM_motor(void);
void OpenDoor(void);
void CloseDoor(void);
#endif

