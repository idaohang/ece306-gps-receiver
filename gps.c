#include "msp430.h"
#include "macros.h"
#include "functions.h"

void init_gps(void){
    PJOUT &= ~GPS_PWRCNTL;
    PJOUT |= GPS_PWRCNTL;

    while(!(PJIN & GPS_PWRCHK));
      //Wait for it to transition to high
    //Once PWRCHK is high, release GPS_PWRCNTL
    //By setting to low.
    PJOUT &= GPS_PWRCNTL;
}
