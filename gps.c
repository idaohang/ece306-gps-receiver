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
    int ci = 0;
    while(GpsPacket[ci++] != ','); //Ends after 1st comma
    while(GpsPacket[ci++] != ','); //Ends after 2nd comma
    
    if(GpsPacket[ci] != 'A')
    {
      Lat[0] = GpsPacket[ci];
      
      return;
    }
    
    while(GpsPacket[ci++] != ','); //Ends after 3rd comma
    
    for(int i = 0; i < sizeof Lat && GpsPacket[ci] != ','; i++, ci++) //Ends at 2nd Comma
      Lat[i] = GpsPacket[ci]; 
    
    while(GpsPacket[ci++] != ','); //Ends after 4th comma
    Lat[15] = GpsPacket[ci];  //N or S
        
    while(GpsPacket[ci++] != ','); //Ends after 5nd comma
    for(int i = 0; i < sizeof Lon && GpsPacket[ci] != ','; i++, ci++) 
      Lon[i] = GpsPacket[ci];
    
    while(GpsPacket[ci++] != ','); //Ends after 6th comma
    Lon[15] = GpsPacket[ci]; //E or W
    }
    
    if(isGPGGA)
    {
    int ci = 0;
    while(GpsPacket[ci++] != ',' && GpsPacket[ci++] != 0xFF); //Ends after 1st comma
    
    if(GpsPacket[ci] == ',' || GpsPacket[ci] == 0xFF)
      return;
    
    while(GpsPacket[ci++] != ','); //Ends after 2nd comma
    
    while(GpsPacket[ci++] != ','); //Ends after 3rd comma
    
    for(int i = 0; i < sizeof Lat && GpsPacket[ci] != ','; i++, ci++) //Ends at 2nd Comma
      Lat[i] = GpsPacket[ci]; 
    
    while(GpsPacket[ci++] != ','); //Ends after 4th comma
    Lat[15] = GpsPacket[ci];  //N or S
        
    while(GpsPacket[ci++] != ','); //Ends after 5nd comma
    for(int i = 0; i < sizeof Lon && GpsPacket[ci] != ','; i++, ci++) 
      Lon[i] = GpsPacket[ci];
    
    while(GpsPacket[ci++] != ','); //Ends after 6th comma
    Lon[15] = GpsPacket[ci]; //E or W
    }  
}
        

        
