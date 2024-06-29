#include <msp430.h>

// Define Pin Mapping of 7-segment Display
// Segments are connected to P1.0 - P1.7

void displaynumber (volatile int number){
    P1DIR |= BIT1; //Set P1.1 as output

    // Toggle seven segment based off number
      switch (number){
      case (0):
            P2OUT =  0B00100001;
            P1OUT = 0x00;
            break;

      case (1):
        P2OUT =  0B11111101;
        P1OUT = 0x00;
          break;
      case (2):
         P2OUT =  0B11000011;
        P1OUT = 0x00;
          break;

      case (3):
         P2OUT =  0B10010001;
         P1OUT = 0x00;
          break;

      case (4):
        P2OUT =  0B00011101;
        P1OUT = 0x00;
          break;

      case (5):
        P2OUT =  0B01010001;
        P1OUT = 0xFF;
          break;

      case (6):
        P2OUT =  0B01000001;
        P1OUT = 0xFF;
          break;

      case (7):
        P2OUT =  0B11111001;
        P1OUT = 0x00;

          break;
      case (8):
        P2OUT =  0B00000001;
        P1OUT = 0x00;
          break;

      case (9):
        P2OUT =  0B00010001;
        P1OUT = 0x00;
          break;

      case (10): //A
        P2OUT =  0B00001001;
        P1OUT = 0x00;
        break;


      case (11): //B
        P2OUT =  0B01000101;
        P1OUT = 0xFF;
          break;

      case (12)://C
        P2OUT =  0B01100011;
        P1OUT = 0xFF;
          break;

      case (13)://D
        P2OUT =  0B11000101;
        P1OUT = 0x00;

          break;
      case (14)://E
        P2OUT =  0B01000011;
        P1OUT = 0xFF;
          break;

      case (15)://F
        P2OUT =  0B0001011;
        P1OUT = 0xFF;
          break;

      default:
          P2OUT ^= 0xFF;
          P1OUT = 0xFF;

          break;
      };

}


void adc(){
    //Set pin to Analog input
    P1SEL |= BIT0;
    volatile unsigned int ADC_value =0;
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
    
    if(ADC10MEM>(ADC+50)||ADC10MEM<(ADC-50))
    {
    	ADC_value = ADC10MEM;
    }

   if(ADC_value <= 20){displaynumber(0);} // reads adc and and displays 0
      
   else if(ADC_value <= 40){displaynumber(1);} // reads adc and and displays 1

   else if(ADC_value <= 130) {displaynumber(2);} // reads adc and and displays 2

   else if(ADC_value <= 200) {displaynumber(3);} // reads adc and and displays 3

   else if(ADC_value <= 270) {displaynumber(4);} // reads adc and and displays 4
     
   else if(ADC_value <= 340) {displaynumber(5);} // reads adc and and displays 5

   else if(ADC_value <= 410) {displaynumber(6);} // reads adc and and displays 6

   else if(ADC_value <= 480) {displaynumber(7);} // reads adc and and displays 7

   else if(ADC_value <= 550) {displaynumber(8);}  // reads adc and and displays 8

   else if(ADC_value <= 620) {displaynumber(9);} // reads adc and and displays 9

   else if(ADC_value <= 690) {displaynumber(10);} // reads adc and and displays A

   else if(ADC_value <= 760) {displaynumber(11);} // reads adc and and displays B

   else if(ADC_value <= 830) {displaynumber(12);} // reads adc and and displays C

   else if(ADC_value <= 900) {displaynumber(13);} // reads adc and and displays D

   else if(ADC_value <= 970) {displaynumber(14);} // reads adc and and displays E

   else {displaynumber(15);} // reads adc and and displays F


}

//0 is on 1 is off
void main(void) {

    WDTCTL = WDTPW | WDTHOLD;           // Stop watchdog timer
    P2SEL &= ~(BIT6);              // Set P2.6 & P2.7 as GPIO (Default is XIN/XOUT)
    P2SEL &= ~(BIT7);

    // Initialize 7-segment pins as Output
    P2DIR |= 0xFF;
    P1DIR |= BIT1; //Set P1.1 as output
    


    while(1){
    adc();
        }

    }
