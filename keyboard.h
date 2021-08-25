/* 
 * File:   keyboard.h
 * Author: xoselolo
 *
 * Created on 30 de abril de 2020, 10:40
 */

#ifndef KEYBOARD_H
#define	KEYBOARD_H

#include "timer.h"
#include "smsDictionary.h"


//Filas del teclado
#define F4 LATBbits.LATB9
#define F3 LATBbits.LATB8
#define F2 LATBbits.LATB7
#define F1 LATBbits.LATB6

 //Columnas    
#define C3 PORTAbits.RA4
#define C2 PORTAbits.RA3
#define C1 PORTAbits.RA2

/**
 * Motor del tad KEYBOARD
 * Pre: Haber inicializado el tad con la funcion KEYBOARD_init()
 * Post: Estado actual = Estado futuro (estado actual[t-1])
 */
void KEYBOARD_motor();

/**
 * Inicializa el tad
 * Pre: Ninguna
 * Post: El tad quedará inicializado
 */
void KEYBOARD_init();


// -------------- INTERFICIE N --------------------
/**
 * Indica si existe una nueva tecla a tratar
 * Pre: init()
 * Post: Sabremos si hay una tecla
 * @return
 *      0 - No hay tecla
 *      1 - Hay tecla
 */
char KEYBOARD_hayTecla();

/**
 * Devuelve el tipo de carácter del que se trata
 * Pre: hayTecla
 * Post: Tipo de tecla
 * @return 
 *      'M' - Menu
 *      'R' - Reset
 *      'N' - Nueva tecla
 *      'C' - Cambiar tecla previa
 */
char KEYBOARD_getTipo();

/**
 * Devuelve la tecla que ha sido apretada
 * Pre: hayTecla
 * Post: tecla apretada
 * @return 
 *      Tecla apretada
 */
char KEYBOARD_getTecla();

void KEYBOARD_setModo(char mode);

void KEYBOARD_setPreviousKey(char key);

char getFila();
char getColumna();

void erraseTipo(void);
#endif	/* KEYBOARD_H */

