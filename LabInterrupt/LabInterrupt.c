/*
 * File:   main.c
 * Author: Yuval Mark Berghaus
 * Github : https://github.com/yuvalBerghaus/embedded_systems
 * Created on December 8, 2022, 4:04 PM
 */

#include <stdio.h>
#include <stdlib.h>
#include <xc.h>
#include "./DispA_D_OLED.X/System/system.h"
#include "DispA_D_OLED.X/spiDriver/spi1_driver.h"
#include "DispA_D_OLED.X/oledDriver/oledC.h"
#include "DispA_D_OLED.X/oledDriver/oledC_colors.h"
#include "DispA_D_OLED.X/oledDriver/oledC_shapes.h"

unsigned char s1_count = 0;
unsigned char s2_count = 0;
int flag = 0;
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
void initialize() {
    TRISAbits.TRISA11 = 1;
    TRISAbits.TRISA12 = 1;
  
    IFS1bits.IOCIF = 0; // we define interrupt flag 0 at the beggeining since we are starting with no interrupts on change
    IEC1bits.IOCIE = 1;  //  enabling interrupt-on-change interrupts
    IOCNAbits.IOCNA11 = 1; // iocna11 = 1 means that we are enabling the pin of register bit for that component for s1 in portA
    IOCNAbits.IOCNA12 = 1; // iocna11 = 1 means that we are enabling the pin of register bit for that component for s2 in portA
    PADCONbits.IOCON = 1; // turning on interrupt controller
    INTCON2bits.GIE = 1; // enable global interrupts
}

void __attribute__((__interrupt__)) _IOCInterrupt(void) {
    if (PORTAbits.RA12 == 0) {
        s2_count++;
        flag = true;
    } else if (PORTAbits.RA11 == 0) {
        s1_count++;
        flag = true;
    }
    IFS1bits.IOCIF = 0;
}
void inputValues(unsigned int s1 , unsigned int s2) {
    char buffer_s1[33];
    char buffer_s2[33];
    char * s1_display = itoa(s1,buffer_s1);
    char * s2_display = itoa(s2,buffer_s2);
    oledC_DrawString(6, 20, 2, 2, "s1 = ", OLEDC_COLOR_BLUEVIOLET);
    oledC_DrawString(6, 61, 2, 2, "s2 = ", OLEDC_COLOR_BLUEVIOLET);
    oledC_DrawString(60, 20, 2, 2, s1_display, OLEDC_COLOR_BLUEVIOLET);
    oledC_DrawString(60, 61, 2, 2, s2_display, OLEDC_COLOR_BLUEVIOLET);
}



void main() {
    
    SYSTEM_Initialize(); 
    initialize();
    oledC_setBackground(OLEDC_COLOR_BROWN);
    inputValues(s1_count,s2_count);
    while(1) {
        if (PORTAbits.RA11 == 0) {
            if (flag == true)
                oledC_clearScreen();
            flag = false;
        } 
        if (PORTAbits.RA12 == 0) {
            if (flag == true)
                oledC_clearScreen();
            flag = false;
        }
        inputValues(s1_count, s2_count);
    }
}