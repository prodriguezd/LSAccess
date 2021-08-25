/*
 * File:   smsDictionary.c
 * Author: xoselolo
 *
 * Created on 8 de abril de 2020, 18:49
 */

#include "smsDictionary.h"

/**
 * dictionary:      matrix with character values for each key
 * previousKey:     index that indicates the last pressed key
 * actualKey:       index that indicates the actually pressed key
 * pressedTimes:    it indicates the number of time this key has been pressed
 */
static DictionaryEntry dictionary[12];

/**
 * Procedure that initializes the structure
 */
void SMSDICTIONARY_init(){
    
   char i;
    for (i = 0; i < 12; i++) {
        switch (i){
            case 0:
                dictionary[i].numValues = 1;
                dictionary[i].values[0] = '1';
                break;
            case 1:
                dictionary[i].numValues = 4;
                dictionary[i].values[0] = 'A';
                dictionary[i].values[1] = 'B';
                dictionary[i].values[2] = 'C';
                dictionary[i].values[3] = '2';
                
                break;
            case 2:
                dictionary[i].numValues = 4;
                dictionary[i].values[0] = 'D';
                dictionary[i].values[1] = 'E';
                dictionary[i].values[2] = 'F';
                dictionary[i].values[3] = '3';
                break;
            case 3:
                dictionary[i].numValues = 4;
                dictionary[i].values[0] = 'G';
                dictionary[i].values[1] = 'H';
                dictionary[i].values[2] = 'I';
                dictionary[i].values[3] = '4';
                break;
            case 4:
                dictionary[i].numValues = 4;
                dictionary[i].values[0] = 'J';
                dictionary[i].values[1] = 'K';
                dictionary[i].values[2] = 'L';
                dictionary[i].values[3] = '5';
                break;
            case 5:
                dictionary[i].numValues = 4;
                dictionary[i].values[0] = 'M';
                dictionary[i].values[1] = 'N';
                dictionary[i].values[2] = 'O';
                dictionary[i].values[3] = '6';
                break;
            case 6:
                dictionary[i].numValues = 5;
                dictionary[i].values[0] = 'P';
                dictionary[i].values[1] = 'Q';
                dictionary[i].values[2] = 'R';
                dictionary[i].values[3] = 'S';
                dictionary[i].values[4] = '7';
                break;
            case 7:
                dictionary[i].numValues = 4;
                dictionary[i].values[0] = 'T';
                dictionary[i].values[1] = 'U';
                dictionary[i].values[2] = 'V';
                dictionary[i].values[3] = '8';
                break;
            case 8:
                dictionary[i].numValues = 5;
                dictionary[i].values[0] = 'W';
                dictionary[i].values[1] = 'X';
                dictionary[i].values[2] = 'Y';
                dictionary[i].values[3] = 'Z';
                dictionary[i].values[4] = '9';
                break;
            case 9:
                dictionary[i].numValues = 1;
                dictionary[i].values[0] = '*'; // Nunca accedera
                break;
            case 10:
                dictionary[i].numValues = 1;
                dictionary[i].values[0] = '0';
                break;
            case 11:
                dictionary[i].numValues = 1;
                dictionary[i].values[0] = '#'; // Nunca accedera
                break;
        }
    }
}

/**
 * Function called when a key on the keyboard is pressed
 * @param actualKey : key that has been pressed
 *                      between [1 - 12]
 * @param pressedTimes : how many times has been pressed
 * @return the char associated
 */
unsigned char SMSDICTIONARY_getChar(char actualKey, char pressedTimes){
    /*if(pressedTimes > dictionary[actualKey - 1].numValues){
        return 0;
    }else{*/
        return dictionary[actualKey - 1].values[pressedTimes - 1];
    //}
}

/**
 * Function that tells how many chars a key has
 * @param actualKey : the key
 * @return the number of chars that the key has or 0 if is not a valid key
 */
char SMSDICTIONARY_maxChars(char actualKey){
    return dictionary[actualKey - 1].numValues;
}


