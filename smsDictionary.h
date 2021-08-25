/*
 * File:   smsDictionary.h
 * Author: xoselolo
 *
 * Created on 8 de abril de 2020, 18:49
 */
#ifndef _SMSDICTIONARY_H
#define _SMSDICTIONARY_H

typedef struct{
    unsigned char numValues;
    unsigned char values[5];
}DictionaryEntry;

/**
 * Procedure that initializes the structure
 */
void SMSDICTIONARY_init();

/**
 * Function called when a key on the keyboard is pressed
 * @param actualKey : key that has been pressed
 *                      between [1 - 12]
 * @param pressedTimes : how many times has been pressed
 * @return the char associated or 0 if pressedTimes has an invalid value
 */
unsigned char SMSDICTIONARY_getChar(char actualKey, char pressedTimes);

/**
 * Function that tells how many chars a key has
 * @param actualKey : the key
 * @return the number of chars that the key has or 0 if is not a valid key
 */
char SMSDICTIONARY_maxChars(char actualKey);

#endif //_SMSDICTIONARY_H
