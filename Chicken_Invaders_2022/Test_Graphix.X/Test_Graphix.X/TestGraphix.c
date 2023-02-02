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
#define ADXL345_ADDRESS_WRITE 0x3A
#define ADXL345_ADDRESS_READ 0x3B
#define ADXL345_REG_DEVID 0x00
#include "./i2c1_driver/i2c1_driver.h"
#include "../Accel.h"
#define Fosc    32000000UL   // Oscillator frequency in Hz
#define TICK_PERIOD (Fosc/4/8)  // Timer tick period
#define INTERRUPT_PERIOD (3UL * TICK_PERIOD) // Interrupt period in timer ticks

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

int flag = false; // global variable flag that represents the transition from screen_2 to the game when it becomes true
int timer_counter = 1;
uint8_t jump_down = 10;
bool update_location_flag = false;
bool generated_obstacle = false;
bool generated_laser_beam = false;
bool is_pressed_any_key = false;
int x_adapt = 60;
int i = 0;
int amount_of_obstacles = 0;
int amount_of_lasers = 0;
int shift =5;
int x = 0, y = 0, z = 0;


typedef struct SHAPE {
    uint8_t x_start;
    uint8_t y_start;
    uint8_t x_end;
    uint8_t y_end;
    uint8_t length;
    uint8_t _type; // type 1 - enemy, type 2 - laser -
    bool hit;
    uint16_t color;
} SHAPE;


/*objects in board*/
SHAPE player;
SHAPE obstacles[30];
SHAPE laser_beams[30];


int convert_x_axis(int x_accelarometer) {
    return 96 - (int)((x_accelarometer + 255) * (96.0 / 510.0));
}
int get_x_axis_from_accel() {
    unsigned char id = 0;
    I2Cerror rc;
    char xx[]="     ";
    char yy[]="     ";
    char zz[]="     ";
    unsigned char xyz[6] = {0};
    i2c1_driver_driver_close();
    i2c1_open();
    //    rc = i2cWriteSlaveRegister(0x3A, 0x2D, 8);


    rc = i2cReadSlaveRegister(0x3A, 0, &id);

    if (rc == OK)
        if(id==0xE5)
            oledC_DrawString(10, 10, 2, 2, "ADXL345", OLEDC_COLOR_BLACK);
        else
            errorStop("Acc!Found");
    else
        errorStop("I2C Error");

    rc = i2cWriteSlaveRegister(0x3A, 0x2D, 8);
    for (i=0 ; i<6 ; ++i) {
        rc=i2cReadSlaveRegister(0x3A, 0x32+i, &xyz[i]);
        DELAY_microseconds(5);            
    }
//    oledC_DrawString(26, 30, 2, 2, xx, OLEDC_COLOR_BLACK);
//    oledC_DrawString(26, 50, 2, 2, yy, OLEDC_COLOR_BLACK);
//    oledC_DrawString(26, 70, 2, 2, zz, OLEDC_COLOR_BLACK);
    x = xyz[0]+xyz[1]*256;  //2xbytes ==> word
    y = xyz[2]+xyz[3]*256;
    z = xyz[4]+xyz[5]*256;
    sprintf(xx, "%d", x);   //Make it a string
    sprintf(yy, "%d", y);
    sprintf(zz, "%d", z);
//    oledC_DrawString(26, 30, 2, 2, xx, OLEDC_COLOR_PURPLE);
//    oledC_DrawString(26, 50, 2, 2, yy, OLEDC_COLOR_PURPLE);
//    oledC_DrawString(26, 70, 2, 2, zz, OLEDC_COLOR_PURPLE);
    return convert_x_axis(x);;
}


void update_frame() {

    //updating obstacles new location
    for(i = 0; i < amount_of_obstacles ; i++) {
        if(obstacles[i].y_end > 75) {
            oledC_DrawRectangle(obstacles[i].x_start,obstacles[i].y_start-jump_down,obstacles[i].x_end,obstacles[i].y_end-jump_down,OLEDC_COLOR_BLACK);
            oledC_DrawRectangle(obstacles[i].x_start,obstacles[i].y_start,obstacles[i].x_end,obstacles[i].y_end,OLEDC_COLOR_BLACK);
            obstacles[i].hit = true;
            continue;
        }
        else {
            obstacles[i].y_start = obstacles[i].y_start+jump_down;
            obstacles[i].y_end = obstacles[i].y_end+jump_down;   
        }
    }
    
    for(i = 0 ; i < amount_of_lasers ; i++) {
        if(laser_beams[i].y_start < 6) {
            oledC_DrawRectangle(laser_beams[i].x_start,laser_beams[i].y_start+jump_down,laser_beams[i].x_end,laser_beams[i].y_end+jump_down,OLEDC_COLOR_BLACK);
            oledC_DrawRectangle(laser_beams[i].x_start,laser_beams[i].y_start,laser_beams[i].x_end,laser_beams[i].y_end,OLEDC_COLOR_BLACK);
            laser_beams[i].hit = true;
            continue;
        }
        else {
            laser_beams[i].y_start -= jump_down;
            laser_beams[i].y_end -= jump_down; 
        }
    }
    
    // updating player's location
//    oledC_DrawRectangle(x_adapt ,90,x_adapt + 10,95,OLEDC_COLOR_BLACK);
//    x_adapt = get_x_axis_from_accel();
//    player.x_start = x_adapt;
//    player.x_end = x_adapt + player.length;
    
    
   // laser_beams[amount_of_lasers++]
}

