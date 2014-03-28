#include "functions.h"
#include "macros.h"
#include "msp430.h"

void Serial_Init(void)
{
        
        for(int i=0; i<SMALL_RING_SIZE; i++)
          USB_Char_Rx[i] = 0x00;// USB Character

          usb_rx_ring_wr = 0;
          usb_rx_ring_rd = 0;
          for(i=0; i<LARGE_RING_SIZE; i++)
            USB_Char_Tx[i] = 0x00;// USB Character

          usb_tx_ring_wr = 0;
          usb_tx_ring_rd = 0;
          // Configure UART 0
          UCA0CTLW0 = 0; // Use word register
          UCA0CTLW0 |= UCSSEL__SMCLK; // Set SMCLK as fBRCLK
          UCA0CTLW0 |= UCSWRST; // Set Software reset enable
          
          // 9,600 Baud Rate
          // 1. Calculate N = fBRCLK/Baudrate [if N > 16 continue with step 3, otherwise with step 2]
          // N = SMCLK / 9,600 => 8,000,000 / 9,600 = 833.3333333
          // 2. OS16 = 0, UCBRx = INT(N) [continue with step 4]
          // 3. OS16 = 1, UCx = INT(N/16), UCFx = INT([(N/16) – INT(N/16)] × 16)
          // UCx = INT(N/16) = 833.333/16 => 52
          // UCFx = INT([(N/16) – INT(N/16)] × 16) = ([833.333/16-INT(833.333/16)]*16) =>
          // (52.08333333-52)*16=>0.083*16=1
          // 4. UCSx can be found by looking up the fractional part of N ( = N - INT(N) ) in Table 18-4
          // Decicmal of SMCLK / 8,000,000 / 9,600 = 833.3333333 => 0.333 yields 0x25
          // 5. If OS16 = 0 was chosen, a detailed error calculation is recommended to be performed
          // TX error (%) RX error (%)
          // BRCLK Baudrate UCOS16 UCBRx UCFx UCSx neg pos neg pos
          // 8000000 9600 1 52 1 0x49 -0.08 0.04 -0.10 0.14
          UCA0BRW = 52 ; // 9,600 baud
          // UCA0MCTLW = UCSx concatenate UCFx concatenate UCOS16;
          // UCA0MCTLW = 0x49 concatenate 1 concatenate 1;
          UCA0MCTLW = 0x4911 ;
          UCA0CTL1 &= ~UCSWRST; // Release from reset
          UCA0IE |= UCRXIE; // Enable RX interrupt
  
          // Configure UART 1
          UCA1CTLW0 = 0; // Use word register
          UCA1CTLW0 |= UCSSEL__SMCLK; // Set SMCLK as fBRCLK
          UCA1CTLW0 |= UCSWRST; // Set Software reset enable
          UCA1BRW = 104 ; // 4800 baud	
          UCA1MCTLW = 0xD621; // UCA0MCTLW = UCSx | UCFx | UCOS16 = 0xD600 | 0x0020 | 0x0001;
          UCA1CTL1 &= ~UCSWRST; // Release from reset
          UCA1IE |= UCRXIE; // Enable RX interrupt
}

void TxByte(char c)
{
	UCA1TXBUF = c;	
        waitMsec(10);
}

void RxByte(char* c)
{
	*c = UCA1RXBUF;
}

void TxArray(char* array, int length)
{
	for(int i=0; i < length; i++)
		TxByte(array[i]);
}

void RxArray(char* array, int* i, int length)
{
	if(*i < 0)
		*i = -*i;
	*i %= length;
	RxByte(array+*i);
        *i++;
}