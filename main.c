/******************************************************************************
Main

DESC: This file contains the Main Routine - "While" Operating System

GLOBALS: Last_SW1_State and Last_SW2_State are defined;
display_1 and display_2 are changed

Nathan Hansen, Feb 2014, Built with IAR Embedded Workbench Version: 5.60.5
******************************************************************************/


//Include Statements
#include  "msp430.h"
#include  "functions.h"
#include  "macros.h"


// Shared Globals
//char *display_1; now specified in functions.h and defined in LCD
//char *display_2; now specified in functions.h and defined in LCD

// Global Variables
volatile unsigned char control_state[CNTL_STATE_INDEX];
volatile unsigned int Time_Sequence = RESET_TIME;
char led_smclk;
volatile char one_time;



//******************************************************************************
// Main Program
// This is the main routine for the program. Execution of code starts here.
// The operating system is Back Ground Fore Ground.
//******************************************************************************

void main(void)
{
	Init_Ports();                             // Initialize Ports
	Init_Clocks();                            // Initialize Clock System 
	Init_Conditions();
	Init_Interrupts();
       
	//PJOUT |= LED1;                          // Turn LED 1 on to indicate boot
	  
	TimeMsec = RESET_TIME;
	Init_Timers(); 				// Initialize Timers
	Init_LCD();				// Initialize LCD
	//Init_LEDs();                            // Initialize LEDs       
        
        
        
	//          "0123456789abcdef"
	display_1 = "   Project  7   ";
	display_2 = "                ";
	Display_Process();
	
	//P1OUT |= IR_LED;
        //waitMsec(10);
	
/* ---------- Begining of the "While" Operating System ------------- */
	while(ALWAYS) 										// Can the Operating system run
	{                            
		if(TimeMsec % EVERY_50 == RESET_TIME)		
			Display_Process();
                
		
	}
}

