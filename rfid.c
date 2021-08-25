/* 
 * File:   rfid.c
 * Author: xoselolo
 *
 * Created on 9 de mayo de 2020, 11:37
 */

#include <xc.h>
#include <stdio.h>

#include "rfid.h"

//#include <16F887.h> 
//#FUSES NOWDT,NOBROWNOUT,NOLVP
//#use delay(clock=8000000)
//#use rs232(baud=9600,parity=N,xmit=PIN_C6,rcv=PIN_C7,bits=8)
#define DELAY 10

//pin 24 -> sda
//pi 23 -> sck
//pin 22 -> si
//pin 21 -> so

#define MFRC522_CS  1          //sda            
#define MFRC522_SCK 2          // CLOCK
#define MFRC522_SI  3          // MASTER out - SLAVE in
#define MFRC522_SO  4          // MASTER in - SLAVE out
#define MFRC522_RST 5          // RESET

// Constantes de iniciode cada función
#define START_READ_CARD_SERIAL  11
#define START_ANTI_COLL         16
#define START_CLEAR             33
#define START_SET               28
#define START_WRITE             38
#define START_READ              62
#define START_TOCARD            85
#define START_HALT              130
#define START_CRC               141

// Variables internas
static unsigned short state = 0;
static unsigned char next_state_clear = 0;
static unsigned char next_state_set = 0;
static unsigned char next_state_wr = 0;
static unsigned char next_state_rd = 0;
static unsigned char next_state_tocard = 0;
static unsigned char next_state_crc = 0;
// Variables de salida
static unsigned char hay_tarjeta = 0;
static char tarjeta[UID_LENGTH];
// -- motor
static char motor_UID[16];
static char motor_TagType[16];
static char motor_i;
static char motor_in;
// -- read
static unsigned int read_i;
static unsigned int read_ucAddr;
static unsigned int read_ucResult = 0;
static unsigned char read_address;
// -- write
static unsigned char write_i;
static unsigned char write_ucAddr;
static unsigned char write_address;
static unsigned char write_value;
// -- clear bit
static unsigned char clear_tmp = 0x0;
static char clear_addr;
static char clear_mask;
// -- set bit
static unsigned char set_tmp = 0x0;
static char set_addr;
static char set_mask;
// -- antenna on
static unsigned char on_stt;
// -- output bit
static unsigned char output_pin;
static unsigned char output_value;
// -- to card
static char tocard_status = MI_ERR;
static char tocard_irqEn = 0x00;
static char tocard_waitIRq = 0x00;
static char tocard_lastBits;
static char tocard_n;
static unsigned tocard_i;
static unsigned char tocard_aux;
static char tocard_command;
static char* tocard_sendData;
static char tocard_sendLen;
static char* tocard_backData;
static unsigned* tocard_backLen;
// -- request
static char request_status;
static unsigned request_backBits;
// -- crc
static char crc_i;
static char crc_n;
static char* crc_dataIn;
static char crc_length;
static char* crc_dataOut;
// -- select
//static char select_i;
//static char select_status;
//static char select_size;
//static unsigned select_recvBits;
//static char select_buffer[9];
// -- halt
static unsigned halt_unLen;
static char halt_buff[4];
// -- anti coll
static char anti_status;
static char anti_i;
static char anti_serNumCheck = 0;
static unsigned anti_unLen;
// -- read card serial
static char readcard_status;
// TOTAL: 80 BYTES

void MFRC522_Init(){
    TRISBbits.TRISB13 = 0;
    TRISBbits.TRISB12 = 0;
    TRISBbits.TRISB11 = 0;
    TRISBbits.TRISB10 = 1;
    TRISBbits.TRISB5 = 0;
    
    AD1PCFGbits.PCFG12 = 1;
    
    output_pin = MFRC522_CS;
    output_value = 1;
    output_bit();
    
    output_pin = MFRC522_RST;
    output_value = 1;
    output_bit();

    MFRC522_Reset();
    
    write_address = TMODEREG;
    write_value	= 0x8D;
    MFRC522_Wr();      //Tauto=1; f(Timer) = 6.78MHz/TPreScaler
    
    write_address = TPRESCALERREG;
    write_value	= 0x3E;
    MFRC522_Wr(); //TModeReg[3..0] + TPrescalerReg
    
    write_address = TRELOADREGL;
    write_value	= 30;
    MFRC522_Wr();
    
    write_address = TRELOADREGH;
    write_value	= 0;
    MFRC522_Wr();
    
    write_address = TXAUTOREG;
    write_value	= 0x40;
    MFRC522_Wr();    //100%ASK
    
    write_address = MODEREG;
    write_value	= 0x3D;
    MFRC522_Wr();      // CRC valor inicial de 0x6363


    MFRC522_AntennaOff() ;            
    MFRC522_AntennaOn();
    
    
    state = 0;
    hay_tarjeta = 0;
}

