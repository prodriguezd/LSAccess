#include <xc.h>
#include "timer.h"
#include "LCD.h"
#include "SIO.h"
#include "keyboard.h"
#include "smsDictionary.h"
#include "ioticat.h"
#include "pwm.h"
#include "speaker.h"
#include "AccessController.h"

// Configuration Bits - 24.1 del Datasheet 
_CONFIG2(		IESO_OFF				// Two Speed Start-up               DISABLED
			&	SOSCSEL_LPSOSC       		// Low Power Secondary Oscillator   ENABLE
			&	WUTSEL_LEG           		// Legacy Wake-up timer selected
		 	&	FNOSC_FRCPLL        		// Fast RC oscillator w/ divide and PLL
		 	& 	FCKSM_CSDCMD			// Clock switching and clock monitor
			&	OSCIOFNC_ON			// OSCO/RC15 function               RC15
			&	IOL1WAY_OFF			// Una configuraciÛ per I/O remapejables deshabilitat
			&	POSCMOD_NONE			// Primary disabled
			& 	I2C1SEL_SEC
);



_CONFIG1(		JTAGEN_OFF                              // JTAG                             DISABLED
			&	GCP_OFF				// Code Protect                     DISABLED
			&	GWRP_OFF			// Write Protect                    DISABLED
			&	BKBUG_OFF			// Background Debugger              DISABLED
            &	ICS_PGx1			// ICD pins select share            PGC1/PGD1
			&	FWDTEN_OFF			// Watchdog Timer                   DISABLED
			&	WINDIS_OFF			// Windowed WDT                     DISABLED
			&	FWPSA_PR128			// Watchdog prescaler	1:128
			&	WDTPS_PS2048			// Watchdog postscale	1:2048      Pre=128 i Post=2048 --> WatchDog Timer = 8seg
);

void initCPU(){
    CLKDIV=0x0000;          // Divisió del clock pel timer i CPU per 1
 //   OSCCON=0x0020;
    RCONbits.SWDTEN = 0;    // Desactivem el Watchdog
}

int main(void) {
    
    initCPU();
    TIMER_init();
    KEYBOARD_init();
    SiInit();
    
    LcInit(2, 16);
    LcClear();
    LcCursorOff();
    LcGotoXY(0,0);
    SPEAKER_init();
    initLCD();
    SMSDICTIONARY_init();
    IOTICAT_init();
    //someInits();
    
    PWM_init();
    initAccessController();
 
    
    while(1) {
        KEYBOARD_motor(); 
        motorAccessController();
        if (isReset() == 1) {
            LcClear();
            initLCD();
            IOTICAT_init();
            initAccessController();
            SPEAKER_init();
        }
        IOTICAT_motor();
        motorTime();
        motorLCD();
        PWM_motor();
        SPEAKER_motor();
        MotorSIO();
        
        
           
    }
    
    return 0;
}
