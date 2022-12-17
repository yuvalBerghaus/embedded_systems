/*
 * File:   LabC1.c
 * Author: Yuval Berghaus
 * Github : https://github.com/yuvalBerghaus/embedded_systems
 * Created on April 11, 2021, 16:17 AM
 */

#include <stdlib.h>
#include <stdio.h>

#include "System/system.h"
#include "System/delay.h"
#include "oledDriver/oledC.h"
#include "oledDriver/oledC_colors.h"
#include "oledDriver/oledC_shapes.h"


/*
                         Main application
 */

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


void init_sys() {
    SYSTEM_Initialize();
    oledC_setBackground(OLEDC_COLOR_WHITE);
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
}

char* itoa(int i, char b[]){
    char const digit[] = "0123456789";
    char* p = b;
    if(i<0){
        *p++ = '-';
        i *= -1;
    }
    int shifter = i;
    do{ //Move to where representation ends
        ++p;
        shifter = shifter/10;
    }while(shifter);
    *p = '\0';
    do{ //Move back, inserting digits as u go
        *--p = digit[i%10];
        i = i/10;
    }while(i);
    return b;
}


void inputValues(unsigned int s1 , const uint16_t color) { // TODO - Change color font to background
    char buffer_s1[33];
    itoa(s1,buffer_s1);
    oledC_DrawString(20, 20, 2, 2, buffer_s1, color);
}



void main(void)
{
    init_sys();
     int current_number = 0;
     int number_before = current_number;
     int check_even = 0;
    while(1) {
        if(PORTAbits.RA12 == 0) {
            delay_ms(100);
            if(check_even++ % 2 == 0) {
                oledC_sendCommand(OLEDC_CMD_SET_DISPLAY_MODE_INVERSE, NULL, 0); 
            }
            else {
                oledC_sendCommand(OLEDC_CMD_SET_DISPLAY_MODE_ON, NULL, 0);
            }
        }
        AD1CON1bits.SAMP = 1;
        delay_ms(10);
        AD1CON1bits.SAMP = 0;
        while(AD1CON1bits.DONE == 0);
        current_number = ADC1BUF0;
        if(current_number != number_before) { // Here i will overrite with the background color the previous number before changing it to the current number
            inputValues(number_before, oledC_getBackground());
            number_before = current_number;
        }
        inputValues(current_number, OLEDC_COLOR_BLUEVIOLET);
        delay_ms(10);
    }
 }
/**
 End of File
*/

