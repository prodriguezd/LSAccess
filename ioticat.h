/*
 * File:   ioticat.h
 * Author: xoselolo
 *
 * Created on 3 de abril de 2020, 11:10
 */

#ifndef IOTICAT_H_
#define IOTICAT_H_

#include <stdio.h>
#include <string.h>

#define BOSS_INDEX              231
#define MAX_REGISTERED_USERS    230 // + 2
#define MAX_WORKING_USERS       100
#define NOT_FOUND_USER          250
#define UID_LENGTH              5



/******************************** TAD STRUCTURE ********************************/
typedef struct {
    unsigned char value[3]; // [2]
}TimeInMinutes;

/**
 * name                 : user's name
 * uid                  : user personal ID
 * in_min_hour          : minimum hour to enter
 * in_min_minute        : minimum minute to enter
 * in_max_hour          : maximum hour to exit
 * in_max_minute        : maximum minute to exit
 * min_permanence_time  : minimum time
 */
typedef struct{
    char name[10];
    unsigned char length;
    unsigned char uid[UID_LENGTH];
    unsigned char in_min_hour;
    unsigned char in_min_minute;
    unsigned char in_max_hour;
    unsigned char in_max_minute;
    TimeInMinutes min_permanence_time;//in minutes
    unsigned char in_real_hour;
    unsigned char in_real_minute;
}User;

/**
 * users                : usuarios que tienen permisos en el sistema
 * times                : timepo que lleva trabajando cada uno de los usuarios que estan trabajando
 * cuantos              : indica cuantos usuarios tienen permiso
 * cuantos_in           : indica cuantos usuarios estan trabajando
 */
typedef struct{
    User users[MAX_REGISTERED_USERS + 2];
    //TimeInMinutes times[MAX_WORKING_USERS];
    unsigned char cuantos;
    unsigned char cuantos_in;
}IoticatDatabase;



/*********************************** TAD METHODS *****************************/
/**
 * void IOTICAT_init()
 * @details: initializes the empty database
 *      @code cuantos = 0
 *      @code cuantos_in = 0
 */
void IOTICAT_init();


void IOTICAT_motor();

// -------------------- INTERFÍCIE ---------------------
    /**
    * void IOTICAT_register()
    * @details: increments in 1 the value of cuantos to make visible the new user
    */
   void IOTICAT_register(User* u);
   
   /**
    * void IOTICAT_searchUser
    * @param uid : uid of the user to be found
    */
   void IOTICAT_searchUser(unsigned char* uidToFound);
   
   /**
    * void IOTICAT_getUser()
    * @param u : mem adress of the user to return
    */
   void IOTICAT_getUser(User* u);
   
   /**
    * void IOTICAT_deleteUser
    * @param i : index of the user to be deleted
    */
   void IOTICAT_deleteUser(unsigned char i);
   
   /**
    * void IOTICAT_cambiarHora
    * @param inc_h : increment on hours
    * @param inc_m : increment on minutes
    */
   void IOTICAT_cambiarHora(unsigned char inc_h, unsigned char inc_m);
   
   /**
    * void IOTICAT_signInUsuario
    * @param i : index of the user to sign in
    */
   void IOTICAT_signInUsuario(unsigned char i);
   
   void IOTICAT_signOutUsuario(unsigned char i);
   
   /**
    * Gives the position of the user we just search
    * @return position of the user
    */
   unsigned char IOTICAT_getIndex(void);
   
   /**
    * Notifies the access controller that he is done with work
    * @return if FerFeina == 0 returns 1 (means done)
    */
   unsigned char IOTICAT_isDone(void);
   
   void IOTICAT_getUserByIndex(unsigned char index, User* u);
   
   unsigned char IOTICAT_getCuantosIn(void);
   unsigned char IOTICAT_getCuantos(void);
   
   
   // Boss methods
   void IOTICAT_isBoss(unsigned char* uidToFound);
   
   char IOTICAT_getIsBoss();
   
   char IOTICAT_isBossIn();
   
   void IOTICAT_signInBoss(unsigned char inHour, unsigned char inMinute);
   
   void IOTICAT_signOutBoss(void);
   
#endif