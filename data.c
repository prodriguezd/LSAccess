#include "data.h"

static char muestraSio, state;
static char uid[10], tarjeta[5];

void initData(void) {
    muestraSio = 0;
    state = 0;
}

void motorData(void) {
    switch(state) {
        case 1:
            uid[0] = tarjeta[0] >> 4;
            uid[0] = uid[0] & 0x0F;
            state = 2;
            break;
            
        case 2:
            if (uid[0] <= 9 && uid[0] >= 0) uid[0] += '0';
            else uid[0] += 55;
            state = 3;
            break;
            
        case 3:
            uid[1] = tarjeta[0] & 0x0F;
            state = 4;
            break;
            
        case 4:
            if (uid[1] <= 9 && uid[1] >= 0) uid[1] += '0';
            else uid[1] += 55;
            state = 5;
            break;
            
        case 5:
            uid[2] = tarjeta[1] >> 4;
            uid[2] = uid[2] & 0x0F;
            state = 6;
            break;
            
        case 6:
            if (uid[2] <= 9 && uid[2] >= 0) uid[2] += '0';
            else uid[2] += 55;
            state = 7;
            break;
            
        case 7:
            uid[3] = tarjeta[1] & 0x0F;
            state = 8;
            break;
            
        case 8:
            if (uid[3] <= 9 && uid[3] >= 0) uid[3] += '0';
            else uid[3] += 55;
            state = 9;
            break;
            
        case 9:
            uid[4] = tarjeta[2] >> 4;
            uid[4] = uid[4] & 0x0F;
            state = 10;
            break;
            
        case 10:
            if (uid[4] <= 9 && uid[4] >= 0) uid[4] += '0';
            else uid[4] += 55;
            state = 11;
            break;
            
        case 11:
            uid[5] = tarjeta[2] & 0x0F;
            state = 12;
            break;
            
        case 12:
            if (uid[5] <= 9 && uid[5] >= 0) uid[5] += '0';
            else uid[5] += 55;
            state = 13;
            break;
            
        case 13:
            uid[6] = tarjeta[3] >> 4;
            uid[6] = uid[6] & 0x0F;
            state = 14;
            break;
            
        case 14:
            if (uid[6] <= 9 && uid[6] >= 0) uid[6] += '0';
            else uid[6] += 55;
            state = 15;
            break;
            
        case 15:
            uid[7] = tarjeta[3] & 0x0F;
            state = 16;
            break;
            
        case 16:
            if (uid[7] <= 9 && uid[7] >= 0) uid[7] += '0';
            else uid[7] += 55;
            state = 17;
            break;
            
        case 17:
            uid[8] = tarjeta[4] >> 4;
            uid[8] = uid[8] & 0x0F;
            state = 18;
            break;
            
        case 18:
            if (uid[8] <= 9 && uid[8] >= 0) uid[8] += '0';
            else uid[8] += 55;
            state = 19;
            break;
            
        case 19:
            uid[9] = tarjeta[4] & 0x0F;
            state = 20;
            break;
            
        case 20:
            if (uid[9] <= 9 && uid[9] >= 0) uid[9] += '0';
            else uid[9] += 55;
            state = 21;
            break;
            
        case 21:
            SiSendChar(uid[0]);
            SiSendChar(uid[1]);
            SiSendChar('-');
            state = 22;
            break;
            
        case 22:
            SiSendChar(uid[2]);
            SiSendChar(uid[3]);
            SiSendChar('-');
            state = 23;
            break;
        
        case 23:
            SiSendChar(uid[4]);
            SiSendChar(uid[5]);
            SiSendChar('-');
            state = 24;
            break;
            
        case 24:
            SiSendChar(uid[6]);
            SiSendChar(uid[7]);
            SiSendChar('-');
            state = 25;
            break;
            
        case 25:
            SiSendChar(uid[8]);
            SiSendChar(uid[9]);
            state = 26;
            break;
            
        case 26:
            SiPuts("\n\0");
            state = 0;
            break;
    }
}

void muestraSIO(char * t) {
    tarjeta[0] = t[0];
    tarjeta[1] = t[1];
    tarjeta[2] = t[2];
    tarjeta[3] = t[3];
    tarjeta[4] = t[4];
    SiPuts("\n\rHan pasado tarjeta! El uid de la tarjeta es -> ");
    state = 1;
}