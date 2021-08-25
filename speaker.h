/* 
 * File:   speaker.h
 * Author: xoselolo
 *
 * Created on 15 de mayo de 2020, 16:26
 */

#ifndef SPEAKER_H
#define	SPEAKER_H

#include "timer.h"

void SPEAKER_init();
void SPEAKER_motor();
void SPEAKER_beep(unsigned char beeps);
unsigned char SPEAKER_isBeeping();

#endif	/* SPEAKER_H */