/* main
    MFRC522_Init();
    while (1){   
        MotorSIO();
        if (MFRC522_isCard () && motor_in){ 
           SiPuts('A');
           motor_in = 0;
           //Read ID (E10)
           if (MFRC522_ReadCardSerial ()){
                SiPuts(motor_UID);
           }     
           MFRC522_Halt () ;
        }
    }
 */

void RFID_motor(){                                      
    switch(state){
        case 0:
            if(hay_tarjeta == 0){
                state = 1;
            }
            break;
        case 1:
            write_address = BITFRAMINGREG;
            write_value	= 0x07;
            next_state_wr = 3;
            state = 2;
            break;
        case 2:
            // WRITE
            next_state_wr = 3;
            state = START_WRITE;
            break;
        case 3:
            motor_TagType[0] = PICC_REQIDL;
            state = 4;
            break;
        case 4:
            tocard_command = PCD_TRANSCEIVE;
            tocard_sendData = motor_TagType;
            tocard_sendLen = 1;
            state = 5;
            break;
        case 5:
            tocard_backData = motor_TagType;
            tocard_backLen = &request_backBits;
            state = 6;
            next_state_tocard = 7;
            break;
        case 6:
            state = START_TOCARD;
            // TO_CARD
            break;
        case 7:
            request_status = tocard_status;
            if((request_status != MI_OK) || (request_backBits != 0x10)){
                
                request_status = MI_ERR;
                state = 8;
            }else{
                state = 9;
            }
            break;
        case 8:
            state = 9;
            break;
        case 9:
            if(request_status != MI_OK){
                state = 0;
                
            }else{
                state = 11;
            }
            break;
        case 10:
            // HALT
            state = START_HALT;
            break;
        case 11:
            // ANTI_COLL
            state = 16; 
            break;
        case 12:
            readcard_status = anti_status;
            motor_UID[5] = 0;
            state = 13;
            break;
        case 13:
            if(readcard_status == MI_OK){
                tarjeta[0] = motor_UID[0];
                tarjeta[1] = motor_UID[1];
                state = 14;
            }else{
                state = 10;
            }
            break;
        case 14:
            tarjeta[2] = motor_UID[2];
            tarjeta[3] = motor_UID[3];
            state = 15;
            break;
        case 15:
            tarjeta[4] = motor_UID[4];
            hay_tarjeta = 1;
            state = 10;
            break;
        case 16:
            write_address = BITFRAMINGREG;
            write_value	= 0x00;
            next_state_wr = 18;
            state = 17;
            break;
        case 17:
            // WRITE
            
            state = START_WRITE;
            break;
        case 18:
            motor_UID[0] = PICC_ANTICOLL;
            motor_UID[1] = 0x20;
            state = 19;
            break;
        case 19:
            clear_addr = STATUS2REG;
            clear_mask = 0x08;
            next_state_clear = 21;
            state = 20;
            break;
        case 20:
            state = 33; // CLEAR_BIT
            break;
        case 21:
            tocard_command = PCD_TRANSCEIVE;
            tocard_sendData = motor_UID;
            state = 22;
            break;
        case 22:
            tocard_sendLen = 2;
            tocard_backData = motor_UID;
            state = 23;
            break;
        case 23:
            tocard_backLen = &anti_unLen;
            state = 24;
            next_state_tocard = 25;
            break;
        case 24:
            // TO_CARD
            state = START_TOCARD;
            break;
        case 25:
            anti_status = tocard_status;
            if(anti_status == MI_OK){
                anti_i = 0;
                state = 26;
            }else{
                state = 12;
            }
            break;
        case 26:
            if(anti_i < 4){
                anti_serNumCheck ^= motor_UID[anti_i];
                anti_i++;
            }else{
                if(anti_serNumCheck != motor_UID[4] ){
                    anti_status = MI_ERR;
                    state = 27;
                }else{
                    state = 12;
                }
            }
            break;
        case 27:
            state = 12;
            break;
        case 28:
            read_address = set_addr;
            next_state_rd = 30;
            state = 29;
            break;
        case 29:
            state = START_READ;
            // READ
            break;
        case 30:
            set_tmp = read_ucResult;
            write_address = set_addr;
            write_value	= set_tmp|set_mask;
            next_state_wr = 32;
            state = 31;
            break;
        case 31:
            // WRITE
            state = START_WRITE;
            break;
        case 32:
            state = next_state_set;
            break;
        case 33:
            read_address = clear_addr;
            next_state_rd = 35;
            state = 34;
            break;
        case 34:
            // READ
            state = START_READ;
            break;
        case 35:
            clear_tmp = read_ucResult;
            write_address = clear_addr;
            write_value	= clear_tmp&~clear_mask;
            next_state_wr = 37;
            state = 36;
            break;
        case 36:
            // WRITE
            state = START_WRITE;
            break;
        case 37:
            state = next_state_clear;
            break;
        case 38:
            output_pin = MFRC522_SCK;
            output_value = 0;
            state = 39;
            break;
        case 39:
            output_bit(); // OUTPUT_BIT
            state = 40;
            break;
        case 40:
            output_pin = MFRC522_CS;
            output_value = 0;
            state = 41;
            break;
        case 41:
            output_bit(); // OUTPUT_BIT
            state = 42;
            break;
        case 42:
            write_ucAddr = ((write_address << 1) & 0x7E);
            write_i = 8;
            state = 43;
            break;
        case 43:
            if(write_i > 0){
                output_pin = MFRC522_SI;
                output_value = (write_ucAddr&0x80) == 0x80;
                state = 44;
            }else{
                write_i = 8;
                state = 50;
            }
            break;
        case 44:
            output_bit(); // OUTPUT_BIT
            state = 45;
            break;
        case 45:
            output_pin = MFRC522_SCK;
            output_value = 1;
            state = 46;
            break;
        case 46:
            output_bit(); // OUTPUT_BIT
            state = 47;
            break;
        case 47:
            write_ucAddr <<= 1;
            output_pin = MFRC522_SCK;
            output_value = 0;
            state = 48;
            break;
        case 48:
            output_bit(); // OUTPUT_BIT
            state = 49;
            break;
        case 49:
            write_i --;
            state = 43;
            break;
        case 50:
            if(write_i > 0){
                output_pin = MFRC522_SI;
                output_value = (write_value & 0x80) == 0x80;
                state = 51;
            }else{
                state = 57;
            }
            break;
        case 51:
            output_bit(); // OUTPUT_BIT
            state = 52;
            break;
        case 52:
            output_pin = MFRC522_SCK;
            output_value = 1;
            state = 53;
            break;
        case 53:
            output_bit(); // OUTPUT_BIT
            state = 54;
            break;
        case 54:
            write_value <<= 1;
            output_pin = MFRC522_SCK;
            output_value = 0;
            state = 55;
            break;
        case 55:
            output_bit(); // OUTPUT_BIT
            state = 56;
            break;
        case 56:
            write_i --;
            state = 50;
            break;
        case 57:
            output_pin = MFRC522_CS;
            output_value = 1;
            state = 58;
            break;
        case 58:
            output_bit(); // OUTPUT_BIT
            state = 59;
            break;
        case 59:
            output_pin = MFRC522_SCK;
            output_value = 1;
            state = 60;
            break;
        case 60:
            output_bit(); // OUTPUT_BIT
            state = 61;
            break;
        case 61:
            state = next_state_wr;
            break;
        case 62:
            output_pin = MFRC522_SCK;
            output_value = 0;
            state = 63;
            break;
        case 63:
            output_bit ();
            state = 64;
            break;
        case 64:
            output_pin = MFRC522_CS;
            output_value = 0;
            state = 65;
            break;
        case 65:
            output_bit ();
            state = 66;
            break;
        case 66:
            read_ucAddr = ((read_address << 1) & 0x7E) | 0x80;
            read_i = 8;
            state = 67;
            break;
        case 67:
            if(read_i > 0){
                output_pin = MFRC522_SI;
                output_value = (read_ucAddr & 0x80) == 0x80;
                state = 68;
            }else{
                read_i = 8;
                state = 74;
            }
            break;
        case 68:
            output_bit ();
            state = 69;
            break;
        case 69:
            output_pin = MFRC522_SCK;
            output_value = 1;
            state = 70;
            break;
        case 70:
            output_bit ();
            state = 71;
            break;
        case 71:
            read_ucAddr <<= 1;
            output_pin = MFRC522_SCK;
            output_value = 0;
            state = 72;
            break;
        case 72:
            output_bit ();
            state = 73;
            break;
        case 73:
            read_i --;
            state = 67;
            break;
        case 74:
            if(read_i > 0){
                output_pin = MFRC522_SCK;
                output_value = 1;
                state = 75;
            }else{
                state = 80;
            }
            break;
        case 75:
            output_bit ();
            state = 76;
            break;
        case 76:
            read_ucResult <<= 1;
            read_ucResult|= PORTBbits.RB10;
            state = 77;
            break;
        case 77:
            output_pin = MFRC522_SCK;
            output_value = 0;
            state = 78;
            break;
        case 78:
            output_bit ();
            state = 79;
            break;
        case 79:
            read_i --;
            state = 74;
            break;
        case 80:
            output_pin = MFRC522_CS;
            output_value = 1;
            state = 81;
            break;
        case 81:
            output_bit ();
            state = 82;
            break;
        case 82:
            output_pin = MFRC522_SCK;
            output_value = 1;
            state = 83;
            break;
        case 83:
            output_bit ();
            state = 84;
            break;
        case 84:
            state = next_state_rd;
            break;
        case 85:
            switch (tocard_command){
                case PCD_AUTHENT:       //Certification cards close
                    // E86
                    tocard_irqEn = 0x12;
                    tocard_waitIRq = 0x10;
                    break;

                case PCD_TRANSCEIVE:    //Transmit FIFO data
                    // E87
                    tocard_irqEn = 0x77;
                    tocard_waitIRq = 0x30;
                    break;

                default:
                    break;
            }
            state = 86;
            break;
        case 86:
            write_address = COMMIENREG;
            write_value	= tocard_irqEn | 0x80;
            next_state_wr = 88;
            state = 87;
            break;
        case 87:
            // WRITE
            state = START_WRITE;
            break;
        case 88:
            
            clear_addr = COMMIRQREG;
            clear_mask = 0x80;
            next_state_clear = 90;
            state = 89;
            break;
        case 89:
            // CLEAR_BIT
            
            state = START_CLEAR;
            break;
        case 90:
            set_addr = FIFOLEVELREG;
            set_mask = 0x80;
            next_state_set = 92;
            state = 91;
            break;
        case 91:
            // SET_BIT
            state = START_SET;
            break;
        case 92:
            write_address = COMMANDREG;
            write_value	= PCD_IDLE;
            next_state_wr = 94;
            state = 93;
            break;
        case 93:
            // WRITE
            state = START_WRITE;
            break;
        case 94:
            tocard_i = 0;
            state = 95;
            break;
        case 95:
            if(tocard_i < tocard_sendLen){
                state = 96;
            }else{
                state = 99;
            }
            break;
        case 96:
            write_address = FIFODATAREG;
            write_value	= tocard_sendData[tocard_i];
            next_state_wr = 98;
            state = 97;
            break;
        case 97:
            // WRITE
            state = START_WRITE;
            break;
        case 98:
            tocard_i ++;
            state = 95;
            break;
        case 99:
            write_address = COMMANDREG;
            write_value	= tocard_command;
            next_state_wr = 101;
            state = 100;
            break;
        case 100:
            // WRITE
            state = START_WRITE;
            break;
        case 101:
            if(tocard_command == PCD_TRANSCEIVE){
                set_addr = BITFRAMINGREG;
                set_mask = 0x80;
                next_state_set = 103;
                state = 102;
            }else{
                state = 103;
            }
            break;
        case 102:
            // SET_BIT
            state = START_SET;
            break;
        case 103:
            tocard_i = 0xFFFF; 
            state = 104;
            break;
        case 104:
            read_address = COMMIRQREG;
            next_state_rd = 106;
            state = 105;
            break;
        case 105:
            // READ
            state = START_READ;
            break;
        case 106:
            
            tocard_n = read_ucResult;
            tocard_i--;
            state = 107;
            break;
        case 107:
            if(tocard_i && !(tocard_n & 0x01) && !( tocard_n & tocard_waitIRq)){
                state = 104;
            }else{
                state = 108;
            }
            break;
        case 108:
            clear_addr = BITFRAMINGREG;
            clear_mask = 0x80;
            next_state_clear = 110;
            state = 109;
            break;
        case 109:
            // CLEAR
            state = START_CLEAR;
            break;
        case 110:
            if(tocard_i != 0){
                read_address = ERRORREG;
                next_state_rd = 112;
                state = 111;
            }else{
                state = 300;
            }
            break;
        case 111:
            // READ
            state = START_READ;
            break;
        case 112:
            tocard_aux = read_ucResult;
            if(!(tocard_aux & 0x1B)){
                tocard_status = MI_OK;
                state = 113;
            }else{
                tocard_status = MI_ERR;
                state = 300;
            }
            break;
        case 113:
            if(tocard_n & tocard_irqEn & 0x01){
                tocard_status = MI_NOTAGERR;
            }
            state = 114;
            break;
        case 114:
            if(tocard_command == PCD_TRANSCEIVE){
                read_address = FIFOLEVELREG;
                next_state_rd = 116;
                state = 115;
            }else{
                state = 300;
            }
            break;
        case 115:
            // READ
            state = START_READ;
            break;
        case 116:
            tocard_n = read_ucResult;
            read_address = CONTROLREG;
            next_state_rd = 118;
            state = 117;
            break;
        case 117:
            // READ
            state = START_READ;
            break;
        case 118:
            tocard_lastBits = read_ucResult;
            tocard_lastBits &= 0x07;
            if(tocard_lastBits){
                state = 119;
            }else{
                state = 122;
            }
            break;
        case 119:
            *tocard_backLen = tocard_n - 1;
            state = 120;
            break;
        case 120:
            *tocard_backLen *= 8;
            state = 121;
            break;
        case 121:
            *tocard_backLen += tocard_lastBits;
            state = 123;
            break;
        case 122:
            *tocard_backLen = tocard_n * 8;
            state = 123;
            break;
        case 123:
            if(tocard_n == 0){
                tocard_n = 1;
            }
            state = 124;
            break;
        case 124:
            if(tocard_n > 16){
                tocard_n = 16;
            }
            state = 125;
            break;
        case 125:
            tocard_i = 0;
            state = 126;
            break;
        case 126:
            if(tocard_i < tocard_n){
                read_address = FIFODATAREG;
                next_state_rd = 128;
                state = 127;
            }else{
                state = 129;
            }
            break;
        case 127:
            // READ
            state = START_READ;
            break;
        case 128:
            tocard_backData[tocard_i] = read_ucResult;
            tocard_i ++;
            state = 126;
            break;
        case 129:
            tocard_backData[tocard_i] = 0;
            state = 300;
            break;
        case 300:
            
            state = next_state_tocard;
            break;
        case 130:
            halt_buff[0] = PICC_HALT;
            halt_buff[1] = 0;
            crc_dataIn = halt_buff;
            state = 131;
            break;
        case 131:
            crc_length = 2;
            crc_dataOut = &halt_buff[2];
            next_state_crc = 133;
            state = 132;
            break;
        case 132:
            // CRC
            state = START_CRC;
            break;
        case 133:
            clear_addr = STATUS2REG;
            clear_mask = 0x80;
            state = 134;
            next_state_clear = 135;
            break;
        case 134:
            // CLEAR
            state = START_CLEAR;
            break;
        case 135:
            tocard_command = PCD_TRANSCEIVE;
            tocard_sendData = halt_buff;
            tocard_sendLen = 4;
            state = 136;
            break;
        case 136:
            tocard_backData = halt_buff;
            tocard_backLen = &halt_unLen;
            state = 137;
            next_state_tocard = 138;
            break;
        case 137:
            // TO_CARD
            state = START_TOCARD;
            break;
        case 138:
            clear_addr = STATUS2REG;
            clear_mask = 0x08;
            state = 139;
            next_state_clear = 140;
            break;
        case 139:
            // CLEAR
            state = START_CLEAR;
            break;
        case 140:
            state = 0;
            break;
        case 141:
            clear_addr = DIVIRQREG;
            clear_mask = 0x04;
            next_state_clear = 143;
            state = 142;
            break;
        case 142:
            // CLEAR
            state = START_CLEAR;
            break;
        case 143:
            set_addr = FIFOLEVELREG;
            set_mask = 0x80;
            next_state_set = 145;
            state = 144;
            break;
        case 144:
            // SET_BIT
            state = START_SET;
            break;
        case 145:
            crc_i = 0;
            state = 146;
            break;
        case 146:
            if(crc_i < crc_length){
                write_address = FIFODATAREG;
                write_value	= *crc_dataIn++;
                next_state_wr = 148;
                state = 147;
            }else{
                state = 149;
            }
            break;
        case 147:
            // WRITE
            state = START_WRITE;
            break;
        case 148:
            crc_i ++;
            state = 146;
            break;
        case 149:
            write_address = COMMANDREG;
            write_value	= PCD_CALCCRC;
            next_state_wr = 151;
            state = 150;
            break;
        case 150:
            state = START_WRITE;
            break;
        case 151:
            crc_i = 0xFF;
            state = 152;
            break;
        case 152:
            read_address = DIVIRQREG;
            state = 153;
            next_state_rd = 154;
            break;
        case 153:
            // READ
            state = START_READ;
            break;
        case 154:
            crc_n = read_ucResult;
            crc_i --;
            state = 155;
            break;
        case 155:
            if(crc_i && !(crc_n & 0x04)){
                state = 152;
            }else{
                state = 156;
            }
            break;
        case 156:
            read_address = CRCRESULTREGL;
            state = 157;
            next_state_rd = 158;
            break;
        case 157:
            // READ
            state = START_READ;
            break;
        case 158:
            crc_dataOut[0] = read_ucResult;
            read_address = CRCRESULTREGM;
            next_state_rd = next_state_crc;
            state = 159;
            break;
        case 159:
            // READ
            state = START_READ;
            break;
        case 160:
            crc_dataOut[1] = read_ucResult;
            state = next_state_crc;
            break;
    }
}

