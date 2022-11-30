/*
 * File:   Lab1.c
 * Author: Yuval
 * github : https://github.com/yuvalBerghaus/embedded_systems/tree/main/LAB1
 * Created on November 15, 2022, 8:45 PM
 */

 // PIC16F628A Configuration Bit Settings

 // 'C' source line config statements

 // CONFIG
#pragma config FOSC = HS        // Oscillator Selection bits (HS oscillator: High-speed crystal/resonator on RA6/OSC2/CLKOUT and RA7/OSC1/CLKIN)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = ON       // RA5/MCLR/VPP Pin Function Select bit (RA5/MCLR/VPP pin function is MCLR)
#pragma config BOREN = OFF      // Brown-out Detect Enable bit (BOD disabled)
#pragma config LVP = OFF        // Low-Voltage Programming Enable bit (RB4/PGM pin has digital I/O function, HV on MCLR must be used for programming)
#pragma config CPD = ON         // Data EE Memory Code Protection bit (Data memory code-protected)
#pragma config CP = ON          // Flash Program Memory Code Protection bit (0000h to 07FFh code-protected)

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

#include <xc.h>
#include <stdio.h>
#include <stdlib.h>

void delay_ms(unsigned int delayMs)
{
    unsigned int  i;
    unsigned char j;

    for (i = 0; i < delayMs; i++)
    {
        for (j = 0; j < 200; j++)
        {
            asm("NOP");
            asm("NOP");
        }
    }
}

unsigned char display7s(unsigned char v)
{
    switch (v)
    {
    case 0:
        return 0b11111110;
    case 1:
        return 0b00111000;
    case 2:
        return 0b11011101; //CD
    case 3:
        return 0b01111101;
    case 4:
        return 0b00111011;
    case 5:
        return 0b01110111;
    case 6:
        return 0b11110111;
    case 7:
        return 0b00111100;
    case 8:
        return 0b11111111;
    case 9:
        return 0b01111111;
    default:
        return 0;
    }

}

void displayNumber(unsigned char v) {
    unsigned char decimal, unit;
    decimal = v / 10;
    unit = v % 10;
    PORTB = display7s(unit);
    //change_side = 0b00010000; //right
    delay_ms(2);
    PORTB = display7s(decimal);
    PORTBbits.RB4 = 0;
    delay_ms(2);
}
/*
 *
 */
void main()
{
    TRISB = 0x00; //All pins as output
    TRISA = 0xFF; //All pins as input
    PORTB = 0; //All pins off
    unsigned char current_num = 0;
    while (1) //main loop
    {
        if (PORTAbits.RA1 == 0) {
            current_num++;
            delay_ms(60);
        }
        if (PORTAbits.RA2 == 0) {
            current_num--;
            delay_ms(60);
        }
        if (current_num == 0xFF) {
            current_num = 20;
        }
        if (current_num > 20) {
            current_num = 0;
        }
        displayNumber(current_num);
    }
}
