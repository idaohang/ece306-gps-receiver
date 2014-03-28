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

volatile char Last_SW1_State = NOT_PRESSED;
volatile char Last_SW2_State = NOT_PRESSED;

volatile int ADC_Thumb = 0;
volatile int ADC_LeftIR = 0; 
volatile int ADC_RightIR = 0;
char IsLight = 1;
char WasLight = 1;

int AdcChannel = ADC10INCH_0;

volatile int MotorTimer = 0;
volatile int LeftMotorPower = LPOWER;
volatile int RightMotorPower = RPOWER;
volatile int ProgramState = 0;
volatile int OldState = -1;

//                  "0123456789abcde\0"
char TxChars[16] = {"    NCSU #1    \0"};
int TxIndex = 0;
char RxChars[16] = {"    NO DATA    \0"};
int RxIndex = 0;

// Global Variables
volatile unsigned char control_state[CNTL_STATE_INDEX];
volatile unsigned int TimeMsec, Time_Sequence = RESET_TIME;
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
	display_1 = "   Homework 8   ";
	display_2 = "                ";
	Display_Process();
	
	//P1OUT |= IR_LED;
        //waitMsec(10);
	
/* ---------- Begining of the "While" Operating System ------------- */
	while(ALWAYS) 										// Can the Operating system run
	{                            
		if(TimeMsec % EVERY_50 == RESET_TIME)
		{
			for(int i = 0; i < sizeof RxChars; i++)
				display_2[i] = RxChars[i];
		
			Display_Process();					//Refreshes screen every 50 'ticks'
		}
                
		if(TimeMsec % EVERY_2 == RESET_TIME)
			Switches_Process();                 // Poll for switch state change every other 'tick'
	}
}

void Control_Process(void)
{
	IsLight = (ADC_LeftIR + ADC_RightIR)/2 < ADC_Thumb;
        
        if(!IsLight)
	{
		//          "0123456789abcdef"
		display_2 = "       Dark     ";
                
                if(WasLight && (ProgramState == 1 || ProgramState == 3))
                  ProgramState++;
	}
	else
        {
		display_2 = "       Light    ";
        }
        
        if((ProgramState != 1 && ProgramState != 3) && MotorTimer <= 0)
          ProgramState++;
        
        if(OldState != ProgramState)
          switch(ProgramState)
          {
            case 1: 
              allStop();
              waitMsec(500);
              MotorTimer = 10000;
              LeftMotorPower = LPOWER;
              RightMotorPower = RPOWER;
              
              //          "0123456789abcdef"
              display_1 = "Forward to Line ";
              break;
            case 2:
              allStop();
              waitMsec(500);
              LeftMotorPower = -LPOWER;
              RightMotorPower = -RPOWER;
              MotorTimer = 1000;
              
              //          "0123456789abcdef"
              display_1 = " Blind Reverse  ";
              break;
            case 3:
              allStop();
              MotorTimer = 10000;
              LeftMotorPower = -LPOWER;
              RightMotorPower = -RPOWER;
              
              //          "0123456789abcdef"
              display_1 = "Reverse to Line ";
              break;
            case 4: 
              allStop();
              //waitMsec(250);
              MotorTimer = (10000 - MotorTimer + 4000)/2;
              waitMsec(500);
              LeftMotorPower = LPOWER-10;
              RightMotorPower = RPOWER;
              
              //          "0123456789abcdef"
              display_1 = "Forward to Mid  ";
              break;
            case 5:
              allStop();
              waitMsec(500);
              MotorTimer = SPIN_TIME*3;
              LeftMotorPower = LPOWER;
              RightMotorPower = -RPOWER;
              
              //          "0123456789abcdef"
              display_1 = "   Spin CW  3x  ";
              break;
            case 6:
              allStop();
              waitMsec(500);
              MotorTimer = SPIN_TIME*2;
              LeftMotorPower = -LPOWER;
              RightMotorPower = RPOWER;
              
              //          "0123456789abcdef"
              display_1 = "   Spin CCW  2x ";
              break;
            default:
              allStop();
              MotorTimer = 0;
              
              //          "0123456789abcdef"
              display_1 = "     Default    ";
              break;
          }
        
        WasLight = IsLight;
        OldState = ProgramState;
}

void ADC_Process(void)
{
	while (ADC10CTL1 & BUSY); // Wait if ADC10 core is active
	ADC10CTL0 |= ADC10ENC + ADC10SC; // Sampling and conversion start
}