#include "msp430.h"
#include "macros.h"
#include "functions.h"

char GpsPacket[GPS_PACKET_SIZE], Lat[LAT_LENGTH], Lon[LON_LENGTH];
char isGPRMC = BEGIN, isGPGGA = BEGIN;


void init_gps(void)
{
    PJOUT &= ~GPS_RESET;

    waitMsec(GPS_WAIT);
    PJOUT |= GPS_RESET;

    while(!(PJIN & GPS_PWRCHK))
    {
        waitMsec(GPS_WAIT);
        PJOUT |= GPS_PWRCNTL;
        waitMsec(GPS_WAIT);
        PJOUT &= ~GPS_PWRCNTL;
    }

    for(int i = BEGIN; i < sizeof GpsPacket; i++)
      GpsPacket[i] = CLEAR_VAL;
    for(int i = BEGIN; i < sizeof Lat; i++)
    {
      Lat[i] = LAT_INIT;
      Lon[i] = LON_INIT;
    }
}

/*char ReadGpsPacket(void)
{
    gpsBuf_R %= sizeof GpsPacketBuffer;

    if(gpsBuf_R == gpsBuf_W)
      return 0xFF;

    return GpsPacketBuffer[gpsBuf_R++];
}

void WriteGpsPacket(char c)
{
    gpsBuf_W %= sizeof GpsPacketBuffer;
    GpsPacketBuffer[gpsBuf_W++] = c;
}
*/

//GPS macros
//#define GPS_WAIT (1000)
//#define BEGIN (0)
//#define THIRD (3)
//#define LAT_INIT (0x00)
//#define LON_INIT (0x00)
//#define FIRST (1)

void GpsPacketChk(void)
{
    if(readSerial() != '$')
    {
      isGPRMC = BEGIN, isGPGGA = BEGIN;
      return;
    }

    for(int i = BEGIN; i < sizeof GpsPacket; GpsPacket[i++] = CLEAR_VAL);

    for(int i = BEGIN; i < sizeof GpsPacket; i++)
      GpsPacket[i] = readSerial();

    if(GpsPacket[THIRD] == 'M' && GpsPacket[4] == 'C')
      isGPRMC = FIRST;

    if(GpsPacket[THIRD] == 'G' && GpsPacket[4] == 'A')
      isGPGGA = FIRST;

    for(int i = BEGIN; i < sizeof Lat; i++)
    {
      Lat[i] = LAT_INIT;
      Lon[i] = LON_INIT;
    }


    if(isGPRMC)
    {
    int cf = BEGIN;
    while(GpsPacket[cf++] != ','); //Ends after 1st comma
    for(int i = BEGIN; i < sizeof Lat && GpsPacket[cf] != ','; i++, cf++) //Ends at 2nd Comma
      Lat[i] = GpsPacket[cf];

    while(GpsPacket[cf++] != ','); //Ends after 2nd comma
    for(int i = BEGIN; i < sizeof Lon && GpsPacket[cf] != ','; i++, cf++)
      Lon[i] = GpsPacket[cf];
    }
}