// -------------- functions --------------
unsigned char MFRC522_Rd(){
    //TRISBbits.TRISB10 = 1;
    // E62
    output_pin = MFRC522_SCK;
    output_value = 0;
    // E63
    output_bit ();            // pin A1 OUTPUT
    
    // E64
    output_pin = MFRC522_CS;
    output_value = 0;
    // E65
    output_bit ();             // pin A0 OUTPUT
    
    // E66 E67
    read_ucAddr = ((read_address << 1) & 0x7E) | 0x80;

    // E67
    for (read_i = 8; read_i > 0; read_i--){
        output_pin = MFRC522_SI;
        output_value = (read_ucAddr & 0x80) == 0x80;
        // E68
        output_bit ();
        
        // E69
        output_pin = MFRC522_SCK;
        output_value = 1;
        // E70
        output_bit ();
        
        // E71
        read_ucAddr <<= 1;
        output_pin = MFRC522_SCK;
        output_value = 0;
        
        // E72
        output_bit ();
        
        // E73
    }

    // E74
    for (read_i = 8; read_i > 0; read_i--){
        output_pin = MFRC522_SCK;
        output_value = 1;
        
        // E75
        output_bit ();
        
        // E76
        read_ucResult <<= 1;
        read_ucResult|= PORTBbits.RB10;
        
        // E77
        output_pin = MFRC522_SCK;
        output_value = 0;
        
        // E78
        output_bit ();
        
        // E79
    }

    // E80
    output_pin = MFRC522_CS;
    output_value = 1;
    // E81
    output_bit ();
    
    // E82
    output_pin = MFRC522_SCK;
    output_value = 1;
    
    // E83
    output_bit ();
    
    // E84
    return read_ucResult;
}

