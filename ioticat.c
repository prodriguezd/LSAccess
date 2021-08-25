/*
 * File:   ioticat.c
 * Author: xoselolo
 *
 * Created on 3 de abril de 2020, 11:10
 */
 
#include "ioticat.h"
#include "SIO.h"

// Info
static char is_boss_in;
static IoticatDatabase database;
static unsigned char state = 0;

static unsigned char ferFeina = 0;

static unsigned char index = 0;
static unsigned char* uid;
static User* user;
static unsigned char inc_hour;
static unsigned char inc_minute;
static char isBoss = 0;

static unsigned char cont_x = 0;
static unsigned char cont_y = 0;

static unsigned char u_aux;
static char aux;


/**
 * void IOTICAT_init()
 * @details: initializes the empty database
 *      cuantos = 0
 *      cuantos_in = 0
 */
void IOTICAT_init(){
    database.cuantos = 200;
    database.cuantos_in = 100;
    state = 0;
    ferFeina = 0;
    index = 0;
   
    
    database.users[BOSS_INDEX].name[0] = 'M';
    database.users[BOSS_INDEX].name[1] = 'a';
    database.users[BOSS_INDEX].name[2] = 'x';
    database.users[BOSS_INDEX].name[3] = ' ';
    database.users[BOSS_INDEX].name[4] = 'B';
    database.users[BOSS_INDEX].name[5] = 'o';
    database.users[BOSS_INDEX].name[6] = 's';
    database.users[BOSS_INDEX].name[7] = 's';
    database.users[BOSS_INDEX].name[8] = ' ';
    database.users[BOSS_INDEX].name[9] = ' ';
    database.users[BOSS_INDEX].length = 8;
    database.users[BOSS_INDEX].uid[0] = 0xCA;
    database.users[BOSS_INDEX].uid[1] = 0xFE;
    database.users[BOSS_INDEX].uid[2] = 0x69;
    database.users[BOSS_INDEX].uid[3] = 0xCA;
    database.users[BOSS_INDEX].uid[4] = 0xFE;
    database.users[BOSS_INDEX].in_min_hour = 0;
    database.users[BOSS_INDEX].in_min_minute = 0;
    database.users[BOSS_INDEX].in_max_hour = 23;
    database.users[BOSS_INDEX].in_max_minute = 59;
    database.users[BOSS_INDEX].min_permanence_time.value[0] = 0;
    database.users[BOSS_INDEX].min_permanence_time.value[1] = 0;
    database.users[BOSS_INDEX].min_permanence_time.value[2] = 0;
    database.users[BOSS_INDEX].in_real_hour = 0;
    database.users[BOSS_INDEX].in_real_minute = 0;
    is_boss_in = 0;
    
    
    for (state = 0; state < 200; state++) {
        database.users[state].name[0] = 'U';
    database.users[state].name[1] = 's';
    database.users[state].name[2] = 'e';
    database.users[state].name[3] = 'r';
    database.users[state].name[4] = ' ';
    database.users[state].name[5] = ' ';
    database.users[state].name[6] = ' ';
    database.users[state].name[7] = ' ';
    database.users[state].name[8] = ' ';
    database.users[state].name[9] = ' ';
    database.users[state].length = 4;
    database.users[state].uid[0] = state;
    database.users[state].uid[1] = state;
    database.users[state].uid[2] = state;
    database.users[state].uid[3] = state;
    database.users[state].uid[4] = state;
    database.users[state].in_min_hour = 0;
    database.users[state].in_min_minute = 0;
    database.users[state].in_max_hour = 3;
    database.users[state].in_max_minute = 30;
    database.users[state].min_permanence_time.value[0] = 0;
    database.users[state].min_permanence_time.value[1] = 1;
    database.users[state].min_permanence_time.value[2] = 5;
    database.users[state].in_real_hour = 0;
    database.users[state].in_real_minute = 0;
    }
    state = 0;
    
    

}

