/* 
 * File:   speaker.c
 * Author: xoselolo
 *
 * Created on 15 de mayo de 2020, 16:26
 */

#include <xc.h>

#include "speaker.h"

#define BEEP_TICS 1    
// Numero de veces que tiene que saltar el Timer0 
                        
// para lograr MEDIO PERIODO
#define BEEP_TIME 500   
// Numero de periodos (duracion) que tendra el "beep"
                        // y tambien la duracion (periodos) del descanso 
                        // entre "beeps"

// PIN 3 = RA1

static unsigned char state = 0;
static unsigned char num_beeps = 0; // indica las veces que quedan aun por sonar
static unsigned short time = 0; // numero de periodos que llevamos (a 1 o a 0)
static int timer_id = -1;

// ---------- PRIVATE TAD METHODS ----------
void start(){
    LATAbits.LATA1 = 1;
}
void stop(){
    LATAbits.LATA1 = 0;
}

// ---------- PUBLIC TAD METHODS ----------
void SPEAKER_init(){
    TRISAbits.TRISA1 = 0;
    state = 0;
    num_beeps = 0;
    time = 0;
    if(timer_id != -1){
        TIMER_closeTimer(timer_id);
    }
    timer_id = TIMER_getTimer();
    stop();
}

void SPEAKER_motor(){
    switch(state){
        case 0:
            if(num_beeps != 0){
                
                TIMER_resetTics(timer_id);
                time = 1;
                start(); // ON
                state = 1;
            }
            break;
        case 1:
            if(TIMER_getTics(timer_id) >= BEEP_TICS){
                TIMER_resetTics(timer_id);
                stop(); // OFF
                state = 2;
            }
            break;
        case 2:
            if(TIMER_getTics(timer_id) >= BEEP_TICS){
                if(time < BEEP_TIME){
                    TIMER_resetTics(timer_id);
                    time ++;
                    start(); // ON
                    state = 1;
                }else{
                    TIMER_resetTics(timer_id);
                    time = 0;
                   
                    state = 3;
                }
            }
            break;
        case 3:
            if(TIMER_getTics(timer_id) >= BEEP_TICS * 2){
                TIMER_resetTics(timer_id);
                time ++;
                state = 4;
            }
            break;
        case 4:
            if(time < BEEP_TIME){
                state = 3;
            }else{
                num_beeps --;
                state = 0;
            }
            break;
    }
}

void SPEAKER_beep(unsigned char beeps){
    num_beeps += beeps;
}

unsigned char SPEAKER_isBeeping(){
    if (num_beeps > 0) return 1;
    else return 0;
}
