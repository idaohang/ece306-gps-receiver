#include "msp430.h"
#include "macros.h"
#include "functions.h"


void init_gps(void)
{
    PJOUT &= ~GPS_RESET;

    waitMsec(GPS_WAIT);
    PJOUT |= GPS_RESET;

    waitMsec(GPS_WAIT);
    gps_wakeup();

    PJOUT &= ~GPS_RESET;
    waitMsec(GPS_WAIT);
    //PJOUT |= GPS_PWRCNTL;


    while(!(PJIN & GPS_PWRCHK))
    {
        waitMsec(GPS_WAIT);
        gps_wakeup();
    }
      //Wait for it to transition to high
    //Once PWRCHK is high, release GPS_PWRCNTL
    //By setting to low.
    //waitMsec(5000);
    //PJOUT &= ~GPS_PWRCNTL;
}

void gps_wakeup(void){
    waitMsec(GPS_WAIT);
    PJOUT |= GPS_PWRCNTL;

    while(!(PJIN & GPS_PWRCHK)){
        //Wait until GPS_PWRCHK comes on.
    }

    PJOUT &= ~GPS_PWRCNTL;
}