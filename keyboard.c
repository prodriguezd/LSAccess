
#include <xc.h>
#include "keyboard.h"
#include "SIO.h"

// Las T definidas estan en función de que el TIMER_0 salte cada 1ms
#define T_ESCOMB 40
#define T_REBOTS 35
#define T_RESET 10000
#define T_MENU_SIO 6000
#define T_MODIFICA 1000

static char state = 0;

static unsigned char sms = 0;

static unsigned char contTecla = 0;

static unsigned char previousKey = '-';
static unsigned char actualKey, returnChar;
static unsigned char hayTecla = 0;
static unsigned char fila = 0;
static unsigned char columna = 0;

static char tipo;  

static int timer_r_id; // Para Rebotes y Reset (r)
static int timer_modifica_id; // Tiempo entre tecla y tecla



/**
 * Inicializa el tad
 * Pre: Ninguna
 * Post: El tad quedará inicializado
 */
void KEYBOARD_init(){
    //Convertim a digital
    AD1PCFGbits.PCFG12 = 1;
    
    state = 0;

    sms = 0;

    contTecla = 0;

    previousKey = 38;
    hayTecla = 0;
    fila = 1;
    columna = 0;

    timer_r_id = TIMER_getTimer();
    timer_modifica_id = TIMER_getTimer();
    
    
    
    //Filas como salida
    TRISBbits.TRISB6 = 0;
    TRISBbits.TRISB7 = 0;
    TRISBbits.TRISB8 = 0;
    TRISBbits.TRISB9 = 0;
    //columnas como entrada
    TRISAbits.TRISA2 = 1; 
    TRISAbits.TRISA3 = 1;
    TRISAbits.TRISA4 = 1;
    
    
    LATBbits.LATB6 = 0; // Fila 1
    LATBbits.LATB7 = 1; // Fila 2
    LATBbits.LATB8 = 1; // Fila 3
    LATBbits.LATB9 = 1; // Fila 4
    
}

/**
 * Informa de si hay alguna tecla apretada en la fila que tenemos activa
 * @return 
 *      0 - No hay tecla apretada
 *      1 - Hay una tecla apretada
 */
char KEYBOARD_pressed(){
    if (C1 == 0 || C2 == 0 || C3 == 0) return 1;
    return 0;
}

unsigned char KEYBOARD_descifraTecla(){
    
    switch (fila){
        case 1:
            switch (columna){
                case 1:
                    actualKey = 1;
                    break;
                case 2:
                    actualKey = 2;
                    break;
                case 3:
                    actualKey = 3;
                    break;
            }
            break;
        case 2:
            switch (columna){
                case 1:
                    actualKey = 4;
                    break;
                case 2:
                    actualKey = 5;
                    break;
                case 3:
                    actualKey = 6;
                    break;
            }
            break;
        case 3:
            switch (columna){
                case 1:
                    actualKey = 7;
                    break;
                case 2:
                    actualKey = 8;
                    break;
                case 3:
                    actualKey = 9;
                    break;
            }
            break;
        case 4:
            switch (columna){
                case 1:
                    actualKey = '*';
                    break;
                case 2:
                    actualKey = 11;
                    break;
                case 3:
                    actualKey = '#';
                    break;
            }
            break;
    }
    return actualKey;
}


/**
 * Motor del tad KEYBOARD
 * Pre: Haber inicializado el tad con la funcion KEYBOARD_init()
 * Post: Estado actual = Estado futuro (estado actual[t-1])
 */
