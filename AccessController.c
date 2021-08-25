#include "AccessController.h"


#define MAXSEGONS 6000
#define MAXCHARS 10
#define SEGUNDO 1000

static unsigned char index,  stateTime, state, aux, cont, keySio, keyboard, lcdmenu, charAux;
static unsigned char seg, horD, horU, minD, minU, reset, stateAux, num;
static unsigned char opcio, pintado, stateEstadisticas;
static unsigned char tempUid[10], stringIn[3], temps[2], horaNueva[4], offset[2];
static unsigned char cuantosIn, registrados, puerta, accesDenegat;
static unsigned int timer, timerAux;
static short unsigned minutesPermanence, minutesUserIn;
static User user;


void addChar(unsigned char aux);
void limpiaSegundaLinea(void);

void myItoa(int numero) {
    stringIn[0] = (char)(numero / 100);
    numero = numero - (stringIn[0] * 100);
    stringIn[1] = (char) (numero / 10);
    numero = numero - (stringIn[1] * 10);
    stringIn[2] = numero + '0';
    stringIn[1] += '0';
    stringIn[0] += '0';
}

void myCoopItoa(unsigned char nextState) {
    switch(stateAux) {
        case 0:
            charAux = num / 100;
            stateAux = 1;
            break;
        case 1:
            stringIn[0] = charAux;
            stateAux = 2;
            break;
        case 2:
            charAux = stringIn[0] * 100;
            stateAux = 3;
            break;
        case 3:
            num = num - charAux;
            stateAux = 4;
            break;
        case 4:
            charAux = num / 10;
            stringIn[1] = charAux;
            stateAux = 5;
            break;
        case 5:
            charAux = stringIn[1] * 10;
            stateAux = 6;
            break;
        case 6:
            num = num - charAux;
            stateAux = 7;
            break;
        case 7:
            stringIn[2] = num;
            stateAux = 8;
            break;
        case 8:
            stringIn[0] += '0';
            stringIn[1] += '0';
            stringIn[2] += '0';
            stateAux = 9;
            break;
        case 9:
            stateAux = 0;
            if (opcio != 5) state = nextState;
            else stateEstadisticas = nextState;
            break;
    }
}

/*TIEMPO*/
void motorTime(void) {
    
    
    if (TIMER_getTics(timer) > 500) {  //1 min es medio minuto ahora
        seg++;
        if (seg == 60) {
            minU++; 
            
            seg = 0;
        }
        if (minU == 9) {
            minD++;
            
            minU = 0;
            
        }
        if (minD == 6) {
            horU++;
            minD = 0;
        }
        if (horU == 4 && horD == 2) {
            horU = 0;
            horD = 0;
        }
        else if (horU == 9) {
            horD++;
            horU = 0;
        }
        TIMER_resetTics(timer);
    }  
    
}

void initAccessController(void) {
    timer = TIMER_getTimer();
    timerAux = TIMER_getTimer();
    
    seg = horD = horU = minD = minU = 0;
    opcio = state = stateTime = pintado = 0;
    
    lcdmenu = 0;
    index = 0;
    
    stateAux = 0;
    
    cuantosIn = IOTICAT_getCuantosIn();
    registrados = IOTICAT_getCuantos();
    puerta = 0;
    accesDenegat = 0;
    
    KEYBOARD_setModo(0);
    
    stateEstadisticas = 0;
    
    user.name[0] = user.name[2] = user.name[3] = user.name[4] = user.name[5] = user.name[6] = user.name[7] = user.name[8] = user.name[9] = ' ';
    
    myItoa((int)cuantosIn);
    
    TIMER_resetTics(timer);
    reset = 0;
}

void Menu(void) {
    SiPutsCooperatiu("\n\r\n\n--------------\r\n\0");
    SiPutsCooperatiu("\rMenu principal\r\n\0");
    SiPutsCooperatiu("\r--------------\r\n\0");
    SiPutsCooperatiu("\r\nTria una opcio:\r\n\0");
    SiPutsCooperatiu("\r\n\0");
    SiPutsCooperatiu("1. Registrar nou usuari\r\n\0");
    SiPutsCooperatiu("2. Eliminar usuari\r\n\0");
    SiPutsCooperatiu("3. Modificar hora del sistema\r\n\0");
    SiPutsCooperatiu("4. Mostrar estadistiques\r\n\0");
    SiPutsCooperatiu("\r\n");
    SiPutsCooperatiu("Opcio:\r\n\r");
}