void MFRC522_Wr(){
    // E38
    output_pin = MFRC522_SCK;
    output_value = 0;
    // E39
    output_bit ();
    
    // E40
    output_pin = MFRC522_CS;
    output_value = 0;
    // E41
    output_bit ();
    
    // E42
    write_ucAddr = ((write_address << 1) & 0x7E);
    
    // E43
    for (write_i = 8; write_i > 0; write_i--){
    
        // E43
        output_pin = MFRC522_SI;
        output_value = (write_ucAddr&0x80) == 0x80;
        // E44
        output_bit ();
        
        // E45
        output_pin = MFRC522_SCK;
        output_value = 1;
        // E46
        output_bit ();
        
        // E47
        write_ucAddr <<= 1;
        output_pin = MFRC522_SCK;
        output_value = 0;
        // E48
        output_bit ();
        // E49
    }

    // E50
    for (write_i = 8; write_i > 0; write_i--){
        // E50
        output_pin = MFRC522_SI;
        output_value = (write_value & 0x80) == 0x80;
        // E51
        output_bit ();
        
        // E52
        output_pin = MFRC522_SCK;
        output_value = 1;
        // E53
        output_bit ();
        
        // E54
        write_value <<= 1;
        output_pin = MFRC522_SCK;
        output_value = 0;
        // E55
        output_bit ();
        // E56
    }

    // E57
    output_pin = MFRC522_CS;
    output_value = 1;
    // E58
    output_bit ();

    // E59
    output_pin = MFRC522_SCK;
    output_value = 1;
    // E60
    output_bit ();
    // E61
}

