#include  "msp430.h"
#include  "functions.h"
#include  "macros.h"


volatile int LeftMotorPower = 0;
volatile int RightMotorPower = 0;
int ProgramState = 0;
int OldState = -1;

char IsLight = 1;
char WasLight = 1;

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