void transformaUID(void) {
    switch(stateAux) {
        case 0:
            
            if (tempUid[0] >= '0' && tempUid[0] <= '9') tempUid[0] -= '0';
            else tempUid[0] -= 55;
            stateAux = 1;
            break;
            
        case 1:
            
            user.uid[0] = tempUid[0] << 4;
            
            stateAux = 2;
            break;
            
        case 2:
            user.uid[0] = user.uid[0] & 0xF0;
            SiSendChar(user.uid[0]);
            stateAux = 3;
            break;
            
        case 3:
            if (tempUid[1] >= '0' && tempUid[1] <= '9') tempUid[1] -= '0';
            else tempUid[1] -= 55;
            stateAux = 4;
            break;
            
        case 4:
            user.uid[0] = user.uid[0] | tempUid[1];
            
            
            stateAux = 5;
            break;
            
        case 5:
            if (tempUid[2] >= '0' && tempUid[2] <= '9') tempUid[2] -= '0';
            else tempUid[2] -= 55;
            stateAux = 6;
            break;
            
        case 6:
            user.uid[1] = tempUid[2] << 4;
            stateAux = 7;
            break;
            
        case 7:
            user.uid[1] = user.uid[1] & 0xF0;
            stateAux = 8;
            break;
            
        case 8:
            if (tempUid[3] >= '0' && tempUid[3] <= '9') tempUid[3] -= '0';
            else tempUid[3] -= 55;
            stateAux = 9;
            break;
            
        case 9:
            user.uid[1] = user.uid[1] | tempUid[3];
            stateAux = 10;
            break;
            
        case 10:
            if (tempUid[4] >= '0' && tempUid[4] <= '9') tempUid[4] -= '0';
            else tempUid[4] -= 55;
            stateAux = 11;
            break;
            
        case 11:
            user.uid[2] = tempUid[4] << 4;
            stateAux = 12;
            break;
            
        case 12:
            user.uid[2] = user.uid[2] & 0xF0;
            stateAux = 13;
            break;
            
        case 13:
            if (tempUid[5] >= '0' && tempUid[5] <= '9') tempUid[5] -= '0';
            else tempUid[5] -= 55;
            stateAux = 14;
            break;
            
        case 14:
            user.uid[2] = user.uid[2] | tempUid[5];
            stateAux = 15;
            break;
            
        case 15:
            if (tempUid[6] >= '0' && tempUid[6] <= '9') tempUid[6] -= '0';
            else tempUid[6] -= 55;
            stateAux = 16;
            break;
            
        case 16:
            user.uid[3] = tempUid[6] << 4;
            stateAux = 17;
            break;
            
        case 17:
            user.uid[3] = user.uid[3] & 0xF0;
            stateAux = 18;
            break;
            
        case 18:
            if (tempUid[7] >= '0' && tempUid[7] <= '9') tempUid[7] -= '0';
            else tempUid[7] -= 55;
            stateAux = 19;
            break;
            
        case 19:
            user.uid[3] = user.uid[3] | tempUid[7];
            stateAux = 20;
            break;
            
        case 20:
            if (tempUid[8] >= '0' && tempUid[8] <= '9') tempUid[8] -= '0';
            else tempUid[8] -= 55;
            stateAux = 21;
            break;
            
        case 21:
            user.uid[4] = tempUid[8] << 4;
            stateAux = 22;
            break;
            
        case 22:
            user.uid[4] = user.uid[4] & 0xF0;
            stateAux = 23;
            break;
            
        case 23:
            if (tempUid[9] >= '0' && tempUid[9] <= '9') tempUid[9] -= '0';
            else tempUid[9] -= 55;
            stateAux = 24;
            break;
            
        case 24:
            user.uid[4] = user.uid[4] | tempUid[9];
            stateAux = 25;
            break;
            
        case 25:
            stateAux = 0;
            state = 6;
            break;
    }
}

void destransformaUID(char nextState) {
    switch(stateAux) {
        case 0:
            tempUid[0] = user.uid[0] >> 4;
            
            stateAux = 1;
            break;
            
        case 1:
            tempUid[0] = tempUid[0] & 0x0F;
            
            if (tempUid[0] >= 0 && tempUid[0] <= 9) tempUid[0] += '0';
            else tempUid[0] += 55;
            
            SiSendChar(tempUid[0]);
            stateAux = 2;
            break;
            
        case 2:
            tempUid[1] = user.uid[0] & 0x0F;
            
            if (tempUid[1] >= 0 && tempUid[1] <= 9) tempUid[1] += '0';
            else tempUid[1] += 55;
            
            SiSendChar(tempUid[1]);
            stateAux = 3;
            break;
            
        case 3:
            SiSendChar('-');
            tempUid[2] = user.uid[1] >> 4;
            
            stateAux = 4;
            break;
            
        case 4:
            tempUid[2] = tempUid[2] & 0x0F;
            
            if (tempUid[2] >= 0 && tempUid[2] <= 9) tempUid[2] += '0';
            else tempUid[2] += 55;
            
            SiSendChar(tempUid[2]);
            stateAux = 5;
            break;
            
        case 5:
            tempUid[3] = user.uid[1] & 0x0F;
            
            if (tempUid[3] >= 0 && tempUid[3] <= 9) tempUid[3] += '0';
            else tempUid[3] += 55;
            
            SiSendChar(tempUid[3]);
            stateAux = 6;
            break;
            
        case 6:
            SiSendChar('-');
            tempUid[4] = user.uid[2] >> 4;
            
            stateAux = 7;
            break;
            
        case 7:
            tempUid[4] = tempUid[4] & 0x0F;
            
            if (tempUid[4] >= 0 && tempUid[4] <= 9) tempUid[4] += '0';
            else tempUid[4] += 55;
            
            SiSendChar(tempUid[4]);
            stateAux = 8;
            break;
            
        case 8:
            tempUid[5] = user.uid[2] & 0x0F;
            
            if (tempUid[5] >= 0 && tempUid[5] <= 9) tempUid[5] += '0';
            else tempUid[5] += 55;
            
            SiSendChar(tempUid[5]);
            stateAux = 9;
            break;
            
        case 9:
            SiSendChar('-');
            tempUid[6] = user.uid[3] >> 4;
            
            stateAux = 10;
            break;
            
        case 10:
            tempUid[6] = tempUid[6] & 0x0F;
            
            if (tempUid[6] >= 0 && tempUid[6] <= 9) tempUid[6] += '0';
            else tempUid[6] += 55;
            
            SiSendChar(tempUid[6]);
            stateAux = 11;
            break;
            
        case 11:
            tempUid[7] = user.uid[3] & 0x0F;
            
            if (tempUid[7] >= 0 && tempUid[7] <= 9) tempUid[7] += '0';
            else tempUid[7] += 55;
            
            SiSendChar(tempUid[7]);
            stateAux = 12;
            break;
            
        case 12:
            SiSendChar('-');
            tempUid[8] = user.uid[4] >> 4;
            
            stateAux = 13;
            break;
            
        case 13:
            tempUid[8] = tempUid[8] & 0x0F;
            
            if (tempUid[8] >= 0 && tempUid[8] <= 9) tempUid[8] += '0';
            else tempUid[8] += 55;
            
            SiSendChar(tempUid[8]);
            stateAux = 14;
            break;
            
        case 14:
            tempUid[9] = user.uid[4] & 0x0F;
            
            if (tempUid[9] >= 0 && tempUid[9] <= 9) tempUid[9] += '0';
            else tempUid[9] += 55;
            SiSendChar(tempUid[9]);
            stateAux = 15;
            break;
            
        case 15:
            stateAux = 0;
            stateEstadisticas = nextState;
            break;
    }
}