void IOTICAT_motor(){
    if(ferFeina != 0){
        switch(state){
            case 0:
                state = ferFeina;
                
                break;
            case 1:
                cont_x = 0;
                cont_y = 0;
                state = 7;
                break;
            case 2:
                cont_y = 0;
                state = 9;
                break;
            case 3:
                if(index == database.cuantos - 1){
                    database.cuantos --;
                    ferFeina = 0;
                    state = 0;
                }else{
                    cont_x = database.cuantos - 1;
                    cont_y = 0;
                    state = 20;
                }
                break;
            case 4:
                if(database.cuantos_in == 0){
                    ferFeina = 0;
                    state = 0;
                }else{
                    cont_x = 0;
                    state = 30;
                }
                break;
            case 5:
                if(index == database.cuantos_in){
                    database.cuantos_in ++;
                    ferFeina = 0;
                    state = 0;
                }else{
                    cont_x = database.cuantos_in;
                    cont_y = 0;
                    state = 20;
                }
                break;
            case 6:
                if (index == database.cuantos_in - 1){
                    database.cuantos_in --;
                    ferFeina = 0;
                    state = 0;
                }else{
                    cont_x = database.cuantos_in - 1;
                    cont_y = 0;
                    state = 20;
                }
                break;
                
            // ------------ 1 : BUSCAR UN USUARIO ---------------
            case 7:
                if(cont_x == database.cuantos){
                    ferFeina = 0;
                    state = 0;
                    index = NOT_FOUND_USER;
                }else{
                    cont_y = 0;
                    state = 8;
                }
                break;
            case 8:
                if(cont_y == UID_LENGTH){
                    ferFeina = 0;
                    index = cont_x;
                    state = 0;
                }else{
                    if(database.users[cont_x].uid[cont_y] == uid[cont_y]){
                        cont_y ++;
                    }else{
                        cont_x ++;
                        state = 7;
                    }
                }
                break;
                
            // ------------ 2 : GET UN USUARIO ---------------
            case 9:
                if(cont_y == /*database.users[index].length*/10){
                    cont_y = 0;
                    state = 10;
                }else{
                    user->name[cont_y] = database.users[index].name[cont_y];
                    cont_y ++;
                }
                break;
            case 10:
                user->length = database.users[index].length;
                state = 11;
                break;
            case 11:
                if(cont_y == UID_LENGTH){
                    user->in_min_hour = database.users[index].in_min_hour;
                    state = 12;
                }else{
                    user->uid[cont_y] = database.users[index].uid[cont_y];
                    cont_y ++;
                }
                break;
            case 12:
                user->in_min_minute = database.users[index].in_min_minute;
                state = 13;
                break;
            case 13:
                user->in_max_hour = database.users[index].in_max_hour;
                state = 14;
                break;
            case 14:
                user->in_max_minute = database.users[index].in_max_minute;
                state = 15;
                break;
            case 15:
                user->min_permanence_time.value[0] = database.users[index].min_permanence_time.value[0];
                state = 16;
                break;
            case 16:
                user->min_permanence_time.value[1] = database.users[index].min_permanence_time.value[1];
                user->min_permanence_time.value[2] = database.users[index].min_permanence_time.value[2];
                state = 17;
                break;
            case 17:
                user->in_real_hour = database.users[index].in_real_hour;
                state = 18;
                break;
            case 18:
                user->in_real_minute = database.users[index].in_real_minute;
                state = 19;
                break;
            case 19:
                ferFeina = 0;
                state = 0;
                break;
                
            // ------------ 3 : GET UN USUARIO ---------------
            // ------------ 5 : SIGN IN USER ---------------
            // ------------ 6 : SIGN OUT USUARIO ---------------
            case 20:
                if(cont_y < /*database.users[index].length*/10 || cont_y < /*database.users[cont_x].length*/10){
                    aux = database.users[index].name[cont_y];
                    database.users[index].name[cont_y] = database.users[cont_x].name[cont_y];
                    database.users[cont_x].name[cont_y] = aux;
                    cont_y ++;
                }else{
                    u_aux = database.users[index].length;
                    database.users[index].length = database.users[cont_x].length;
                    database.users[cont_x].length = u_aux;
                    state = 21;
                }
                break;
            case 21:
                if(cont_y < UID_LENGTH){
                    aux = database.users[index].uid[cont_y];
                    database.users[index].uid[cont_y] = database.users[cont_x].uid[cont_y];
                    database.users[cont_x].uid[cont_y] = aux;
                    cont_y ++;
                }else{
                    aux = database.users[index].in_min_hour;
                    database.users[index].in_min_hour = database.users[cont_x].in_min_hour;
                    database.users[cont_x].in_min_hour = aux;
                    state = 22;
                }
                break;
            case 22:
                aux = database.users[index].in_min_minute;
                database.users[index].in_min_minute = database.users[cont_x].in_min_minute;
                database.users[cont_x].in_min_minute = aux;
                state = 23;
                break;
            case 23:
                aux = database.users[index].in_max_hour;
                database.users[index].in_max_hour = database.users[cont_x].in_max_hour;
                database.users[cont_x].in_max_hour = aux;
                state = 24;
                break;
            case 24:
                aux = database.users[index].in_max_minute;
                database.users[index].in_max_minute = database.users[cont_x].in_max_minute;
                database.users[cont_x].in_max_minute = aux;
                state = 25;
                break;
            case 25:
                u_aux = database.users[index].min_permanence_time.value[0];
                database.users[index].min_permanence_time.value[0] = database.users[cont_x].min_permanence_time.value[0];
                database.users[cont_x].min_permanence_time.value[0] = u_aux;
                state = 26;
                break;
            case 26:
                u_aux = database.users[index].min_permanence_time.value[1];
                database.users[index].min_permanence_time.value[1] = database.users[cont_x].min_permanence_time.value[1];
                database.users[cont_x].min_permanence_time.value[1] = u_aux;
                state = 47;
                break;
            case 47:
                u_aux = database.users[index].min_permanence_time.value[2];
                database.users[index].min_permanence_time.value[2] = database.users[cont_x].min_permanence_time.value[2];
                database.users[cont_x].min_permanence_time.value[2] = u_aux;
                state = 27;
                break;    
            case 27:
                aux = database.users[index].in_real_hour;
                database.users[index].in_real_hour = database.users[cont_x].in_real_hour;
                database.users[cont_x].in_real_hour = aux;
                state = 28;
                break;
            case 28:
                aux = database.users[index].in_real_minute;
                database.users[index].in_real_minute = database.users[cont_x].in_real_minute;
                database.users[cont_x].in_real_minute = aux;
                state = 29;
                break;
            case 29:
                switch(ferFeina){
                    case 3:
                        database.cuantos --;
                        break;
                    case 5:
                        database.cuantos_in ++;
                        break;
                    case 6:
                        database.cuantos_in --;
                        break;
                }
                ferFeina = 0;
                state = 0;
                break;
                
            // ------------ 4 : CAMBIAR HORA DEL SISTEMA  ---------------
            case 30:
                database.users[cont_x].in_real_minute += inc_minute;
                state = 31;
                break;
            case 31:
                if(database.users[cont_x].in_real_minute > 59){
                    database.users[cont_x].in_real_minute -= 60;
                    //database.users[cont_x].in_real_hour ++;
                }
                state = 32;
                break;
            case 32:
                database.users[cont_x].in_real_hour += inc_hour;
                state = 33;
                break;
            case 33:
                if(database.users[cont_x].in_real_hour > 23){
                    database.users[cont_x].in_real_hour -= 24;
                }
                state = 34;
                break;
            case 34:
                cont_x++;
                state = 35;
                break;
            case 35:
                if(cont_x == database.cuantos_in){
                    ferFeina = 0;
                    state = 0;
                }else{
                    state = 30;
                }
                break;
                
            // ------------ 36 : REGISTRAR NUEVO USUARIO  ---------------
            case 36:
                cont_x = database.cuantos;
                cont_y = 0;
                state = 37;
                break;
            case 37:
                //LOLO: Hay que guardar antes el length y despues comparar el valor del contador
                if(cont_y < /*user->length*/10){
                    database.users[cont_x].name[cont_y] = user->name[cont_y];
                    cont_y++;
                    //AQUI ESTA EL PROBLEMA, CUANDO LO INTENTO PASAR A LA SIO NO MUESTRA NADA
                    
                }else{
                    database.users[cont_x].length = user->length;
                    cont_y = 0;
                    state = 38;
                }
                break;
            case 38:
                if(cont_y < UID_LENGTH){
                    database.users[cont_x].uid[cont_y] = user->uid[cont_y];
                    cont_y++;
                }else{
                    database.users[cont_x].in_min_hour = user->in_min_hour;
                    state = 39;
                }
                break;
            case 39:
                database.users[cont_x].in_min_minute = user->in_min_minute;
                state = 40;
                break;
            case 40:
                database.users[cont_x].in_max_hour = user->in_max_hour;
                state = 41;
                break;
            case 41:
                database.users[cont_x].in_max_minute = user->in_max_minute;
                state = 42;
                break;
            case 42:
                database.users[cont_x].min_permanence_time.value[0] = user->min_permanence_time.value[0];
                state = 43;
                break;
            case 43:
                database.users[cont_x].min_permanence_time.value[1] = user->min_permanence_time.value[1];
                database.users[cont_x].min_permanence_time.value[2] = user->min_permanence_time.value[2];
                state = 44;
                break;
            case 44:
                database.users[cont_x].in_real_hour = user->in_real_hour;
                state = 45;
                break;
            case 45:
                database.users[cont_x].in_real_minute = user->in_real_minute;
                state = 46;
                break;
            case 46:
                database.cuantos++;
                ferFeina = 0;
                state = 0; 
                break;
            // Look if it's boss
            case 48:
                cont_y = 0;
                state = 49;
                break;
            case 49:
                if(uid[cont_y] == database.users[BOSS_INDEX].uid[cont_y]){
                    state = 50;
                }else{
                    ferFeina = 1;
                    state = 0;
                }
                break;
            case 50:
                if(cont_y == 4){
                    ferFeina = 0;
                    index = BOSS_INDEX;
                    isBoss = 1;
                    state = 0;
                }else{
                    cont_y++;
                    state = 49;
                }
                break;
            case 51:
                if(is_boss_in){
                    database.users[BOSS_INDEX].in_real_minute += inc_minute;
                    state = 52;
                }else{
                    state = 4;
                }
                break;
            case 52:
                if(database.users[BOSS_INDEX].in_real_minute > 59){
                    database.users[BOSS_INDEX].in_real_minute -= 60;
                    //database.users[BOSS_INDEX].in_real_hour ++;
                }
                state = 53;
                break;
            case 53:
                database.users[BOSS_INDEX].in_real_hour += inc_hour;
                state = 54;
                break;
            case 54:
                if(database.users[BOSS_INDEX].in_real_hour > 23){
                    database.users[BOSS_INDEX].in_real_hour -= 24;
                }
                state = 4;
                break;
        }
    }  
}