void MFRC522_Clear_Bit(){
    // E33
    read_address = clear_addr;
    
    // E34
    clear_tmp = MFRC522_Rd();
    
    // E35
    write_address = clear_addr;
    write_value	= clear_tmp&~clear_mask;
    
    // E36
    MFRC522_Wr();
}

void MFRC522_Set_Bit(){
    // E28
    read_address = set_addr;

    // E29
    set_tmp = MFRC522_Rd();
    
    // E30
    write_address = set_addr;
    write_value	= set_tmp|set_mask;
    
    // E31
    MFRC522_Wr();
}

void MFRC522_Reset(){
    output_pin = MFRC522_RST;
    output_value = 1;
    output_bit ();
    
    delay_us (1);
    
    output_pin = MFRC522_RST;
    output_value = 0;
    output_bit ();
    
    delay_us (1);
    
    output_pin = MFRC522_RST;
    output_value = 1;
    output_bit () ;
    
    delay_us (1);
    
    write_address = COMMANDREG;
    write_value	= PCD_RESETPHASE;
    MFRC522_Wr();
    
    delay_us (1);
}

void MFRC522_AntennaOn(){
    read_address = TXCONTROLREG;
    on_stt = MFRC522_Rd();
    
    set_addr = TXCONTROLREG;
    set_mask = 0x03;
    MFRC522_Set_Bit();
}