void getRandom(void) {
    switch(stateEstadisticas) {
        case 0:
            user.name[0] = 'R';
            user.name[1] = 'a';
            user.name[2] = 'n';
            stateEstadisticas = 1;
            break;
        case 1:
            user.name[3] = 'd';
            user.name[4] = 'o';
            user.name[5] = 'm';
            stateEstadisticas = 2;
            break;
        case 2:
            user.name[6] = '!';
            user.name[7] = ' ';
            user.name[8] = ' ';
            user.name[9] = ' ';
            stateEstadisticas = 3;
            break;
        case 3:
            user.length = 7;
            cont = 5;
            stateEstadisticas = 0;
            stateAux = 9;
            break;
    }
}

void puedeEntrar(void) {
    switch(stateAux) {
        case 0:
            if (aux > MAX_REGISTERED_USERS) {
                stateAux = 11;
            }
            else {
                stateAux = 1;
            }
        break;
        case 1:
            if (aux >= cuantosIn) {
                //entrar
                stateAux = 2;
            }
            else {
                //salir
                stateAux = 5;
            }
        break;
        case 2:
            if (temps[0] >= user.in_min_hour && temps[0] <= user.in_max_hour) {
                //se encuentra dento del rangode tiempo para entrar
                stateAux = 3;
            }
            else {
                cont = 2;
                stateAux = 9;
                
            }
        break;
        case 3:
            if (temps[0] == user.in_min_hour) {
                if (temps[1] >= user.in_min_minute) {
                    cont = 1;
                }
                else {
                    cont = 2;
                }
            }
            else if (temps[0] == user.in_max_hour) {
                if (temps[1] <= user.in_max_minute) {
                    cont = 1;
                }
                else {
                    cont = 2;
                }
            }
            else {
                cont = 1;
            }
            stateAux = 4;
        break;
        case 4:
            if (cont == 1) {
                if (cuantosIn > 100) cont = 4;
                else {
                    user.in_real_hour = temps[0];
                    user.in_real_minute = temps[1];
                    cuantosIn++;
                }
            }
            stateAux = 9;
        break;

        case 5:
            minutesUserIn = user.in_real_hour - temps[0];
            minutesPermanence = user.min_permanence_time.value[0] * 100;
            stateAux = 6;
        break;

        case 6:
            minutesUserIn *= 60;
            stateAux = 7;
        break;

        case 7:
            if (temps[1] > user.in_real_minute) {
                minutesUserIn = minutesUserIn + (temps[1] - user.in_real_minute);
            }
            else {
                minutesUserIn = minutesUserIn + (user.in_real_minute - temps[1]);
            }
            minutesPermanence += (user.min_permanence_time.value[1] * 10);
            stateAux = 10;
        break;
        
        case 10:
            minutesPermanence += (user.min_permanence_time.value[2]);
            stateAux = 8;
            break;

        case 8:
            if (minutesUserIn >= minutesPermanence) { //los minutos dentro comparado con los min minimos
                cont = 1;
                cuantosIn--;
            }
            else {
                cont = 2;
            }
            stateAux = 9;
        break;

        case 9:
            lcdmenu = 3;
            stateAux = 0;
            state = 10;
        break;
        
        case 11:
            if (aux == BOSS_INDEX) {
                cont = 1;
                stateAux = 12;
            }
            else {
                getRandom();
                //cont = 5;
            }
            break;
            
        case 12:
            if (IOTICAT_isBossIn() == 1) {
                //boss quiere salir!
                cuantosIn--;
                IOTICAT_signOutBoss();
                stateAux = 9;
            }
            else {
                //quiere entrar!
                cuantosIn++;
                IOTICAT_signInBoss(temps[0], temps[1]);
                stateAux = 9;
            }
            break;
    }
}

char isReset(void) {
    if (reset == 1) {
        reset = 0;
        return 1;
    }
    return 0;
}

void usuarioPorDefecto(void) {
    switch(stateAux) {
        case 0:
            user.in_min_hour = horD * 10;
            user.in_min_minute = minD * 10;
            stateAux = 1;
            break;
        case 1:
            user.in_min_hour += horU;
            user.in_max_hour = user.in_min_hour + 1;
            stateAux = 2;
            break;
        case 2:
            user.in_min_minute += minU;
            user.in_max_minute = user.in_min_minute;
            stateAux = 3;
            break;
        case 3:
            user.min_permanence_time.value[0] = 0;
            user.min_permanence_time.value[1] = 6;
            user.min_permanence_time.value[2] = 9;
            stateAux = 4;
            break;
        case 4:
            stateAux = 0;
            state = 22;
            registrados++;
            break;
    }
}

