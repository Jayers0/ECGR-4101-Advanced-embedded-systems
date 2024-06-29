/************************************************************************************
// Lab 9
// Group 3
// 11/10/2022
// David Nichols and Joshua Ayers
// Versions
//
// Lab Objective
// Use PWM signal to make speaker generate sound bassed on distance
// then pass that distance over uart to be displayed on a 4x7 segment display
//
**********************************************************************************************/


#include "stdint.h"
#include <msp430.h>

void setup_1();
void setup_2();

void sensorsetup();
void buzzersetup();

void uartsetup_1();
void uartsetup_2();

void Write(char);

void displaynumber(int, int);
void displaydigit(unsigned int);

void sensorreading();
void buzz(int);

#define Global_timer 1000
int miliseconds;
int distance;
long sensor;
char display;


int main(void) {
        WDTCTL = WDTPW + WDTHOLD; //Disable the Watchdog timer for our convenience.
        char test = 8;

        P2SEL &= ~(BIT6 | BIT7);
        P2DIR &= ~BIT0;
        P2REN = BIT0;
        P2IN &= BIT0;
        int flag = P2IN & BIT0;
        if(flag > 0){
            while(1)
            {
                setup_1();
           //  sensorreading();
                Write(test);
            }

                    }
            else{
                while(1){
                    setup_2();
                    displaydigit(display);
                        }
            }
}

//calls required setup functions
//delcares pins for seven seg
void setup_1(){
    if (CALBC1_1MHZ==0xFF)              // Check if calibration constant erased
       {
           while(1);                       // do not load program
       }

    sensorsetup();
    buzzersetup();
    uartsetup_1();
    _BIS_SR(GIE);           // global interrupt enable

}

void setup_2(){

    P2DIR |= 0xFF;
    P1DIR |= 0xFF;

    if (CALBC1_1MHZ==0xFF)              // Check if calibration constant erased
       {
           while(1);                       // do not load program
       }

   // sensorsetup();
   // buzzersetup();
    uartsetup_2();
    _BIS_SR(GIE);           // global interrupt enable

}

//no inputs or outputs
//sets up pins for speaker
void buzzersetup(){
    P2DIR |= BIT1; //Set pin 2.1 to the output direction.
    P2SEL |= BIT1; //Select pin 2.1 as our PWM output.
    TA1CCTL1 = OUTMOD_7;
    TA1CTL = TASSEL_2 + MC_1; //TASSEL_2 selects SMCLK as the clock source, and MC_1 //tells it to count up to the value in TA0CCR0.
}

//no inputs or outputs
//sets up sensor
void sensorsetup(){
    BCSCTL1 = CALBC1_1MHZ;
     DCOCTL = CALDCO_1MHZ;          // submainclock 1mhz

     CCTL0 = CCIE;                 // CCR0 interrupt enabled
     CCR0 = 1000;                  // 1ms at 1mhz
     TACTL = TASSEL_2 + MC_1;      // SMCLK, upmode

     P2IFG  = 0x00;                //clear all interrupt flags

}

//no inputs
//uart setup calls requires isr for setup
void uartsetup_1(){
    P1SEL = BIT1 + BIT2 ;                     // P1.1 = RXD, P1.2=TXD
    P1SEL2 = BIT1 + BIT2;

        UCA0CTL1 |= UCSSEL_2;                     // SMCLK
        UCA0BR0 = 104;                            // 1MHz 9600
        UCA0BR1 = 0;                              // 1MHz 9600
        UCA0MCTL = UCBRS1 + UCBRS0;                        // Modulation UCBRSx = 1
        UCA0CTL1 &= ~UCSWRST;                     // Initialize USCI state machine

}

void uartsetup_2(){
    P1SEL = BIT1 + BIT2 ;                     // P1.1 = RXD, P1.2=TXD
    P1SEL2 = BIT1 + BIT2;

        UCA0CTL1 |= UCSSEL_2;                     // SMCLK
        UCA0BR0 = 104;                            // 1MHz 9600
        UCA0BR1 = 0;                              // 1MHz 9600
        UCA0MCTL = UCBRS1 + UCBRS0;                        // Modulation UCBRSx = 1
        UCA0CTL1 &= ~UCSWRST;                     // Initialize USCI state machine
        IE2 |= UCA0RXIE;                          // Enable USCI_A0 RX interrupt

}

