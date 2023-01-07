/*
 * File:   LabC1.c
 * Author: Yuval Berghaus
 *
 * Created on December 30, 2022, 15:17 PM
 */
// github - https://github.com/yuvalBerghaus/embedded_systems.git
#include <stdlib.h>
#include <stdio.h>

#include "System/system.h"
#include "System/delay.h"
#include "oledDriver/oledC.h"
#include "oledDriver/oledC_colors.h"
#include "oledDriver/oledC_shapes.h"


void delay_ms(unsigned int delayMs)
{
unsigned int  i;
unsigned char j;

 for (i =0; i< delayMs; i++)
 {
  for (j =0 ; j < 200; j++)
    {
	asm("NOP");
	asm("NOP");
    }
 }
}


void init() {
    // RED
    TRISAbits.TRISA0 = 0;
    TRISAbits.TRISA1 = 0;
    TRISCbits.TRISC7 = 0;
    TRISAbits.TRISA12 = 1;
    TRISAbits.TRISA11 = 1;
    OC1CON2bits.SYNCSEL = 0x1F;
    OC1CON2bits.OCTRIG = 0;
    OC1CON1bits.OCTSEL = 0b111;
    OC1CON1bits.OCM = 0b110;
    OC1CON2bits.TRIGSTAT = 1;
    
    
    
    
    ANSB = 12;
    AD1CON1bits.SSRC= 0;
    AD1CON1bits.MODE12 = 0;
    AD1CON1bits.ADON = 1;
    AD1CON1bits.FORM = 0;
    TRISAbits.TRISA12 = 1;
    AD1CON2 = 0;
    AD1CHS = 8;
    AD1CON3bits.ADCS = 0xFF;
    AD1CON3bits.SAMC = 0x10;
    
    
    
    // GREEN
    OC2CON2bits.SYNCSEL = 0x1F;
    OC2CON2bits.OCTRIG = 0;
    OC2CON1bits.OCTSEL = 0b111;
    OC2CON1bits.OCM = 0b110;
    OC2CON2bits.TRIGSTAT = 1;
    
    //BLUE
    OC3CON2bits.SYNCSEL = 0x1F;
    OC3CON2bits.OCTRIG = 0;
    OC3CON1bits.OCTSEL = 0b111;
    OC3CON1bits.OCM = 0b110;
    OC3CON2bits.TRIGSTAT = 1;    
    OC1RS = 1023;
    OC2RS = 1023;
    OC3RS = 1023;
    _RP26R = 13;  
    _RP27R = 14;
    _RP23R = 15;
    
}

void turnGreen(int value) {
    OC2R = value;
    OC3R = 0;
    OC1R = 0;
}

void turnBlue(int value) {
    OC3R = value;
    OC1R = 0;
    OC2R = 0;
}

void turnRed(int value) {
    OC1R = value;
    OC2R = 0;
    OC3R = 0;
}

void mixColors(int red, int green, int blue) {
    OC1R = red;
    OC2R = green;
    OC3R = blue;
}

void mixRed(int red) {
    OC1R = red;
}
void mixGreen(int green) {
    OC2R = green;
}
void mixBlue(int blue) {
    OC3R = blue;
}


/*
                         Main application
 */
void main()
{
    init();
    int red_number = 1023;
    int green_number = 1023;
    int blue_number = 1023;
    int red_mixed = 0;
    int green_mixed = 0;
    int blue_mixed = 0;
    int flag_mixed = 0;
    long double shared_value = 0;
    int current_number = ADC1BUF0;
    int mode = 0;
    while(1) {
        current_number = ADC1BUF0;
        AD1CON1bits.SAMP = 1;
        delay_ms(10);
        AD1CON1bits.SAMP = 0;
        while(AD1CON1bits.DONE == 0);
        if(mode % 3 == 0 && !flag_mixed) { // RED
            turnRed(current_number);
        }
        else if(mode % 3 == 1 && !flag_mixed) { // GREEN
            turnGreen(current_number);
        }
        else if(mode % 3 == 2 && !flag_mixed) {
            turnBlue(current_number); // BLUE
        }
        else if(flag_mixed) {
            shared_value = ADC1BUF0 / 1024.0;
//            mixRed(red_number * shared_value);
//            mixGreen(green_number * shared_value);
//            mixBlue(blue_number * shared_value);
            mixColors(red_number * shared_value , green_number * shared_value, blue_number * shared_value);
        }
        delay_ms(10);
        
        
       if(PORTAbits.RA12 == 0) { // s2 is pushed
           if(mode % 3 == 0) { // FROM RED TO GREEN
               red_number = current_number; // STORE RED VALUE
               mode++;
           }
           else if(mode % 3 == 1) { // BLUE
               green_number = current_number;
               mode++;
           }
           else if(mode%3 == 2) {
               blue_number = current_number;
               mode++;
           }
           delay_ms(200);
       }
       if(PORTAbits.RA11 == 0) { // s1 is pushed
           flag_mixed = !flag_mixed;
           if(flag_mixed == 1) {
               shared_value = ADC1BUF0 / 1024.0;
               mixRed((red_number * shared_value));
               mixGreen((green_number * shared_value));
               mixBlue((blue_number * shared_value));
           }
           else {
               red_number = 1023;
               green_number = 1023;
               blue_number = 1023;
           }
           delay_ms(200);
       }  
    }
}
/**
 End of File
*/