void registraUserSIO(void) {
    switch(stateAux) {
        case 0:
            SiPutsCooperatiu("\n\rIntroduiex la hora minima d'entrada: [hh:mm] ");
            stateAux = 1;
            break;
        case 1:
            if (cont < 5) {
                if (SiCharAvail()) {
                    aux = SiGetChar() - '0';
                    stateAux = 2;
                }
            }
            else stateAux = 3;
            break;
        case 2:
            switch(cont) {
                case 0:
                    user.in_min_hour = aux * 10;
                    break;
                case 1:
                    user.in_min_hour += aux;
                    break;
                case 3:
                    user.in_min_minute = aux * 10;
                    break;
                case 4:
                    user.in_min_minute += aux;
                    break;
            }
            cont++;
            stateAux = 1;
            SiSendChar(aux + '0');
            break;
        case 3:
            cont = 0;
            SiPutsCooperatiu("\0");
            SiPutsCooperatiu("\n\rIntroduiex la hora maxima d'entrada: [hh:mm] ");
            stateAux = 4;
            break;
        case 4:
            if (cont < 5) {
                if (SiCharAvail()) {
                    aux = SiGetChar() - '0';
                    stateAux = 5;
                }
            }
            else stateAux = 6;
            break;
        case 5:
            switch(cont) {
                case 0:
                    user.in_max_hour = aux * 10;
                    break;
                case 1:
                    user.in_max_hour += aux;
                    break;
                case 3:
                    user.in_max_minute = aux * 10;
                    break;
                case 4:
                    user.in_max_minute += aux;
                    break;
            }
            cont++;
            stateAux = 4;
            SiSendChar(aux + '0');
            break;
        case 6:
            cont =0;
            SiPutsCooperatiu("\0");
            SiPutsCooperatiu("\n\rIntroduiex els minuts minims de permanencia: [XXX] ");
            stateAux = 7;
            break;
        case 7:
            if (cont < 3) {
                if (SiCharAvail()) {
                    aux = SiGetChar() - '0';
                    stateAux = 8;
                }
            }
            else {
                SiPutsCooperatiu("\0");
                stateAux = 9;
            }
            break;
            
        case 8:
            user.min_permanence_time.value[cont++] = aux;
            SiSendChar(aux + '0');
            stateAux = 7;
            break;
            
        case 9:
            stateAux = 0;
            registrados++;
            state = 22;
            break;
    }
}

void cambiaHoraSistema(void) {
    switch(stateAux) {
        case 0:
            SiPuts("\n\n\rCanvi de hora del sistema\n\0");
            stateAux = 1;
            break;
        case 1:
            SiPuts("\r\tIntrodueix la nova hora: [hh:mm] ");
            stateAux = 2;
            break;
        case 2:
            if (cont < 5) {
                if (SiCharAvail()) {
                    aux = SiGetChar() - '0';
                    stateAux = 3;
                }
            }
            else stateAux = 4;
            break;
        case 3:
            switch(cont) {
                case 0:
                    horaNueva[cont] = aux;
                    offset[0] = aux * 10;
                    break;
                case 1:
                    horaNueva[cont] = aux;
                    offset[0] += aux;
                    break;
                case 3:
                    horaNueva[cont -1] = aux;
                    offset[1] = aux * 10;
                    break;
                case 4:
                    horaNueva[cont -1] = aux;
                    offset[1] += aux;
                    break;
            }
            cont++;
            stateAux = 2;
            SiSendChar(aux + '0');
            break;
            
        case 4:
            temps[0] = horD * 10;
            stateAux = 5;
            break;
            
        case 5:
            
            temps[1] = minD * 10;
            stateAux = 6;
            break;
            
        case 6:
            temps[0] += horU;
            temps[1] += minU;
            stateAux = 7;
            break;
            
        case 7:
            if (offset[0] < temps[0]) {
                aux = 24 - temps[0];
                offset[0] += aux;
            }
            else if (offset[0] > temps[0]){
                offset[0] -= temps[0];
            }
            else {
                offset[0] = 0;
            }
            stateAux = 8;
            break;
            
        case 8:
            if (offset[1] < temps[1]) {
                aux = 60 - temps[1];
                offset[1] += aux;
            }
            else if (offset[1] > temps[1]) {
                offset[1] -= temps[1];
            }
            else {
                offset[1] = 0;
            }
            stateAux = 9;
            break;
            
        case 9:
            SiPuts("\0");
            SiPuts("\n\rllego\n\0");
            stateAux = 0;
            state = 25;
            break;
    }
}

