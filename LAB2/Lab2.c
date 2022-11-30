/* 
 * File:   Lab2.c
 * Author: Yuval
 * github : https://github.com/yuvalBerghaus/embedded_systems/blob/main/LAB2/Lab2.X/Lab2.c
 * Created on November 29, 2022, 10:41 PM
 */


#include <xc.h>

#pragma config FOSC = HS       
#pragma config WDTE = OFF     
#pragma config PWRTE = OFF      
#pragma config BOREN = OFF     
#pragma config LVP = OFF        
#pragma config CPD = OFF        
#pragma config WRT = OFF        
#pragma config CP = OFF         

#include <stdio.h>
#include <stdlib.h>
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

/*
 * 
 */
unsigned char display7s(unsigned char digit)
{
  switch(digit)
  {
    case 0:
      return 0x3F;
    case 1:
      return 0x06;
    case 2:
      return 0x5B;
    case 3:
      return 0x4F;
    case 4:
      return 0x66;
    case 5:
      return 0x6D;
    case 6:
      return 0x7D;
    case 7:
      return 0x07;
    case 8:
      return 0x7F;
    case 9:
      return 0x6F;
    default:
      return 0;
  }

}

void displayNumber(int number) {
    unsigned char alafim, meot, asarot, unit;
    unit = number%10;
    PORTB = 0x00;
    PORTD = display7s(unit);
    PORTBbits.RB4 = 1;  
    delay_ms(3);
    asarot = (number/10)%10;
    PORTB = 0x00;
    PORTD = display7s(asarot);
    PORTBbits.RB5 = 1;
    delay_ms(3);
    meot = (number/100)%10;
    PORTB = 0x00;
    PORTD = display7s(meot);
    PORTBbits.RB6 = 1;
    delay_ms(3);
    alafim = (number/1000)%10;
    PORTB = 0x00;
    PORTD = display7s(alafim);
    PORTBbits.RB7 = 1;
    delay_ms(3);
    
}

void Init() {
    ADCON0 = 0x09;  //CHS0=1, ADON=1
    ADCON0bits.ADON = 1;
    TRISA = 0xFF;
    TRISB = 0x00;
    TRISD = 0x00;
}

void main() {
    Init();
    int res = 0;
    while(1) {
        ADCON0bits.GO = 1;
        while(ADCON0bits.GO == 1);
        res = ADRESH * 2^8 + ADRESL;
        displayNumber(res);
    }
}

