#include <msp430.h>
/************************************************************************************
// Lab 5
// 09/21/2022
// David Nichols and Joshua Ayers
// Versions
// v1 - fixed the displaynumber function. v2 - added displaydigit. v3 - added avg to ADC function.
//
// Lab Objective
// Uses the potentiometer connected to the ADC to display 0-1023 to a 4x7-segment display.
//
// Functions input/output
// displaynumber takes two int, no output, adc takes input and outputs an a 1 or 0, displaydigit takes an int.
//
// Functions explained
// DISPLAYNUMBER is used to display 0-9 to a digit, takes two inputs. First int is the digit place and second is
// the number to be displayed
//
// ADC is used to determine voltage from potentiometer and input the adc value into displaydigit, if a 0 is inputed
// displaydigit is NOT called, if 1 is inputed display digit is called
//
// DISPLAYDIGIT takes an input from the adc and displays 0-9 by calling displaynumber, switching digits is done by
// using the % operator and turning the power for the digit on/off so fast that the human eye cannot see.
************************************************************************************/

void displaynumber(int, int);
void displaydigit(unsigned int);
int adc();


//counter for cycling adc
char Global_Timer = '0';
int Global_cords = 0;
char Global_flag = '0';

int ADCtoG(int Val); //shifted by 100

void main(void) {

    WDTCTL = WDTPW | WDTHOLD;           // Stop watchdog timer

    // Initialize 7-segment pins as Output
    P2DIR |= 0xFF;
    P1DIR |= 0xFF;
    P2SEL &= ~(BIT6 | BIT7);

    //button interrupts enabled
    P1IE |=  BIT3;            // P1.3 interrupt enabled
    P1IES |= BIT3;            // P1.3 Hi/lo edge
    P1REN |= BIT3;            // Enable Pull Up on SW2 (P1.3)
    P1IFG &= ~BIT3;           // P1.3 IFG cleared

//value for historisis
unsigned int value;
unsigned int G;
__enable_interrupt();

while(1){

//only incrementing the display if the adc value has changed by 5
    value = adc();
    if(value+1 > adc() || value-1 < adc() ){
        __delay_cycles(5000);  // Wait 2.5ms to denoise
            displaydigit(value);
            value = adc();
            }
    if(Global_flag == '1'){
            G=ADCtoG(value);
            displaydigit(G);
                            }

       }
}


void displaydigit(unsigned int digit){
//using % and / to determine first, second, third, and fourth digit
    unsigned int FD, SD,TD, FoD;
        FD = digit%10;
        SD = (digit%100)/10;
        TD = (digit%1000)/100;
        FoD = digit/1000;

//if else to determine the 0-1023
    if(digit>1000){
        displaynumber(4,FoD);
        __delay_cycles(2500);  // Wait 2.5ms to denoise

        displaynumber(3,TD);
        __delay_cycles(2500);  // Wait 2.5ms to denoise

        displaynumber(2,SD);
        __delay_cycles(2500);  // Wait 2.5ms to denoise

        displaynumber(1,FD);
        __delay_cycles(2500);  // Wait 2.5ms to denoise

    }

    else if(digit>100){
        displaynumber(3,TD);
        __delay_cycles(2500);  // Wait 2,5ms to denoise

        displaynumber(2,SD);
        __delay_cycles(2500);  // Wait 2.5ms to denoise

        displaynumber(1,FD);
        __delay_cycles(2500);  // Wait 2.5ms to denoise

    }
    else if(digit>10){
        displaynumber(2,SD);
        __delay_cycles(2500);  // Wait 2.5ms to denoise

        displaynumber(1,FD);
        __delay_cycles(2500);  // Wait 2.5ms to denoise

       }
    else{
        displaynumber(1,FD);
    }

}