void MFRC522_AntennaOff(){
    clear_addr = TXCONTROLREG;
    clear_mask = 0x03;
    MFRC522_Clear_Bit();                                          
}

void output_bit(){
    
    switch(output_pin){
        case MFRC522_CS:
            LATBbits.LATB13 = output_value;
            break;
        case MFRC522_SCK:
            LATBbits.LATB12 = output_value;
            break;
        case MFRC522_SI:
            LATBbits.LATB11 = output_value;
            break;
        
        case MFRC522_RST:
            LATBbits.LATB5 = output_value;
            break;
    }
}

void delay_us(unsigned char f){
    int a = 0;
}

char MFRC522_ToCard(){
    // E85
    switch (tocard_command){
        case PCD_AUTHENT:       //Certification cards close
            // E86
            tocard_irqEn = 0x12;
            tocard_waitIRq = 0x10;
            break;
        
        case PCD_TRANSCEIVE:    //Transmit FIFO data
            // E87
            tocard_irqEn = 0x77;
            tocard_waitIRq = 0x30;
            break;
       
        default:
            break;
    }
    
    // E86
    write_address = COMMIENREG;
    write_value	= tocard_irqEn | 0x80;
    // E87
    MFRC522_Wr();  //Interrupt request
    
    // E88
    clear_addr = COMMIRQREG;
    clear_mask = 0x80;
    // E89
    MFRC522_Clear_Bit();   //Clear all interrupt request bit
    
    // E90
    set_addr = FIFOLEVELREG;
    set_mask = 0x80;
    // E91
    MFRC522_Set_Bit();   //FlushBuffer=1, FIFO Initialization
    
    // E92
    write_address = COMMANDREG;
    write_value	= PCD_IDLE;
    // E93
    MFRC522_Wr();      //NO action; Cancel the current command???

    // E94 E95
    for (tocard_i = 0; tocard_i < tocard_sendLen; tocard_i++){
        // E96
        write_address = FIFODATAREG;
        write_value	= tocard_sendData[tocard_i];
        
        // E97
        MFRC522_Wr();
        // E98
    }

    // E99
    write_address = COMMANDREG;
    write_value	= tocard_command;
    // E100
    MFRC522_Wr();
    
    // E101
    if (tocard_command == PCD_TRANSCEIVE ){
        set_addr = BITFRAMINGREG;
        set_mask = 0x80;
        // E102
        MFRC522_Set_Bit(); //StartSend=1,transmission of data starts 
    }

    // E103
    tocard_i = 0xFFFF;  
    do{
        // E104
        read_address = COMMIRQREG;
        // E105
        tocard_n = MFRC522_Rd();
        // E106
        tocard_i--;
        // E107
    }while (tocard_i && !(tocard_n & 0x01) && !( tocard_n & tocard_waitIRq ) );
    
    // E108
    clear_addr = BITFRAMINGREG;
    clear_mask = 0x80;
    // E109
    MFRC522_Clear_Bit();
    
    // E110
    if (tocard_i != 0){
        read_address = ERRORREG;
        // E111
        tocard_aux = MFRC522_Rd();
        // E112
        if(!(tocard_aux & 0x1B)){
            tocard_status = MI_OK;
            // E113
            if (tocard_n & tocard_irqEn & 0x01){
                tocard_status = MI_NOTAGERR;      
            }
            // E114
            if (tocard_command == PCD_TRANSCEIVE){
                read_address = FIFOLEVELREG;
                // E115
                tocard_n = MFRC522_Rd();
                // E116
                read_address = CONTROLREG;
                //tocard_lastBits = MFRC522_Rd() & 0x07;
                // E117
                tocard_lastBits = MFRC522_Rd();
                // E118
                tocard_lastBits &= 0x07;
                if (tocard_lastBits){
                    //*tocard_backLen = (tocard_n-1) * 8 + tocard_lastBits;
                    // E119
                    *tocard_backLen = tocard_n - 1;
                    // E120
                    *tocard_backLen *= 8;
                    // E121
                    *tocard_backLen += tocard_lastBits;
                }
                else{
                    // E122
                    *tocard_backLen = tocard_n * 8;
                }
                // E123
                if (tocard_n == 0){
                    tocard_n = 1;
                }
                // E124
                if (tocard_n > 16){
                    tocard_n = 16;
                }
                // E125 E126
                for (tocard_i = 0; tocard_i < tocard_n; tocard_i++){
                    read_address = FIFODATAREG;
                    // E127
                    tocard_backData[tocard_i] = MFRC522_Rd();
                    // E128
                }
                // E129
                tocard_backData[tocard_i] = 0;
            }
        }else{
            // E200
            tocard_status = MI_ERR;
        }
    }

    // E300
    return tocard_status;
}