//no input or outputs
//reads and pulses pwm pins to measure distance from the sensor
void sensorreading(){
    P2IE &= ~0x01;          // disable interupt
    P2DIR |= BIT4;          // trigger pin as output
    P2OUT |= BIT4;          // generate pulse
    __delay_cycles(10);     // for 100us
    P2OUT &= ~BIT4;         // stop pulse
    P2DIR &= ~BIT5;         // make pin P1.7 input (ECHO)
    P2IFG = 0x00;           // clear flag just in case anything happened before
    P2IE |= BIT4;           // enable interupt on ECHO pin
    P2IES &= ~BIT5;         // rising edge on ECHO pin

              __delay_cycles(30000);          // delay for 30ms (after this time echo //times out if there is no object detected)
              if(distance > 40){ //turning LED on if distance is greater than 40cm
                  buzz(0);
              }
              else if(distance <= 39  && distance > 30){ //turning LED on if //distance is less than 40cm and if distance greater than 30cm
                  buzz(1);
              }

              else if(distance <= 29 && distance > 20){ //turning LED on if //distance is less than 30cm and if distance greater than 20cm
                  buzz(2);
              }

              else if(distance <= 19 && distance > 10){ //turning LED on if //distance is less than 20cm and if distance greater than 10cm
                  buzz(3);
              }

              else if(distance <= 9 && distance >= 0){ //turning LED on if distance //is less than 9cm and if distance greater than 0cm
                  buzz(4);
              }

}


//2500, 2000, 1500, 1000, 500
//1250, 1000, 750,  500, 250
//Takes flag as input to decide if buzzing high or low
//switches timer one vars according to flag
void buzz(int flag){
    switch (flag)
    {
    case(0):
    TA1CCR0 = 2500;
    TA1CCR1 = 1250;
        break;

    case(1):
    TA1CCR0 = 2000;
    TA1CCR1 = 1000;
        break;

    case(2):
    TA1CCR0 = 1500;
    TA1CCR1 = 750;
        break;

    case(3):
    TA1CCR0 = 1000;
    TA1CCR1 = 500;
        break;

    case(4):
    TA1CCR0 = 750;
    TA1CCR1 = 375;
        break;

    default:
    TA1CCR0 = 0;
    TA1CCR1 = 0;
        break;
    }
}



// DISPLAYDIGIT takes an input from the adc and displays 0-9 by calling displaynumber, switching digits is done by
// using the % operator and turning the power for the digit on/off so fast that the human eye cannot see.
void displaydigit(unsigned int digit){
// using % and / to determine first, second, third, and fourth digit
    unsigned int FD, SD,TD, FoD;
        FD = digit%10;
        SD = (digit%100)/10;
        TD = (digit%1000)/100;
        FoD = digit/1000;

// if else to determine the 0-1023
    if(digit>1000){
        displaynumber(4,FoD);
        __delay_cycles(Global_timer);  // Wait 2.5ms to denoise

        displaynumber(3,TD);
        __delay_cycles(Global_timer);  // Wait 2.5ms to denoise

        displaynumber(2,SD);
        __delay_cycles(Global_timer);  // Wait 2.5ms to denoise

        displaynumber(1,FD);
        __delay_cycles(Global_timer);  // Wait 2.5ms to denoise

    }

    else if(digit>100){
        displaynumber(3,TD);
        __delay_cycles(Global_timer);  // Wait 2,5ms to denoise

        displaynumber(2,SD);
        __delay_cycles(Global_timer);  // Wait 2.5ms to denoise

        displaynumber(1,FD);
        __delay_cycles(Global_timer);  // Wait 2.5ms to denoise

    }
    else if(digit>10){
        displaynumber(2,SD);
        __delay_cycles(Global_timer);  // Wait 2.5ms to denoise

        displaynumber(1,FD);
        __delay_cycles(Global_timer);  // Wait 2.5ms to denoise

       }
    else{
        displaynumber(1,FD);
    }

}