//takes any int input and displays 0-15, if the input doesnt equal 0-15 then nothing is displayed
void displaynumber (int sigfig, int digit){
    P1DIR |= BIT1; //Set P1.1 as output

    //pin 12, 8, 9, 6
    //pin 12 = 2.1, pin 6 = 2.2, pin 9 = 2.4 and pin 8 = 2.3
    //pin 6 is first digit, pin 8 is second, pin 9 is third and pin 12 is fourth

    //switch statement used to determine what digit to power on/off
    volatile unsigned int mask;
    switch (sigfig){

    case(3)://DIG 2
        mask = 0B10001000;
        break;

    case(4)://DIG 1
        mask = 0B10010000;
        break;

    case(2)://DIG 3
        mask = 0B10100000;
        break;

    case(1)://DIG 4
        mask = 0B11000000;
        break;

    }


    // Toggle seven segment based off number
    //Takes mask from above and sends that to P2 to enable/disable digit
    switch (digit){
      case (0):
        P1OUT =  0B00000000;
        P2OUT = mask | 0B00000100;
            break;

      case (1):
        P1OUT =  0B10010000;
      P2OUT = mask | 0B00000111;
          break;
      case (2):
        P1OUT =  0B01000000;
      P2OUT = mask | 0B00000010;
          break;

      case (3):
        P1OUT =  0B00000000;
      P2OUT = mask| 0B00000011;
          break;

      case (4):
        P1OUT =  0B10010000;
      P2OUT = mask | 0B01111001;;
          break;

      case (5):
        P1OUT =  0B00100000;
      P2OUT = mask | 0B01111001;
          break;

      case (6):
       P1OUT =  0B00110000;
       P2OUT = mask | 0B01111000;
          break;

      case (7):
        P1OUT =  0B10000000;
            P2OUT = mask | 0B00000111;

          break;
      case (8):
        P1OUT =  0B00000000;
      P2OUT = mask | 0B00000000;;
          break;

      case (9):
        P1OUT =  0B00000000;
        P2OUT = mask | 0B01111001;
          break;

      default:
          P1OUT = 0xF;
          P2OUT = 0xF;
          break;
      };

}


int adc(){

    ADC10CTL0 &= ~ENC;                        // ADC10 disabled
    ADC10CTL0 = 0;                            // ADC10, Vref disabled completely


    switch(Global_cords)
           {
           case(1):{P1SEL |= BIT0;}
           case(2):{P1SEL |= BIT1;}
           case(3):{P1SEL |= BIT2;}
           default:{P1SEL |=BIT0;}
           }


    volatile unsigned int ADC_value = 0;
    // select channel A0
    ADC10AE0 = 0x01;
    // Select Channel A0, ADC10CLK/3
    ADC10CTL1 = INCH_0 + ADC10DIV_3;

    // Vcc & Vss as reference,
    // Sample/hold 64 cycle, mult sample, turn on
    ADC10CTL0 = ADC10SHT_3 + MSC + ADC10ON;
    //Use the ADC – Fetch values
    // Sampling and conversion start
    ADC10CTL0 |= ENC + ADC10SC;
    // Assigns the value held in ADC10MEM to the
    // integer called ADC_value
    //read ADC_value and compare
    //1/1000 * i and i & 1000

        ADC_value=ADC10MEM;
    return ADC_value;

}


int ADCtoG(int Val){
    volatile unsigned int output=(Val/1023*400)-200;
    return output;

}

//*************************************************************
// Timer A0 interrupt service routine
//*************************************************************
#pragma vector=TIMER0_A0_VECTOR
__interrupt void Timer_A (void) {
    Global_Timer = '1';
    CCR0 += 30000;                    // Add Offset to CCR0
    Global_cords++;
    if(Global_cords>4){
        Global_cords = 0;}
}


//*************************************************************
// Port 1.3 interrupt service routine
//*************************************************************
#pragma vector=PORT1_VECTOR
__interrupt void Port_1(void) {
Global_flag='1';     // set flag that will cause LED to be toggled in main
P1IFG &= ~BIT3;  // P1.3 IFG cleared
}
