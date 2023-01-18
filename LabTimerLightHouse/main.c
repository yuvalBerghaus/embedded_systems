
/*
 * File:   main.c
 * Author: Yuval Mark Berghaus
 * Github : https://github.com/yuvalBerghaus/embedded_systems
 * Created on December 8, 2022, 4:04 PM
 */
#include <xc.h>
#include <stdio.h>
#include <stdlib.h>
#include "./DispA_D_OLED.X/System/system.h"
#include "DispA_D_OLED.X/spiDriver/spi1_driver.h"
#include "DispA_D_OLED.X/oledDriver/oledC.h"
#include "DispA_D_OLED.X/oledDriver/oledC_colors.h"
#include "DispA_D_OLED.X/oledDriver/oledC_shapes.h"

static int flag = 0;

static void init(void)
{
    oledC_setBackground(0xffff);
    oledC_clearScreen();
    
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


void drawLightHouse() {
    int i = 0;
    for(i = 95 ; i > 48 ; i--)
        oledC_DrawPoint(i,i, OLEDC_COLOR_ORANGE);

    for(i = 0 ; i < 48 ; i++)
        oledC_DrawPoint(i,95-i, OLEDC_COLOR_ORANGE);    
}

void __attribute__((__interrupt__)) _T1Interrupt(void)
{
    drawLightHouse();
    if (flag==0)
    {
        oledC_DrawRing(48, 48, 60, 10, OLEDC_COLOR_WHITE);
        oledC_sendCommand(OLEDC_CMD_SET_DISPLAY_MODE_ON, NULL, 0); // white
        flag=1;
    }
    else if(flag==1)
    {
        oledC_sendCommand(OLEDC_CMD_SET_DISPLAY_MODE_INVERSE, NULL, 0); // black
        oledC_DrawCircle(48, 48, 10, OLEDC_COLOR_ORANGE);
        flag = 2;
    }
    else if(flag==2)
    {
        oledC_sendCommand(OLEDC_CMD_SET_DISPLAY_MODE_ON, NULL, 0); // black
        oledC_DrawCircle(48, 48, 10, OLEDC_COLOR_WHITE);
        oledC_DrawRing(48, 48, 20, 10, OLEDC_COLOR_ORANGE);
        flag = 3;
    }
    else if(flag==3)
    {
        oledC_sendCommand(OLEDC_CMD_SET_DISPLAY_MODE_INVERSE, NULL, 0); // black
        oledC_DrawCircle(48, 48, 30, OLEDC_COLOR_WHITE);
        oledC_DrawRing(48, 48, 40, 10, OLEDC_COLOR_ORANGE);
        flag = 4;
    }
    else if(flag==4)
    {
        oledC_sendCommand(OLEDC_CMD_SET_DISPLAY_MODE_ON, NULL, 0); // black
        oledC_DrawCircle(48, 48, 50, OLEDC_COLOR_WHITE);
        oledC_DrawRing(48, 48, 60, 10, OLEDC_COLOR_ORANGE);
        flag = 0;
    }
    IFS0bits.T1IF=0;
}
void main()
{
    SYSTEM_Initialize();
    init();
    while(true);
}