void muestraEstadisticas(void) {
    switch(stateEstadisticas) {
        case 0:
            SiPuts("\n\n\rPanell d'estadistiques: \n\n\0");
            SiPuts("\r\tUsuaris dins de l'IoTiCat: ");
            stateEstadisticas = 1;
            break;
            
        case 1:
            if (cuantosIn > 99) {
                SiSendChar(stringIn[0]);
            }
            stateEstadisticas = 2;
            break;
            
        case 2:
            if (cuantosIn > 9) {
                SiSendChar(stringIn[1]);
            }
            SiSendChar(stringIn[2]);
            stateEstadisticas = 3;
            break;
        //mostrados cuantos in del ioticat
        //vamos a mostrar los usuarios...
        case 3:
            SiPuts("\n\n\0");
            stateEstadisticas = 4;
            break;
            
        case 4:
            if (cont < cuantosIn && cont < 10) {
                SiPuts("\r\t\t");
                
                IOTICAT_getUserByIndex(cont, &user);
                stateEstadisticas = 5;
            }
            else {
                if (cuantosIn > 10) {
                    SiPuts("\r\t\t\t...\n\0");
                }
                stateEstadisticas = 17;
            }
            break;
        case 5:
            if (IOTICAT_isDone() == 1) {
                if (cont < 10) SiSendChar(cont + '0' + 1);
                else SiPuts("10");
                SiPuts(". ");
                stateEstadisticas = 6;
            }
            break;
        case 6:
            destransformaUID(7);
            break;
            
        case 7:
            SiPuts(" - ");
            user.name[user.length] = '\0';
            SiPuts(user.name);
            stateEstadisticas = 8;
            break;
            
        case 8:
            SiPuts(" - ");
            temps[0] = horD * 10;
            stateEstadisticas = 9;
            break;
            
        case 9:
            temps[1] = minD * 10;
            temps[0] += horU;
            stateEstadisticas = 10;
            break;
            
        case 10:
            temps[1] += minU;
            if (temps[0] < user.in_real_hour) {
                offset[0] = 24 + temps[0];
                offset[0] -= user.in_real_hour;
            }
            else {
                offset[0] = temps[0] - user.in_real_hour;
            }
            stateEstadisticas = 11;
            break;
            
        case 11:
            offset[0] *= 60;
            stateEstadisticas = 12;
            break;
            
        case 12:
            if (temps[1] < user.in_real_minute) {
                offset[1] = 60 + temps[1];
                offset[1] -= user.in_real_minute;
            }
            else {
                offset[1] = temps[1] - user.in_real_minute;
            }
            stateEstadisticas = 13;
            break;
            
        case 13:
            offset[0] += offset[1];
            num = offset[0];
            stateEstadisticas = 14;
            break;
            
        case 14:
            myCoopItoa(15);
            break;
            
        case 15:
            if (offset[0] > 99) {
                SiSendChar(stringIn[0]);
            }
            if (offset[0] > 9) {
                SiSendChar(stringIn[1]);
            }
            SiSendChar(stringIn[2]);
            stateEstadisticas = 16;
            break;
            
        case 16:
            SiPuts("\n\0");
            cont++;
            stateEstadisticas = 4;
            break;
            
        case 17:
            SiPuts("\r\tUsuaris registrats al sistema: ");
            num = registrados;
            stateEstadisticas = 18;
            break;
            
        case 18:
            myCoopItoa(50);
            break;
            
        case 19:
            SiPuts("\n\n\0");
            if (registrados > 14) {
                aux = 3;
            }
            cont = 0;
            stateEstadisticas = 20;
            break;
            
        case 20:
            if (cont < 10 && cont < registrados) {
                SiPuts("\r\t\t");
                
                IOTICAT_getUserByIndex(aux, &user);
                stateEstadisticas = 21;
            }
            else {
                if (registrados > 10) {
                    SiPuts("\r\t\t\t...\n\0");
                }
                stateEstadisticas = 38;
            }
            break;
            
        case 21:
            if (IOTICAT_isDone() == 1) {
                SiSendChar(cont + '0' + 1);
                SiPuts(". ");
                stateEstadisticas = 22;
            }
            break;
            
        case 22:
            destransformaUID(23);
            break;
            
        case 23:
            SiPuts(" - ");
            user.name[user.length] = '\0';
            SiPuts(user.name);
            stateEstadisticas = 24;
            break;
            
        case 24:
            SiPuts(" - ");
            num = user.in_min_hour;
            stateEstadisticas = 25;
            break;
            
        case 25:
            myCoopItoa(26);
            break;
            
        case 26:
            SiSendChar(stringIn[1]);
            SiSendChar(stringIn[2]);
            SiPuts(":");
            stateEstadisticas = 27;
            break;
            
        case 27:
            num = user.in_min_minute;
            cont++;
            stateEstadisticas = 28;
            break;
            
        case 28:
            myCoopItoa(29);
            break;
            
        case 29:
            SiSendChar(stringIn[1]);
            SiSendChar(stringIn[2]);
            SiPuts(" - ");
            stateEstadisticas = 30;
            break;
            
        case 30:
            num = user.in_max_hour;
            aux++;
            stateEstadisticas = 31;
            break;
            
        case 31:
            myCoopItoa(32);
            break;
            
        case 32:
            SiSendChar(stringIn[1]);
            SiSendChar(stringIn[2]);
            SiPuts(":");
            stateEstadisticas = 33;
            break;
            
        case 33:
            num = user.in_max_minute;
            stateEstadisticas = 34;
            break;
            
        case 34:
            myCoopItoa(35);
            break;
            
        case 35:
            SiSendChar(stringIn[1]);
            SiSendChar(stringIn[2]);
            SiPuts(" - ");
            stateEstadisticas = 36;
            break;
            
        case 36:
            if (user.min_permanence_time.value[0] != 0) {
                SiSendChar(user.min_permanence_time.value[0] + '0');
                SiSendChar(user.min_permanence_time.value[1] + '0');
            }
            else {
                if (user.min_permanence_time.value[1] != 0) {
                    SiSendChar(user.min_permanence_time.value[1] + '0');
                }
            }
            stateEstadisticas = 37;
            break;
            
        case 37:
            SiSendChar(user.min_permanence_time.value[2] + '0');
            SiPuts("\n\0");
            stateEstadisticas = 20;
            break;
            
        case 38:
            SiPuts("\n\n\0");
            SiPuts("\r\tUsuaris amb acces denegat: ");
            num = accesDenegat;
            stateEstadisticas = 39;
            break;
            
        case 39:
            myCoopItoa(40);
            break;
            
        case 40:
            if (accesDenegat > 99) {
                SiSendChar(stringIn[0]);
            } 
            if (accesDenegat > 9) {
                SiSendChar(stringIn[1]);
            }
            SiSendChar(stringIn[2]);
            
            stateEstadisticas = 41;
            break;
            
        case 41:
            SiPuts("\n\0");
            SiPuts("\r\tVegades que s'ha obert la porta: ");
            num = puerta;
            stateEstadisticas = 42;
            break;
            
        case 42:
            myCoopItoa(43);
            break;
            
        case 43:
            if (puerta > 99) {
                SiSendChar(stringIn[0]);
            } 
            if (puerta > 9) {
                SiSendChar(stringIn[1]);
            }
            SiSendChar(stringIn[2]);
            
            stateEstadisticas = 44;
            break;
            
        case 44:
            SiPuts("\n\0");
            SiPuts("\r\tHora del sistema: ");
            stateEstadisticas = 45;
            break;
            
        case 45:
            SiSendChar(horD + '0');
            SiSendChar(horU + '0');
            SiSendChar(':');
            
            stateEstadisticas = 46;
            break;
            
        case 46:
            SiSendChar(minD + '0');
            SiSendChar(minU + '0');
            SiPuts("\n\n\n\0");
            stateEstadisticas = 47;
            break;
            
        case 47:
            stateEstadisticas = 0;
            state = 30;
            break; 
            
        case 50:
            if (registrados > 99) {
                SiSendChar(stringIn[0]);
            }
            if (registrados > 9) {
                SiSendChar(stringIn[1]);
            }
            SiSendChar(stringIn[2]);
            stateEstadisticas = 19;
            break;
    }
}