char MFRC522_Request(){
    // --- E1
    write_address = BITFRAMINGREG;
    write_value	= 0x07;
    
    // E2
    MFRC522_Wr();
    
    // E3
    motor_TagType[0] = PICC_REQIDL;
    
    // E4
    tocard_command = PCD_TRANSCEIVE;
    tocard_sendData = motor_TagType;
    tocard_sendLen = 1;
    
    // E5
    tocard_backData = motor_TagType;
    tocard_backLen = &request_backBits;
    
    // E6
    request_status = MFRC522_ToCard();
    
    // E7
    if ((request_status != MI_OK) || (request_backBits != 0x10)){
      request_status = MI_ERR;
      // E8
    }
    // E9
    return request_status;
}

void MFRC522_CRC(){
    // E141
    clear_addr = DIVIRQREG;
    clear_mask = 0x04;
    // E142
    MFRC522_Clear_Bit();
    
    // E143
    set_addr = FIFOLEVELREG;
    set_mask = 0x80;
    // E144
    MFRC522_Set_Bit();   
   
    // E145 E146
    for ( crc_i = 0; crc_i < crc_length; crc_i++ ){
        // E146
        write_address = FIFODATAREG;
        write_value	= *crc_dataIn++;
        // E147
        MFRC522_Wr();  
        // E148
    }
   
    // E149
    write_address = COMMANDREG;
    write_value	= PCD_CALCCRC;
    // E150
    MFRC522_Wr();
    
    // E151
    crc_i = 0xFF;
 
    
    do
    {
        // E152
        read_address = DIVIRQREG;
        // E153
        crc_n = MFRC522_Rd();
        // E154
        crc_i--;
    }// E155
    while( crc_i && !(crc_n & 0x04) );        //CRCIrq = 1
    
    // E156
    read_address = CRCRESULTREGL;
    // E157
    crc_dataOut[0] = MFRC522_Rd();
    // E158
    read_address = CRCRESULTREGM;
    // E159
    crc_dataOut[1] = MFRC522_Rd();     
    // Estado next state crc
}