////The following variable will save the location of the player so whenever i clear the screen 
//
void drawShapes() {
    for(i = 0 ; i <= amount_of_lasers ; i++) {
        if(laser_beams[i].hit == true)
            continue;
        oledC_DrawRectangle(laser_beams[i].x_start,laser_beams[i].y_start+jump_down,laser_beams[i].x_end,laser_beams[i].y_end+jump_down,OLEDC_COLOR_BLACK);
        oledC_DrawRectangle(laser_beams[i].x_start,laser_beams[i].y_start,laser_beams[i].x_end,laser_beams[i].y_end,OLEDC_COLOR_RED); // the enemy
    }
    for(i = 0 ; i <= amount_of_obstacles ; i++) {
        if(obstacles[i].hit == true)
            continue;
        oledC_DrawRectangle(obstacles[i].x_start,obstacles[i].y_start-jump_down,obstacles[i].x_end,obstacles[i].y_end-jump_down,OLEDC_COLOR_BLACK);
        oledC_DrawRectangle(obstacles[i].x_start,obstacles[i].y_start,obstacles[i].x_end,obstacles[i].y_end,OLEDC_COLOR_PURPLE); // the enemy
    }
//    oledC_DrawRectangle(player.x_start,player.y_start,player.x_end,player.y_end,OLEDC_COLOR_BLUE);
}

void drawSpaceShip() {
    // updating player's location
    oledC_DrawRectangle(x_adapt ,90,x_adapt + 10,95,OLEDC_COLOR_BLACK);
    x_adapt = get_x_axis_from_accel();
    player.x_start = x_adapt;
    player.x_end = x_adapt + player.length;
    oledC_DrawRectangle(player.x_start,player.y_start,player.x_end,player.y_end,OLEDC_COLOR_BLUE);
}

void display_screen1() {
      oledC_clearScreen();
      oledC_DrawString(10, 10, 2, 2, "Shenkar", OLEDC_COLOR_WHITE); 
      oledC_DrawString(23, 35, 2, 2, "TECH", OLEDC_COLOR_WHITE); 
      oledC_DrawString(5, 55, 2, 2, "Presents", OLEDC_COLOR_WHITE); 
      oledC_DrawString(20, 75, 1, 1, "........", OLEDC_COLOR_WHITE); 
}
//
void display_screen2() {
      oledC_clearScreen();
      oledC_DrawString(20, 10, 2, 2, "Space", OLEDC_COLOR_WHITE); 
      oledC_DrawString(25, 35, 2, 2, "Wars", OLEDC_COLOR_WHITE); 
      oledC_DrawString(5, 65, 1, 1, "Press any key", OLEDC_COLOR_WHITE); 
      oledC_DrawString(22, 80, 1, 1, "to begin", OLEDC_COLOR_WHITE); 
}


void generate_player() {
      player._type = 0;
      player.color = OLEDC_COLOR_BLUE;
      player.hit = false;
      player.length = 10;
      player.x_start = x_adapt;
      player.y_start = 90;
      player.x_end = x_adapt + player.length;
      player.y_end = 95;
}

void start_game() {
    oledC_clearScreen(); 
    generate_player();
//      oledC_DrawRectangle(35,90,55,95,OLEDC_COLOR_BLUE);
}


void setup_timer2(void)
{
    T2CONbits.TSIDL = 1;
    T2CONbits.TGATE = 0;
    T2CONbits.TCKPS = 3;  // 1:8 prescaler
    T2CONbits.TCS = 0;
    TMR2 = 0;
    PR2 = INTERRUPT_PERIOD - 1;
    IFS0bits.T2IF = 0;
    IPC1bits.T2IP = 0x02;
    IEC0bits.T2IE = 1;
    T2CONbits.TON = 1;
}

//
//
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
    
    
    
    setup_timer2();