void motorAccessController(void) {
    
    
    
    switch(state) {
        case 0:
            
            if (pintado == 0 && opcio == 0 && KEYBOARD_hayTecla() == 0 && SiCharAvail() == 0) {
                KEYBOARD_setModo(0);
                pintado = 1;
                lcdmenu = 0;
                Menu();
                erraseTipo();
                user.name[0] = user.name[2] = user.name[3] = user.name[4] = user.name[5] = user.name[6] = user.name[7] = user.name[8] = user.name[9] = ' ';
            }
            else if (opcio == 0) {
                if (SiCharAvail() == 1) {
                    
                    opcio = SiGetChar();
                    keyboard = 0;
                }
                if (KEYBOARD_hayTecla() == 1) {
                    
                    opcio = KEYBOARD_getTecla();
                    keyboard = 1;
                }
                
            }
            if ((opcio == 1  && keyboard == 1) || (opcio == 2 && keyboard == 1)) {//registro entrada
                lcdmenu = 1;
                state = 1;
            }
            else if (opcio == '1' && keyboard == 0) {//registro
                opcio = 2;
                lcdmenu = 1;
                state = 1;
            }
            else if (opcio == '2' && keyboard == 0) { //eliminar
                opcio = 3;
                lcdmenu = 1;
                state = 1;
            }
            else if (opcio == '3' && keyboard == 0) { //hora
                cont = 0;
                opcio = 4;
                state = 24;
            }
            else if (opcio == '4' && keyboard == 0) { //estadisticas
                opcio = 5;
                cont = 0;
                state = 29;
            }
            else if (opcio == 27 && keyboard == 0) {
                pintado = 0;
                opcio = 0;
            }
            
            else if (KEYBOARD_getTipo() == 'M') {
                opcio = 0;
                lcdmenu = 0;
            }
            
            else if (KEYBOARD_getTipo() == 'R') {
                reset = 1;
            }
            else if (KEYBOARD_getTipo() == 'S' && KEYBOARD_hayTecla() == 1) {
                Menu();
                KEYBOARD_getTecla();
                opcio = 0;
                
        
            }
            
            else if (KEYBOARD_getTipo() == 'S') {
                KEYBOARD_getTecla();
                erraseTipo();
                opcio = 0;
                pintado = 0;
               
            }
            else {
                opcio = 0;
                lcdmenu = 0;
            }
            break;
            
        case 1:
            KEYBOARD_setModo(1);
            cont = 0;
            state = 2;
            break;
            
        case 2:
            if (cont == MAXCHARS) state = 5;
            else {
                if (KEYBOARD_hayTecla()) {
                    keySio = 0;
                    state = 3;
                }
                else if (SiCharAvail()) {
                    keySio = 1;
                    aux = SiGetChar();
                    state = 3;
                }
            }
            break;
            
        case 3:
            if (keySio == 1) {
                KEYBOARD_setPreviousKey(38);
                tempUid[cont++] = aux;
                state = 4;
            }
            else {
                switch(KEYBOARD_getTipo()) {
                    case 'N': //letra nueva
                        tempUid[cont++] = KEYBOARD_getTecla();
                        state = 4;
                        break;
                    case 'C'://cambiar anterior
                        cont--;
                        tempUid[cont++] = KEYBOARD_getTecla();
                        index--;
                        state = 4;
                        break;
                    case 'M'://menuPrincipal
                        limpiaSegundaLinea();
                        state = 30;
                        break;
                    case 'R':
                        reset = 1;
                        break;
                    case 'S':
                        pintado = 0;
                        num = cuantosIn;
                        state = 30;
                        break;
                }
            }
            break;
            
        case 4:
            if (keySio == 1) addChar(aux);
            else {
                addChar(tempUid[--cont]);
                cont++;
            }
            keySio = 0;
            state = 2;
            break;
            
            
        case 5: //ya tenemos el uid entero
            transformaUID();
            cont = 0;
            break;
            
        case 6:
            cont = 0;
            switch(opcio) {
                case 1:
                    temps[0] = horD * 10;
                    IOTICAT_isBoss(user.uid);
                    state = 7;
                    break;
                case 2:
                    limpiaSegundaLinea();
                    lcdmenu = 2;
                    state = 17;
                    break;
                case 3:
                    IOTICAT_isBoss(user.uid);
                    state = 7;
                    break;
            }
            break;
            
        case 7:
            if (IOTICAT_isDone() == 1) {
                temps[1] = minD * 10;
                aux = IOTICAT_getIndex();
                state = 8;
                
            }
            break;
            
        case 8:
            switch (opcio) {
                case 3:
                    if (IOTICAT_getIsBoss() == 1) {
                        SiPutsCooperatiu("\n\rAquest usuari no es pot eliminar!\n\0");
                        limpiaSegundaLinea();
                        state = 13;
                    }
                    else {
                        if (aux > MAX_REGISTERED_USERS) {
                            SiPutsCooperatiu("\n\rAquest usuari no es pot eliminar! Usuari inexistent\n\0");
                            limpiaSegundaLinea();
                            state = 13;
                        }
                        else {
                            IOTICAT_deleteUser(aux);
                            state = 16;
                        }
                    }
                    break;
                case 1:
                    IOTICAT_getUserByIndex(aux, &user);
                    temps[0] += horU;
                    temps[1] += minU;
                    state = 9;
                    break;
            }
            break;
        /*GESTION ENTRADA DE UN USER*/    
        case 9:
            if (IOTICAT_isDone() == 1) {
                
                TIMER_resetTics(timerAux);
                
                puedeEntrar();
            }
            break;
            
        case 10:
            //lcdmenu = 3;
            if (TIMER_getTics(timerAux) >= MAXSEGONS) {
                limpiaSegundaLinea();
                TIMER_resetTics(timerAux);
                state = 11;
            }
            break;
            
        case 11: //aceptado el acceso
            if (cont == 1) {
                OpenDoor();
                SPEAKER_beep(1);
                state = 12;
            }
            else {
                SPEAKER_beep(cont);
                accesDenegat++;
                state = 30;
            }
            break;
            
        case 12:
            if (TIMER_getTics(timerAux) >= MAXSEGONS) {
                CloseDoor();
                puerta++;
                state = 30;
            }
            break;
        /*FIN ENTRADA DE UN USUARIO*/
        
        /*SALIDA GENERAL DE TODAS LAS OPCIONES*/
        case 13:
            if (SPEAKER_isBeeping() == 0) {
                num = cuantosIn;
                KEYBOARD_setModo(0);
                state = 14;
            }
            break;
            
        case 14:
            myCoopItoa(15);
            break;
            
        case 15:
            lcdmenu = 0;
            
            opcio = 0;
            state = 0;
            break;
        /*FIN SALIDA GENERAL*/
         
        /*ELIMINAR USUARIO*/    
        case 16:
            if (IOTICAT_isDone() == 1) {
                SiPutsCooperatiu("\n\rUsuari eliminat!\n\0");
                registrados--;
                state = 30;
            }
            break;
        /*FIN ELIMINAR USUARIO*/
            
        /*REGISTRO USER*/
        case 17:
            if (cont == MAXCHARS) {
                
            }
            else if (KEYBOARD_hayTecla()) {
                keySio = 0;
                state = 18;
            }
            else if (SiCharAvail()) {
                keySio = 1;
                aux = SiGetChar();
                state = 18;
            }
            break;
            
        case 18:
            if (keySio == 1) {
                if (aux == 13) state = 20;
                else {
                    KEYBOARD_setPreviousKey(38);
                    user.name[cont++] = aux;
                    state = 19;
                }
            }
            else {
                switch(KEYBOARD_getTipo()) {
                    case 'C':
                        user.name[--cont] = KEYBOARD_getTecla();
                        cont++;
                        index--;
                        state = 19;
                        break;
                    case 'N':
                        aux = KEYBOARD_getTecla();
                        if (aux == '#') state = 20;
                        else {
                            user.name[cont++] = aux;
                            state = 19;
                        }
                        break;
                    case 'M':
                        limpiaSegundaLinea();
                        state = 30;
                        break;
                    case 'R':
                        reset = 1;
                        break;
                    case 'S':
                        pintado = 0;
                        state = 30;
                        break;
                }
            }
            break;
        case 19:
            if (keySio == 1) addChar(aux);
            else {
                addChar(user.name[--cont]);
                cont++;
            }
            keySio = 0;
            state = 17;
            break;
            
        case 20: //ya tenemos el nombre
            KEYBOARD_setModo(0);
            user.length = cont;
            cont = 0;
            state = 21;
            break;
            
        case 21:
            if (keyboard == 1) {
                usuarioPorDefecto();
            }
            else {
                registraUserSIO();
            }
            break;
            
        case 22:
            if (registrados == MAX_REGISTERED_USERS) {
                SiPutsCooperatiu("\n\rNo es poden registrar mes usuaris!\n\0");
                limpiaSegundaLinea();
                state = 30;
            }
            else {
                IOTICAT_register(&user);
                state = 23;
            }
            break;
            
        case 23:
            if (IOTICAT_isDone() == 1) {
                
                SiPutsCooperatiu("\n\rUsuari registrat amb exit!\n\0");
                limpiaSegundaLinea();
                state = 30;
            }
            break;
        
        /*CAMBIO DE HORA DEL SISTEMA*/
        case 24:
            cambiaHoraSistema();
            break;
            
        case 25:
            IOTICAT_cambiarHora(offset[0], offset[1]);
            state = 26;
            break;
            
        case 26:
            if (IOTICAT_isDone() == 1) {
                SiPutsCooperatiu("\n\ryoyo\n\0");
                horD = horaNueva[0];
                horU = horaNueva[1];
                state = 27;
            }
            break;
            
        case 27:
            minD = horaNueva[2];
            minU = horaNueva[3];
            SiPutsCooperatiu("\n\rHora del sistema modificada!\n\0");
            state = 30;
            break; 
            
        case 28:
            seg = 0;
            TIMER_resetTics(timer);
            state = 31;
            break;
        
        
        /*ESTADISTICAS*/
        case 29:
            muestraEstadisticas();
            break;
            
        case 30:
            user.name[0] = user.name[2] = user.name[3] = user.name[4] = ' ';
            opcio = 0;
            limpiaSegundaLinea();
            state = 31;
            break;
            
        case 31:
            user.name[5] = user.name[6] = user.name[7] = user.name[8] = user.name[9] = ' ';
            state = 13;
            lcdmenu = 0;
            break;
    }
    
}

