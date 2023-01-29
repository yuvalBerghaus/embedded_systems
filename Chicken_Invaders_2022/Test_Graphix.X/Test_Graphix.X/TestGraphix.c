/*
 * File:   LabC2.c
 * Author: Amit
 *
 * Created on April 11, 2021, 16:17 AM
 */

#include <stdlib.h>
#include <stdio.h>

#include "System/system.h"
#include "System/delay.h"
#include "oledDriver/oledC.h"
#include "oledDriver/oledC_colors.h"
#include "oledDriver/oledC_shapes.h"

int flag = false; // global variable flag that represents the transition from screen_2 to the game when it becomes true




void display_screen1() {
      oledC_clearScreen();
      oledC_DrawString(10, 10, 2, 2, "Shenkar", OLEDC_COLOR_WHITE); 
      oledC_DrawString(23, 35, 2, 2, "TECH", OLEDC_COLOR_WHITE); 
      oledC_DrawString(5, 55, 2, 2, "Presents", OLEDC_COLOR_WHITE); 
      oledC_DrawString(20, 75, 1, 1, "........", OLEDC_COLOR_WHITE); 
}

void display_screen2() {
      oledC_clearScreen();
      oledC_DrawString(20, 10, 2, 2, "Space", OLEDC_COLOR_WHITE); 
      oledC_DrawString(25, 35, 2, 2, "Wars", OLEDC_COLOR_WHITE); 
      oledC_DrawString(5, 65, 1, 1, "Press any key", OLEDC_COLOR_WHITE); 
      oledC_DrawString(22, 80, 1, 1, "to begin", OLEDC_COLOR_WHITE); 
}

void start_game() {
      oledC_clearScreen(); 
      oledC_DrawRectangle(35,80,55,95,OLEDC_COLOR_BLUE);
      oledC_DrawRectangle(35,80,55,95,OLEDC_COLOR_BLUE);
}


void Init() {
    TRISAbits.TRISA11 = 1; // define input button s1
    TRISAbits.TRISA12 = 1; // define input button s2
  
    IFS1bits.IOCIF = 0; // we define interrupt flag 0 at the beggeining since we are starting with no interrupts on change
    IEC1bits.IOCIE = 1;  //  enabling interrupt-on-change interrupts
    IOCNAbits.IOCNA11 = 1; // iocna11 = 1 means that we are enabling the pin of register bit for that component for s1 in portA
    IOCNAbits.IOCNA12 = 1; // iocna11 = 1 means that we are enabling the pin of register bit for that component for s2 in portA
    PADCONbits.IOCON = 1; // turning on interrupt controller
    INTCON2bits.GIE = 1; // enable global interrupts
}


/*
                         Main application
 */


void __attribute__((__interrupt__)) _T1Interrupt(void)
{
    if (flag == 1) // the first object starts to fall
    {
        flag++;
        
    }
    IFS0bits.T1IF=0;
}

void __attribute__((__interrupt__)) _IOCInterrupt(void) {
    if (PORTAbits.RA12 == 0) {
        flag = true;
    } else if (PORTAbits.RA11 == 0) {
        flag = true;
    }
    IFS1bits.IOCIF = 0;
}

int main(void)
{
    // initialize the system
    SYSTEM_Initialize();
    Init();
    oledC_setBackground(OLEDC_COLOR_BLACK);
    display_screen1();
    DELAY_milliseconds(3000);
    display_screen2();
    //Main loop
    while(1) {
        // First we need to check if any IO interrupts occured from screen_2 so we can start the game 
        if (PORTAbits.RA11 == 0) {
            if (flag == true) {
                start_game();
            }
        } 
        if (PORTAbits.RA12 == 0) {
            if (flag == true) {
                start_game();
            }
        }
    }
  return 1;
}
/**
 End of File
*/