unsigned char IOTICAT_getCuantos(void) {
    return database.cuantos;
}

unsigned char IOTICAT_getCuantosIn(void) {
    return database.cuantos_in;
}

// -------------------- INTERFÍCIE ---------------------
    /**
    * void IOTICAT_register()
    * @details: increments in 1 the value of cuantos to make visible the new user
    */
   void IOTICAT_register(User* u){
       ferFeina = 36;
       user = u;
       /*SiPuts("\r\n");
       SiPuts(user->uid);
       SiPuts("\n\0");*/
   }
   
   /**
    * void IOTICAT_searchUser
    * @param uid : uid of the user to be found
    */
   void IOTICAT_searchUser(unsigned char* uidToFound){
       uid = uidToFound;
       cont_x = 0;
       ferFeina = 48;
   }
   
   /**
    * void IOTICAT_getUser()
    * @param u : mem adress of the user to return
    */
   void IOTICAT_getUser(User* u){
       u = user;
       
       ferFeina = 2;
   }

   /**
    * void IOTICAT_deleteUser
    * @param i : index of the user to be deleted
    */
   void IOTICAT_deleteUser(unsigned char i){
       index = i;
       ferFeina = 3;
   }
   
   void IOTICAT_getUserByIndex(unsigned char i, User* u) {
       index = i;
       user = u;
       ferFeina = 2;
   }
   
   /**
    * void IOTICAT_cambiarHora
    * @param inc_h : increment on hours
    * @param inc_m : increment on minutes
    */
   void IOTICAT_cambiarHora(unsigned char inc_h, unsigned char inc_m){
       ferFeina = 51;
       inc_hour = inc_h;
       inc_minute = inc_m;
   }
   
   /**
    * void IOTICAT_signInUsuario
    * @param i : index of the user to sign in
    */
   void IOTICAT_signInUsuario(unsigned char i){
       ferFeina = 5;
       index = i;
   }
   
   void IOTICAT_signOutUsuario(unsigned char i){
       ferFeina = 6;
       index = i;
   }
   
   unsigned char IOTICAT_getIndex() { return index; }
   
   unsigned char IOTICAT_isDone() {
       if (ferFeina == 0) return 1;
       else return 0;
   }
   
   
   // BOSS methods
   void IOTICAT_isBoss(unsigned char* uidToFound){
       uid = uidToFound;
       cont_x = 0;
       ferFeina = 48;
   }
   
char IOTICAT_getIsBoss(){
    return isBoss;
}
   
char IOTICAT_isBossIn(){
    return is_boss_in;
}


void IOTICAT_signInBoss(unsigned char inHour, unsigned char inMinute){
    database.users[BOSS_INDEX].in_real_hour = inHour;
    database.users[BOSS_INDEX].in_real_minute = inMinute; 
    is_boss_in = 1;
}
   
   void IOTICAT_signOutBoss(void){
       is_boss_in = 0;
   }