// Select tag function (never used)
/*
char MFRC522_SelectTag( char *serNum ){
    select_buffer[0] = PICC_SElECTTAG;
    select_buffer[1] = 0x70;
    for (select_i = 2; select_i < 7; select_i++){
        select_buffer[select_i] = *serNum++;
    }
    crc_dataIn = select_buffer;
    crc_length = 7;
    crc_dataOut = &select_buffer[7];
    MFRC522_CRC();            
    tocard_command = PCD_TRANSCEIVE;
    tocard_sendData = select_buffer;
    tocard_sendLen = 9;
    tocard_backData = select_buffer;
    tocard_backLen = &select_recvBits;
    select_status = MFRC522_ToCard();
    
    if ( (select_status == MI_OK) && (select_recvBits == 0x18) ){
        select_size = select_buffer[0];
    }
    else{
        select_size = 0;
    }
    return select_size;
}*/
 

//hibernation
void MFRC522_Halt(){
    // E130
    halt_buff[0] = PICC_HALT;
    halt_buff[1] = 0;
    crc_dataIn = halt_buff;
    // E131
    crc_length = 2;
    crc_dataOut = &halt_buff[2];
    // E132
    MFRC522_CRC();
    
    // E133
    clear_addr = STATUS2REG;
    clear_mask = 0x80;
    // E134
    MFRC522_Clear_Bit();
    
    // E135
    tocard_command = PCD_TRANSCEIVE;
    tocard_sendData = halt_buff;
    tocard_sendLen = 4;
    // E136
    tocard_backData = halt_buff;
    tocard_backLen = &halt_unLen;
    // E137
    MFRC522_ToCard();
    
    // E138
    clear_addr = STATUS2REG;
    clear_mask = 0x08;
    // E139
    MFRC522_Clear_Bit();
    // E140 (go back to state 0)
}

char MFRC522_AntiColl(){
    // E16
    write_address = BITFRAMINGREG;
    write_value	= 0x00;
    
    // E17
    MFRC522_Wr();                //TxLastBists = BitFramingReg[2..0]
    
    // E18
    motor_UID[0] = PICC_ANTICOLL;
    motor_UID[1] = 0x20;
    
    // E19
    clear_addr = STATUS2REG;
    clear_mask = 0x08;
    
    // E20
    MFRC522_Clear_Bit();
    
    // E21
    tocard_command = PCD_TRANSCEIVE;
    tocard_sendData = motor_UID;
    // E22
    tocard_sendLen = 2;
    tocard_backData = motor_UID;
    // E23
    tocard_backLen = &anti_unLen;
    
    // E24
    anti_status = MFRC522_ToCard();
    
    // E25
    if (anti_status == MI_OK){
        // E25 E26
        for (anti_i = 0; anti_i < 4; anti_i++ ){
            anti_serNumCheck ^= motor_UID[anti_i];
        }
        // E26
        if (anti_serNumCheck != motor_UID[4] ){
            anti_status = MI_ERR;
        }
    } // E2
    return anti_status;
}

char MFRC522_isCard(){
    if (MFRC522_Request() == MI_OK)
        return 1;
    else
        return 0;
}

char MFRC522_ReadCardSerial(){
    // E11
    readcard_status = MFRC522_AntiColl();
    // E12
    motor_UID[5] = 0;
 
    // E13
    if (readcard_status == MI_OK) return 1;
    else return 0;
}



// FUNCIONES EXTERNAS
unsigned char hanPasadoTarjeta(void){
    return hay_tarjeta;
}

/**
 * Paula, pasame la tarjeta como parametro, y te la lleno
 * @param toCopyTarjeta
 */
void getTarjeta(char* toCopyTarjeta){
    toCopyTarjeta[0] = tarjeta[0];
    toCopyTarjeta[1] = tarjeta[1];
    toCopyTarjeta[2] = tarjeta[2];
    toCopyTarjeta[3] = tarjeta[3];
    toCopyTarjeta[4] = tarjeta[4];
    hay_tarjeta = 0;
}


