#ifndef _CONTROLLER_H_
#define _CONTROLLER_H_

#include "ioticat.h"
#include "keyboard.h"
#include "pwm.h"
#include "speaker.h"
#include "timer.h"
#include "SIO.h"
#include "LCD.h"


void initAccessController(void);
//pre: ioticat sio pwm timer speaker inicializados
//post:--

void motorAccessController(void);
//pre: tad inicializado
//post:--

void initLCD(void);
//pre: timer inicialitzat
//post: --

void motorLCD(void);
//pre: tad inicialitzat
//post:--

void motorTime(void);
//pre: timer inicializado, acc inicializado
//poost: --

char isReset(void);
//avisa al main que hay que volver a hacer inits

#endif