#define MAXCOLUMNES     16

static unsigned char  caracterInici, i, j, estatLCD;
static int timerLCD;
static unsigned char menuLinea[38] = {" XX:XX - XXX persones - Menu principal"};
static unsigned char introdueixUID[16] = {"Introdueix UID: "};
static unsigned char introdueixNom[16] = {"Introdueix nom: "};
static unsigned char benvinguda[16] = {"Hola            "};
static char getLength[4] = {38, 16, 16, 16};

/*PONER HORA LCD*/
void setHoraLCD(char lcd) {
    if (lcd == 0) {
        menuLinea[1] = horD + '0';
        menuLinea[2] = horU + '0';
        menuLinea[4] = minD + '0';
        menuLinea[5] = minU + '0';
    }
    estatLCD = 2;
}

void initLCD(void) {
    timerLCD = TIMER_getTimer();
    caracterInici = 0;
    estatLCD = j = i = 0;
    
    TIMER_resetTics(timerLCD);
}

/*AÑADE CARACTER A LA LINEA INFERIOR DEL LCD*/
void addChar(unsigned char aux) {
    LcGotoXY(index, 1);
    LcPutChar(aux);                                                                    
    index++;
    LcGotoXY(i, 0);
}

/*SET CUANTOS*/
void setCuantos(char lcd) {
    if (lcd == 0) {
        menuLinea[9] = stringIn[0];
        menuLinea[10] = stringIn[1];
        menuLinea[11] = stringIn[2];
    }
    estatLCD = 3;
}

