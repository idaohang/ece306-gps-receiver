//Include Statements
#include  "msp430.h"
#include  "functions.h"
#include  "macros.h"


//// Port J Macros
//#define GPS_PWR (0x01)
//#define GPS_RESET (0x02)
//#define GPS_PWRCNTL (0x04)
//#define GPS_PWRCHK (0x08)

#include "msp430g2553.h"
                                                       // Hardware-related definitions
#define UART_TXD 0x02                                  // TXD on P1.1 (Timer0_A.OUT0)
#define UART_RXD 0x04                                  // RXD on P1.2 (Timer0_A.CCI1A)

#define UART_TBIT_DIV_2 (1000000 / (9600 * 2))         // Conditions for 9600 Baud SW UART, SMCLK = 1MHz
#define UART_TBIT (1000000 / 9600)
                                                       // Globals for full-duplex UART communication
unsigned int txData;                                   // UART internal variable for TX
unsigned char rxBuffer;                                // Received UART character

void TimerA_UART_tx(unsigned char byte);               // Function prototypes
void TimerA_UART_print(char *string);

void TimerA_UART_tx(unsigned char byte) {              // Outputs one byte using the Timer_A UART

  while (TACCTL0 & CCIE);                              // Ensure last char got TX'd

  TA0CCR0 = TAR;                                       // Current state of TA counter

  TA0CCR0 += UART_TBIT;                                // One bit time till first bit

  TA0CCTL0 = OUTMOD0 + CCIE;                           // Set TXD on EQU0, Int

  txData = byte;                                       // Load global variable

  txData |= 0x100;                                     // Add mark stop bit to TXData

  txData <<= 1;                                        // Add space start bit
}

void TimerA_UART_print(char *string) {                 // Prints a string using the Timer_A UART

  while (*string)
    TimerA_UART_tx(*string++);
}

#pragma vector = TIMER0_A0_VECTOR                      // Timer_A UART - Transmit Interrupt Handler

   __interrupt void Timer_A0_ISR(void) {

  static unsigned char txBitCnt = 10;

  TA0CCR0 += UART_TBIT;                                // Add Offset to CCRx

  if (txBitCnt == 0) {                                 // All bits TXed?

    TA0CCTL0 &= ~CCIE;                                 // All bits TXed, disable interrupt

    txBitCnt = 10;                                     // Re-load bit counter
  }
  else {
    if (txData & 0x01)
      TA0CCTL0 &= ~OUTMOD2;                            // TX Mark '1'
    else
      TA0CCTL0 |= OUTMOD2;                             // TX Space '0'
  }
  txData >>= 1;                                        // Shift right 1 bit
  txBitCnt--;
}

#pragma vector = TIMER0_A1_VECTOR                      // Timer_A UART - Receive Interrupt Handler

  __interrupt void Timer_A1_ISR(void) {

  static unsigned char rxBitCnt = 8;

  static unsigned char rxData = 0;

  switch (__even_in_range(TA0IV, TA0IV_TAIFG)) {       // Use calculated branching

    case TA0IV_TACCR1:                                 // TACCR1 CCIFG - UART RX

         TA0CCR1 += UART_TBIT;                         // Add Offset to CCRx

         if (TA0CCTL1 & CAP) {                         // Capture mode = start bit edge

           TA0CCTL1 &= ~CAP;                           // Switch capture to compare mode

           TA0CCR1 += UART_TBIT_DIV_2;                 // Point CCRx to middle of D0
         }
         else {
           rxData >>= 1;

           if (TA0CCTL1 & SCCI)                        // Get bit waiting in receive latch
             rxData |= 0x80;
           rxBitCnt--;

           if (rxBitCnt == 0) {                        // All bits RXed?

             rxBuffer = rxData;                        // Store in global variable

             rxBitCnt = 8;                             // Re-load bit counter

             TA0CCTL1 |= CAP;                          // Switch compare to capture mode

             _BIC_SR(LPM0_EXIT);                       // wake up from low power mode.
           }
         }
         break;
   }
}