void KEYBOARD_motor(){
    
    AD1PCFGbits.PCFG12 = 1;
   
    TRISBbits.TRISB6 = 0;
    TRISBbits.TRISB7 = 0;
    TRISBbits.TRISB8 = 0;
    TRISBbits.TRISB9 = 0;
    //columnas como entrada
    TRISAbits.TRISA2 = 1; 
    TRISAbits.TRISA3 = 1;
    TRISAbits.TRISA4 = 1;
    
    
   
    switch(state) {
        case 0:
            //CAMBIAR TECLADO PORT POR LAT
            LATBbits.LATB6 = 0; // Fila 1
            LATBbits.LATB7 = 1; // Fila 2
            LATBbits.LATB8 = 1; // Fila 3
            LATBbits.LATB9 = 1; // Fila 4
            //fila = 1;
            if(KEYBOARD_pressed() == 1){
                if(PORTAbits.RA2 == 0){
                    columna = 1;
                }else if(PORTAbits.RA3 == 0){
                    columna = 2;
                }else if (PORTAbits.RA4 == 0){
                    columna = 3;
                }
                TIMER_resetTics(timer_r_id);
                state = -1;
            }else if(TIMER_getTics(timer_r_id) > T_ESCOMB){
                LATBbits.LATB6 = 1; // Fila 1
                LATBbits.LATB7 = 0; // Fila 2
                LATBbits.LATB8 = 1; // Fila 3
                LATBbits.LATB9 = 1; // Fila 4
                TIMER_resetTics(timer_r_id);
                state = 1;
                fila = 2;
            }
           
            break;
        case 1:
            LATBbits.LATB6 = 1; // Fila 1
            LATBbits.LATB7 = 0; // Fila 2
            LATBbits.LATB8 = 1; // Fila 3
            LATBbits.LATB9 = 1; // Fila 4
            //fila = 2;
            if(KEYBOARD_pressed() == 1){
                if(PORTAbits.RA2 == 0){
                    columna = 1;
                }else if(PORTAbits.RA3 == 0){
                    columna = 2;
                }else if (PORTAbits.RA4 == 0){
                    columna = 3;
                }
                TIMER_resetTics(timer_r_id);
                state = -1;
            }else if(TIMER_getTics(timer_r_id) > T_ESCOMB){
                LATBbits.LATB6 = 1; // Fila 1
                LATBbits.LATB7 = 1; // Fila 2
                LATBbits.LATB8 = 0; // Fila 3
                LATBbits.LATB9 = 1; // Fila 4
                TIMER_resetTics(timer_r_id);
                state = 2;
                fila = 3;
            }
            break;
        case 2:
            LATBbits.LATB6 = 1; // Fila 1
            LATBbits.LATB7 = 1; // Fila 2
            LATBbits.LATB8 = 0; // Fila 3
            LATBbits.LATB9 = 1; // Fila 4
            //fila = 3;
            if(KEYBOARD_pressed() == 1){
                if(PORTAbits.RA2 == 0){
                    columna = 1;
                }else if(PORTAbits.RA3 == 0){
                    columna = 2;
                }else if (PORTAbits.RA4 == 0){
                    columna = 3;
                }
                TIMER_resetTics(timer_r_id);
                state = -1;
            }else if(TIMER_getTics(timer_r_id) > T_ESCOMB){
                LATBbits.LATB6 = 1; // Fila 1
                LATBbits.LATB7 = 1; // Fila 2
                LATBbits.LATB8 = 1; // Fila 3
                LATBbits.LATB9 = 0; // Fila 4
                TIMER_resetTics(timer_r_id);
                state = 3;
                fila = 4;
            }
            
            break;
        case 3:
            LATBbits.LATB6 = 1; // Fila 1
            LATBbits.LATB7 = 1; // Fila 2
            LATBbits.LATB8 = 1; // Fila 3
            LATBbits.LATB9 = 0; // Fila 4
            //fila = 4;
            if(KEYBOARD_pressed() == 1){
                if(PORTAbits.RA2 == 0){
                    columna = 1;
                }else if(PORTAbits.RA3 == 0){
                    columna = 2;
                }else if (PORTAbits.RA4 == 0){
                    columna = 3;
                }
                TIMER_resetTics(timer_r_id);
                state = -1;
            }else if(TIMER_getTics(timer_r_id) > T_ESCOMB){
                LATBbits.LATB6 = 0; // Fila 1
                LATBbits.LATB7 = 1; // Fila 2
                LATBbits.LATB8 = 1; // Fila 3
                LATBbits.LATB9 = 1; // Fila 4
                TIMER_resetTics(timer_r_id);
                state = 0;
                fila = 1;
            }
            
            break;
        case -1:
            actualKey = KEYBOARD_descifraTecla();
            state = -2;
            break;
        case -2:
            if (TIMER_getTics(timer_r_id) < T_REBOTS) {
                if(KEYBOARD_pressed() == 0){
                    LATBbits.LATB6 = 0; // Fila 1
                    LATBbits.LATB7 = 1; // Fila 2
                    LATBbits.LATB8 = 1; // Fila 3
                    LATBbits.LATB9 = 1; // Fila 4
                    TIMER_resetTics(timer_r_id);
                    state = 0;
                    fila = 1;
                }
            }
            else if (KEYBOARD_pressed() == 1 && TIMER_getTics(timer_r_id) > T_REBOTS) {
                state = 4;
            }
            break;
        case 4:
            if(actualKey == '#'){
                state = 20;
            }
                /*if(actualKey == '*'){
                    TIMER_resetTics(timer_r_id);
                    contTecla = 0;
                    // hayTecla = 1;
                    // tipo = 'M';
                    state = 13; // MODIFICADO
                }else{
                    if(sms == 1){
                        state = 6;
                    }else{
                        hayTecla = 1;
                        //tipo = 'O';
                        state = 8;
                    }
                }*/
            else if (actualKey == '*') {
                state = 25;    
            }
            else {
               if(sms == 1){
                        state = 6;
                    }else{
                        hayTecla = 1;
                        //tipo = 'O';
                        state = 8;
                    } 
            }
            
            break;
        case 5:
            if(KEYBOARD_pressed() == 0){
               TIMER_resetTics(timer_r_id);
               hayTecla = 1;
               returnChar = '#';
               tipo = 'N';
               state = 8;
           }else{
               if(TIMER_getTics(timer_r_id) >= T_RESET && KEYBOARD_pressed() == 1){
                  hayTecla = 1;
                  tipo = 'R';
                  state = 8;
               }
           }
            break;
        case 6:
            if(actualKey != previousKey){
                contTecla = 1;
                TIMER_resetTics(timer_modifica_id);
                state = 11;
                tipo = 'N';
            }else{
                if(TIMER_getTics(timer_modifica_id) < T_MODIFICA){
                    contTecla++;
                    TIMER_resetTics(timer_modifica_id);
                    state = 7;
                    tipo = 'C';
                }else{
                    contTecla = 1;
                    TIMER_resetTics(timer_modifica_id);
                    state = 11;
                    tipo = 'N';
                }
            }
            break;
        case 7:
            if(contTecla > SMSDICTIONARY_maxChars(actualKey)){
                contTecla = 1;
            }
            state = 11;
            break;
        case 8:
            if(hayTecla == 0){
                state = 9;
            }
            break;
        case 9:
            if(KEYBOARD_pressed() == 0){
                //hayTecla = 1;
                TIMER_resetTics(timer_r_id);
                state = 10;
            }
            break;
        case 10:
            if(TIMER_getTics(timer_r_id) > T_REBOTS){
                
                LATBbits.LATB6 = 0; // Fila 1
                LATBbits.LATB7 = 1; // Fila 2
                LATBbits.LATB8 = 1; // Fila 3
                LATBbits.LATB9 = 1; // Fila 4
                TIMER_resetTics(timer_r_id);
                state = 0;
                fila = 1;
                
                /*if (hayTecla == 0) {
                    
                }*/
            }
            break;
        case 11:
            returnChar = SMSDICTIONARY_getChar(actualKey, contTecla);
            previousKey = actualKey;
            state = 12;
            break;
        case 12:
            hayTecla = 1;
            state = 8;
            break;
        case 13:
            if(TIMER_getTics(timer_r_id) > T_MENU_SIO && KEYBOARD_pressed() == 1 ){
                
                    TIMER_resetTics(timer_r_id);
                    hayTecla = 1;
                    tipo = 'S';
                    state = 8;
                }
            else{
                if(KEYBOARD_pressed() == 0){
                    TIMER_resetTics(timer_r_id);
                    hayTecla = 1;
                    tipo = 'M';
                    state = 8;
                } 
            }
            break;
            
        case 20:
            TIMER_resetTics(timer_r_id);
            TIMER_resetTics(timer_modifica_id);
            state = 21;
            break;
            
        case 21:
            if (KEYBOARD_pressed() == 0) {
                TIMER_resetTics(timer_r_id);
                state = 22;
            }
            break;
        case 22:
            if (TIMER_getTics(timer_r_id) >= T_REBOTS) {
                if (KEYBOARD_pressed() == 1) {
                    //eran rebotes
                    state = 21;
                }
                else {
                    state = 23;
                }
            }
            break;
            
        case 23:
            if (TIMER_getTics(timer_modifica_id) > T_RESET) {
                tipo = 'R';
                SiPuts("\n\rdetecto reset\n\0");
                hayTecla = 1;
                state = 24; 
            }
            else {
                tipo = 'N';
                //hayTecla = 1;
                state = 24;
                
            }
            break;
            
        case 24:
            TIMER_resetTics(timer_r_id);
            hayTecla = 1;
                state = 0;
                fila = 1;
            break;
            
        case 25:
            TIMER_resetTics(timer_r_id);
            TIMER_resetTics(timer_modifica_id);
            state = 26;
            break;
            
        case 26:
            if (KEYBOARD_pressed() == 0) {
                TIMER_resetTics(timer_r_id);
                state = 27;
            }
            break;
            
        case 27:
            if (TIMER_getTics(timer_r_id) >= T_REBOTS) {
                if (KEYBOARD_pressed() == 1) {
                    //eran rebotes
                    state = 26;
                }
                else {
                    state = 28;
                }
            }
            break;
            
        case 28:
            if (TIMER_getTics(timer_modifica_id) > T_MENU_SIO) {
                tipo = 'S';
                hayTecla = 1;
                state = 24; 
            }
            else {
                tipo = 'M';
                //hayTecla = 1;
                state = 24;  
            }
            break;
    }
}

// -------------- INTERFICIE N --------------------
/**
 * Indica si existe una nueva tecla a tratar
 * Pre: init()
 * Post: Sabremos si hay una tecla
 * @return
 *      0 - No hay tecla
 *      1 - Hay tecla
 */
char KEYBOARD_hayTecla(){
    return hayTecla;
}

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
char KEYBOARD_getTipo(){
    return tipo;
}

/**
 * Devuelve la tecla que ha sido apretada
 * Pre: hayTecla
 * Post: tecla apretada
 * @return 
 *      Tecla apretada
 */
char KEYBOARD_getTecla(){
    hayTecla = 0;
    if (sms == 1) return returnChar;
    else return actualKey;  
}

void KEYBOARD_setModo(char modo) { sms = modo; }

void KEYBOARD_setPreviousKey(char key) { previousKey = 38; }

void erraseTipo(void) {
    tipo = 'L';
}