/*LIMPIAR LINEA INFERIOR DEL LCD*/
void limpiaSegundaLinea(void) {
    LcGotoXY(0, 1);
    LcPutString("                ");
    LcGotoXY(i, 0);
    index = 0;
}

void PosaChar(char lcd) {
    switch(lcd) {
        case 0:
            LcPutChar(menuLinea[j]);
            j++;
            if (j == 38) j = 0;
            break;
        case 1:
            LcPutChar(introdueixUID[j]);
            j++;
            if (j == 16) j = 0;
            break;
        case 2:
            LcPutChar(introdueixNom[j]);
            j++;
            if (j == 16) j = 0;
            break;
        case 3:
            LcPutChar(benvinguda[j]);
            j++;
            if (j == 16) j = 0;
            break;
    }
    if (i++ > MAXCOLUMNES) {
        estatLCD = 1;
        TIMER_resetTics(timerLCD);
        LcGotoXY(0, 0);
    }
    //SiSendChar(estatLCD + '0');
}

void setName(char lcd) {
    if (lcd == 3) {
        benvinguda[5] = user.name[0];
        benvinguda[6] = user.name[1];
        benvinguda[7] = user.name[2];
        benvinguda[8] = user.name[3];
        benvinguda[9] = user.name[4];
        benvinguda[10] = user.name[5];
        benvinguda[11] = user.name[6];
        benvinguda[12] = user.name[7];
        benvinguda[13] = user.name[8];
        benvinguda[14] = user.name[9];
        benvinguda[user.length + 5] = '!';
    }
    estatLCD = 4;
}

void motorLCD(void) {
    switch(estatLCD) {
        case 0:
            PosaChar(lcdmenu);
            break;
        case 1:
            setHoraLCD(lcdmenu);
            break;
        case 2:
            setCuantos(lcdmenu);
            break;
        case 3:
            setName(lcdmenu);
            break;
        case 4:
            if(TIMER_getTics(timerLCD) > 200) {
                TIMER_resetTics(timerLCD);
                i = 0;
                estatLCD = 5;
            }
            break;
        case 5:
            if (TIMER_getTics(timerLCD) >= 900) {
                //Alerta, ja porto 50ms. des de l'últim refresc
                caracterInici++;
                if (caracterInici == getLength[lcdmenu]) 
                    caracterInici = 0;
                estatLCD = 0;
                LcGotoXY(0,0);
                j = caracterInici;
                i = 0;
            }
            break;
    }
}


