#include "msp430.h"
#include "macros.h"
#include "functions.h"

char GpsPacket[64], Lat[16], Lon[16];
char isGPRMC = 0, isGPGGA = 0;


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
    
    for(int i = 0; i < sizeof GpsPacket; i++)
      GpsPacket[i] = 0x00;
    for(int i = 0; i < sizeof Lat; i++)
    {
      Lat[i] = 0x00;
      Lon[i] = 0x00;
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
void GpsPacketChk(void)
{
    if(readSerial() != '$')
    {
      isGPRMC = 0, isGPGGA = 0;
      return;
    }
    
    for(int i = 0; i < sizeof GpsPacket; GpsPacket[i++] = 0x00);
    
    for(int i = 0; i < sizeof GpsPacket; i++)
      GpsPacket[i] = readSerial();
    
    if(GpsPacket[3] == 'M' && GpsPacket[4] == 'C')
      isGPRMC = 1;
    
    if(GpsPacket[3] == 'G' && GpsPacket[4] == 'A')
      isGPGGA = 1;
    
    for(int i = 0; i < sizeof Lat; i++)
    {
      Lat[i] = 0x00;
      Lon[i] = 0x00;
    }
    
    
    if(isGPRMC)
    {
    int cf = 0;
    while(GpsPacket[cf++] != ','); //Ends after 1st comma
    for(int i = 0; i < sizeof Lat && GpsPacket[cf] != ','; i++, cf++) //Ends at 2nd Comma
      Lat[i] = GpsPacket[cf]; 
    
    while(GpsPacket[cf++] != ','); //Ends after 2nd comma
    for(int i = 0; i < sizeof Lon && GpsPacket[cf] != ','; i++, cf++) 
      Lon[i] = GpsPacket[cf];
    }
          
}
        

        
