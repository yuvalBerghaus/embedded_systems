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
#include "i2c1_driver/i2c1_driver.h"
int flag = false; // global variable flag that represents the transition from screen_2 to the game when it becomes true
uint8_t jump_down = 10;
uint8_t current_start_y_enemy = 0;
uint8_t current_end_y_enemy = 10;
//The following variable will save the location of the player so whenever i clear the screen 


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

void start_reading() {
    i2c1_driver_driver_open();
    i2c1_driver_start();
    i2c1_driver_TXData(0x3A); // send slave address and write bit
    i2c1_driver_waitRX();
    if (i2c1_driver_isNACK()) {
        oledC_DrawString(5, 65, 1, 1, "Error: NACK received", OLEDC_COLOR_WHITE);
        i2c1_driver_stop();
        return 1;
    }
    
    // send data to slave device
    i2c1_driver_TXData(0x31);
    i2c1_driver_waitRX();
    if (i2c1_driver_isNACK()) {
        oledC_DrawString(5, 65, 1, 1, "Error: NACK received", OLEDC_COLOR_WHITE);
        i2c1_driver_stop();
        return 1;
    }
    
    i2c1_driver_stop();
    i2c1_driver_start();
    i2c1_driver_TXData(0x3B); // send slave address and read bit
    i2c1_driver_waitRX();
    if (i2c1_driver_isNACK()) {
        oledC_DrawString(5, 65, 1, 1, "Error: NACK received", OLEDC_COLOR_WHITE);
        i2c1_driver_stop();
        return 1;
    }
    
    // receive data from slave device
    i2c1_driver_startRX();
    i2c1_driver_waitRX();
    char data = i2c1_driver_getRXData();
//    printf("Data received from slave: %d\n", data);
    
    ///////////////////////////////////////////////////////////////
    char buffer_s1[33];
    char * information = itoa(data,buffer_s1);
    oledC_DrawString(20, 75, 1, 1, information, OLEDC_COLOR_WHITE); 
    //////////////////////////////////////////////////////////////
    i2c1_driver_sendNACK();
    i2c1_driver_stop();
    
    i2c1_driver_driver_close();
}

void start_game() {
      oledC_clearScreen(); 
      oledC_DrawRectangle(35,80,55,95,OLEDC_COLOR_BLUE);
      oledC_DrawRectangle(35,80,55,95,OLEDC_COLOR_BLUE);
}


void Init() {
    // Input/Output Configuration for TRIState
    TRISAbits.TRISA11 = 1; // define input button s1
    TRISAbits.TRISA12 = 1; // define input button s2
    // Interrupt IO
    IFS1bits.IOCIF = 0; // we define interrupt flag 0 at the beggeining since we are starting with no interrupts on change
    IEC1bits.IOCIE = 1;  //  enabling interrupt-on-change interrupts
    IOCNAbits.IOCNA11 = 1; // iocna11 = 1 means that we are enabling the pin of register bit for that component for s1 in portA
    IOCNAbits.IOCNA12 = 1; // iocna11 = 1 means that we are enabling the pin of register bit for that component for s2 in portA
    PADCONbits.IOCON = 1; // turning on interrupt controller
    INTCON2bits.GIE = 1; // enable global interrupts
    
    
    // Interrupt Timer
    T1CONbits.TON = 1;
    T1CONbits.TSIDL = 1;
    T1CONbits.TGATE = 0;
    T1CONbits.TCKPS = 3; //                                             1:64
    T1CONbits.TCS = 0;
    TMR1 = 0;
    PR1=16400;
    IFS0bits.T1IF=0;
    IPC0bits.T1IP=1;
    IEC0bits.T1IE=1;
    INTCON2bits.GIE=1;

}


/*
                         Main application
 */


void __attribute__((__interrupt__)) _T1Interrupt(void)
{
    if (flag == 1) // the first object starts to fall
    {
        oledC_DrawRectangle(0,current_start_y_enemy-jump_down,10,current_end_y_enemy-jump_down,OLEDC_COLOR_BLACK);
        oledC_DrawRectangle(0,current_start_y_enemy,10,current_end_y_enemy,OLEDC_COLOR_PURPLE); // the enemy
        current_start_y_enemy += jump_down;
        current_end_y_enemy += jump_down;
    }
    start_reading();
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