//    T2CONbits.TON = 1;
//    T2CONbits.TSIDL = 1;
//    T2CONbits.TGATE = 0;
//    T2CONbits.TCKPS = 3; //                                             1:64
//    T2CONbits.TCS = 0;
//    TMR2 = 0;
//    PR2=16400;
//    IFS0bits.T2IF=0;
//    IPC1bits.T2IP=1;
//    IEC0bits.T2IE=1;
    
    oledC_setBackground(OLEDC_COLOR_BLACK);
    display_screen1();
    DELAY_milliseconds(3000);
    display_screen2();

}

void errorStop(char *msg)
{
    oledC_DrawString(0, 20, 2, 2, msg, OLEDC_COLOR_DARKRED);

    for (;;)
        ;
}


int randomNumber() {
    static int seeded = 0;
    if (!seeded) {
        srand(123456789);
        seeded = 1;
    }
    return rand() % 79;
}

void __attribute__((__interrupt__,auto_psv)) _T1Interrupt(void)
{
    if (flag && update_location_flag == false) // the first object starts to fall
    {
        update_location_flag = true;
//        if(timer_counter % 3) generated_obstacle = false;
        timer_counter++;
    }
    IFS0bits.T1IF=0;
}
/*Timer 2 is in charge of turning the flag on every 3 seconds when generating a new obstacle*/
void __attribute__((__interrupt__,auto_psv)) _T2Interrupt(void)
{
    if (flag && generated_obstacle == false) // the first object starts to fall
    {
        generated_obstacle = true;
    }
    IFS0bits.T2IF=0;
}


/*IO interrupt - whenever we click any button at the beggeining it will 
transfer us to the starting game and whenever we click the starting game the button s2 
then it will put flag to generate laser*/
void __attribute__((__interrupt__,auto_psv)) _IOCInterrupt(void) {
//    if (PORTAbits.RA12 == 0) {
//        flag = true;
//    } else if (PORTAbits.RA11 == 0) {
//        flag = true;
//    }
    if(flag && generated_laser_beam == false) {
        generated_laser_beam = true;
    }
    if(is_pressed_any_key == false) {
         is_pressed_any_key = true;   
    }
    IFS1bits.IOCIF = 0;
}

int main(void)
{
    // initialize the system
    SYSTEM_Initialize();
    Init();
//    Main loop
    while(1) {
        // First we need to check if any IO interrupts occured from screen_2 so we can start the game 
        if(is_pressed_any_key == true && flag == false) {
//            is_pressed_any_key = false;
            flag = true;
            start_game();
            generate_player();
        }
        if(flag) {
            DELAY_microseconds(20000); 
            drawSpaceShip();
            if(update_location_flag) {
                update_location_flag = false;
                update_frame();
                drawShapes();
            }
            //every 3 seconds generated_obstacle flag will be true from the start of the game
            if(generated_obstacle == true) {
                generated_obstacle = false;
                if(amount_of_obstacles == 30) {
                    amount_of_obstacles = 0;
                }
                uint8_t random_number = randomNumber();
                SHAPE obstacle;
                obstacle.color = OLEDC_COLOR_PURPLE;
                obstacle.x_start = random_number;
                obstacle.y_start = 0;
                obstacle.x_end = random_number+5;
                obstacle.y_end = 5;
                obstacle.length = 5;
                obstacle._type = 0x1;
                obstacle.hit = false;
                obstacles[amount_of_obstacles++] = obstacle;
                
            }
            if(generated_laser_beam == true) {
                generated_laser_beam = false;
                if(amount_of_lasers == 30) {
                    amount_of_lasers = 0;
                }
                SHAPE laser;
                laser.color = OLEDC_COLOR_RED;
                laser.length = 4;
                laser.x_start = player.x_start+laser.length;
                laser.y_start = player.y_start-laser.length;
                laser.x_end = player.x_end-laser.length;
                laser.y_end = player.y_start;
                laser._type = 0x1;
                laser.hit = false;
                laser_beams[amount_of_lasers++] = laser;
                
            }
 ////////////////////////////////////////////////
//            //  === Display Axes Acceleration   ====================
//            oledC_DrawString(26, 30, 2, 2, xx, OLEDC_COLOR_BLACK);
//            oledC_DrawString(26, 50, 2, 2, yy, OLEDC_COLOR_BLACK);
//            oledC_DrawString(26, 70, 2, 2, zz, OLEDC_COLOR_BLACK);
//            DELAY_milliseconds(1500);
//
//            //  === Erase Axes Acceleration   ====================
//            oledC_DrawString(26, 30, 2, 2, xx, OLEDC_COLOR_SKYBLUE);
//            oledC_DrawString(26, 50, 2, 2, yy, OLEDC_COLOR_SKYBLUE);
//            oledC_DrawString(26, 70, 2, 2, zz, OLEDC_COLOR_SKYBLUE);        
        }
    /////////////////////////////////////////////////
    }
    return 1;
}
