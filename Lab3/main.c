/* 
 * File:   main.c
 * Author: Yuval
 *
 * Created on December 8, 2022, 3:00 PM
 */

#include <stdio.h>
#include <stdlib.h>
#include <xc.h>
#include "./DispA_D_OLED.X/System/system.h"
#include "DispA_D_OLED.X/spiDriver/spi1_driver.h"
#include "DispA_D_OLED.X/oledDriver/oledC.h"
#include "DispA_D_OLED.X/oledDriver/oledC_colors.h"
#include "DispA_D_OLED.X/oledDriver/oledC_shapes.h"


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

void clicked(int id) { // S1 - Left - 1, S2 - Right - 2
    
}

void initialize() {
    TRISAbits.TRISA8 = 0;
    TRISAbits.TRISA11 = 1;
    TRISAbits.TRISA12 = 1;
    TRISAbits.TRISA9 = 0;
}




void inputValues(unsigned int s1 , unsigned int s2) {
    char buffer_s1[33];
    char buffer_s2[33];
    char * s1_display = itoa(s1,buffer_s1);
    char * s2_display = itoa(s2,buffer_s2);
    oledC_clearScreen();
    oledC_DrawString(6, 20, 2, 2, "s1 = ", OLEDC_COLOR_BLUEVIOLET);
    oledC_DrawString(6, 61, 2, 2, "s2 = ", OLEDC_COLOR_BLUEVIOLET);
    oledC_DrawString(60, 20, 2, 2, s1_display, OLEDC_COLOR_BLUEVIOLET);
    oledC_DrawString(60, 61, 2, 2, s2_display, OLEDC_COLOR_BLUEVIOLET);
}





void main() {

    unsigned char s1_count = 0;
    unsigned char s2_count = 0;
    SYSTEM_Initialize(); 
    initialize();
    oledC_setBackground(OLEDC_COLOR_BROWN);
    inputValues(s1_count,s2_count);
    while(1) {
       if(PORTAbits.RA11 == 0) {
           PORTAbits.RA8 = 1;
           inputValues(++s1_count, s2_count);
           delay_ms(60);
           PORTAbits.RA8 = 0;
       }
       if(PORTAbits.RA12 == 0) {
           PORTAbits.RA9 = 1;
           inputValues(s1_count, ++s2_count);
           delay_ms(60);
           PORTAbits.RA9 = 0;
       }   
    }
}

