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


/*
                         Main application
 */
int main(void)
{
    uint8_t x, y;
    // initialize the system
    SYSTEM_Initialize();
    oledC_setBackground(OLEDC_COLOR_BLACK);
    display_screen1();
    DELAY_milliseconds(3000);
    display_screen2();
    //Main loop
    while(1)
    { 
    }
  return 1;
}
/**
 End of File
*/