// DISPLAYNUMBER is used to display 0-9 to a digit, takes two inputs.
// First int is the digit place and second is the number to be displayed
void displaynumber (int sigfig, int digit){
    P1DIR |= BIT1; //Set P1.1 as output

    //pin 12, 8, 9, 6
    //pin 12 = 2.1, pin 6 = 2.2, pin 9 = 2.4 and pin 8 = 2.3
    //pin 6 is first digit, pin 8 is second
    //pin 9 is third and pin 12 is fourth

    //switch statement used to determine what digit to power on/off
    volatile unsigned int mask;
    switch (sigfig){


    case(3)://Pin 9
        mask = 0B00010000;
        break;

    case(1)://Pin 12
        mask = 0B00000010;
        break;

    case(2)://Pin 8
        mask = 0B00001000;
        break;

    case(4)://Pin 6
        mask = 0B00000100;
        break;

    }

    // Toggle seven segment based off number
    // Takes mask from above and sends that to P2 to enable/disable digit
    switch (digit){
      case (0):
        P1OUT =  0B10000000;
        P2OUT = mask;
            break;

      case (1):
      P1OUT =  0B11011111;
      P2OUT = mask | 0B11000000;
          break;
      case (2):
      P1OUT =  0B01100111; //A B
      P2OUT = mask | 0B01000000;
          break;

      case (3):
      P1OUT =  0B01001111;
      P2OUT = mask | 0B01000000;
      break;

      case (4):
      P1OUT =  0B00011111;
      P2OUT = mask | 0B11000000;
          break;

      case (5):
      P1OUT =  0B00001111;
      P2OUT = mask | 0B01100000;
          break;

      case (6):
       P1OUT =  0B00000111;
       P2OUT = mask | 0B01100000;
          break;

      case (7):
        P1OUT =  0B11011111;
        P2OUT = mask | 0B01000000;

          break;
      case (8):
        P1OUT =  0B00000000;
        P2OUT = mask;
          break;

      case (9):
        P1OUT =  0B00011111;
        P2OUT = mask | 0B01000000;
          break;

      default:
          P1OUT = 0xF;
          P2OUT = 0xF;
          break;
      };

}


void Write(char byte)
{
    //while (!(IFG2&UCA0TXIFG));
    UCA0TXBUF = byte;
}


//interrupt for sensor
#pragma vector=PORT2_VECTOR
__interrupt void Port_2(void)
{
   // int temp[2] = {0,0};
    if(P2IFG&BIT4)  //is there interrupt pending?
        {
          if(!(P2IES&BIT4)) // is this the rising edge?
          {
            TACTL|=TACLR;   // clears timer A
            miliseconds = 0;
            P2IES |= 0x20;  //falling edge
            //temp[0]=TAR;
          }
          else
          {
             // temp[1]=TAR;
              sensor = (long)miliseconds*1000 + (long)TAR;
              distance = sensor/58;
              //TACTL|=TACLR;   // clears timer A
              miliseconds=0;

          }
    P2IFG &= ~0x20; //clear flag
    }
}


// USCI A0/B0 Receive ISR
#pragma vector=USCIAB0RX_VECTOR
__interrupt void USCI0RX_ISR(void)
{
  display = UCA0RXBUF;                        // Display RX'ed charater
}

#pragma vector=TIMER0_A0_VECTOR
__interrupt void Timer_A (void)
{
  miliseconds++;
}

/*
// USCI A0/B0 Transmit ISR
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=USCIAB0TX_VECTOR
__interrupt void USCI0TX_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(USCIAB0TX_VECTOR))) USCI0TX_ISR (void)
#else
#error Compiler not supported!
#endif
{
  unsigned char TxByte=0;
  UCA0TXBUF = TxByte;                       // Read, justify, and transmit
}


#pragma VECTOR = USIAB0RX_VECTOR
__interupt void UART_rec()
{
char rec_val = UCA0RXBUF;
if((int)rec_val == 1 && txFlg == 0) // checks for send flag and that the transmit is not in progress
  {
    txFlg = 1; // sets the transmit flag to high
  }
else if(txFlg ==1 && dataFrames[0] == 0) // checks if it is the first or second data frame
  {
    dataFrames[0] = rec_val;
  }
  else if(txFlg ==1 && dataFrames[0] != 0) checks if the first data frame is empty
    {
      dataFrames[1] = rec_val;
      txFlg =0; // resets the transmission flag
      pData = dataFrames[0]+dataFrames[1]; // Sums the two data frames to the full integer
      dataFrames[0] = 0; // clearing data frame for next transmit
      dataFrames[1] = 0; // clearing data frame for next transmit
    }
}
*/
