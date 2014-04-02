#include "msp430.h"
#include "macros.h"
#include "functions.h"

void init_gps(void)
{
    PJOUT |= GPS_RESET | GPS_PWR;
    waitMsec(5000);  
    PJOUT &= ~GPS_RESET;
    waitMsec(1000);
    //PJOUT |= GPS_PWRCNTL;
    

    while(!(PJIN & GPS_PWRCHK))
    {
      PJOUT |= GPS_PWRCNTL;
      waitMsec(500);
      PJOUT &= ~GPS_PWRCNTL;
      waitMsec(2000);
    }
      //Wait for it to transition to high
    //Once PWRCHK is high, release GPS_PWRCNTL
    //By setting to low.
    //waitMsec(5000);
    //PJOUT &= ~GPS_PWRCNTL;
}
