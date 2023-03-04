/*
 * File:   LabC2.c
 * Author: Amit
 *
 * Created on April 11, 2021, 16:17 AM
 */



#include <stdlib.h>
#include <stdio.h>
#include <time.h>

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
#define Fosc 32000000UL   // Oscillator frequency in Hz
#define TICK_PERIOD (Fosc / 4 / 8)  // Timer tick period
#define INTERRUPT_PERIOD (3UL * TICK_PERIOD) // Interrupt period in timer ticks
#define SIZE_ARRAY 30
int flag = false; // global variable flag that represents the transition from screen_2 to the game when it becomes true
int timer_counter = 1;
uint8_t jump_down = 10;
uint8_t jump_up = 7;
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
int countObs=0;

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
SHAPE obstacles[SIZE_ARRAY];
SHAPE laser_beams[SIZE_ARRAY];
SHAPE null_shape = {0, 0, 0, 0, 0, 0, false, 0};


void Led_Initialize(void)
{
  ANSB = (1<<12);  //Set RB12 (AN8) to Input

  //Configure A/D Control Registers
  AD1CON1bits.SSRC = 0x0 ;
  AD1CON1bits.MODE12 = 0 ;
  AD1CON1bits.ADON = 1 ;
  AD1CON1bits.FORM = 0b00 ;
  AD1CON2bits.SMPI = 0x0 ;
  AD1CON2bits.PVCFG = 0x00 ;
  AD1CON2bits.NVCFG0 = 0x0 ;
  AD1CON3bits.ADCS = 0xFF ;
  AD1CON3bits.SAMC = 0b10000 ;

  //Configure S1 and LED1 IO directions
  
   RPOR13bits.RP26R = 13;
    RPOR13bits.RP27R = 14;
    RPOR11bits.RP23R = 15;
    
    TRISAbits.TRISA11 = 1;
    TRISAbits.TRISA12 = 1;
    TRISAbits.TRISA8 = 0;
    TRISAbits.TRISA9 = 0;

    OC1RS = 0;
    OC2RS = 0;
    OC3RS = 0;

    OC1CON2bits.SYNCSEL = 0x1F; //self-sync
    OC1CON2bits.OCTRIG = 0; //sync mode
    OC1CON1bits.OCTSEL = 0b111; //FOSC/2
    OC1CON1bits.OCM = 0b110; //edge aligned
    OC1CON2bits.TRIGSTAT = 1;
    OC2CON2bits.SYNCSEL = 0x1F; //self-sync
    OC2CON2bits.OCTRIG = 0; //sync mode
    OC2CON1bits.OCTSEL = 0b111; //FOSC/2
    OC2CON1bits.OCM = 0b110; //edge aligned
    OC2CON2bits.TRIGSTAT = 1;
    OC3CON2bits.SYNCSEL = 0x1F; //self-sync
    OC3CON2bits.OCTRIG = 0; //sync mode
    OC3CON1bits.OCTSEL = 0b111; //FOSC/2
    OC3CON1bits.OCM = 0b110; //edge aligned
    OC3CON2bits.TRIGSTAT = 1;
    
}

bool check_collisions() {
    int i,j;
    for (i = 0; i < amount_of_lasers; i++) {
        SHAPE laser = laser_beams[i];
      //  if (laser._type != 2) continue; // skip non-laser shapes
        for (j = 0; j < amount_of_obstacles; j++) {
            if(obstacles[j].hit == true) {
                continue;
            }
            SHAPE obstacle = obstacles[j];
            if (laser.x_start >= obstacle.x_start && laser.x_end <= obstacle.x_end && laser.y_start <= obstacle.y_end && obstacle.hit==false && laser.hit==false) {
                // collision detected
                laser_beams[i].hit = true;
                obstacles[j].hit = true;
                countObs--;
                // mark obstacle as removed
                //amount_of_obstacles--;
                oledC_DrawRectangle(obstacles[j].x_start,obstacles[j].y_start,obstacles[j].x_end,obstacles[j].y_end,OLEDC_COLOR_BLACK);
                oledC_DrawRectangle(laser_beams[i].x_start,laser_beams[i].y_start,laser_beams[i].x_end,laser_beams[i].y_end,OLEDC_COLOR_BLACK);
                obstacles[j]._type = 0;
                return true;
            }                
        }
    }
    return false;
}


int convert_x_axis(int x_accelarometer) {
    return 96 - (int)((x_accelarometer + 255) * (96.0 / 510.0));
}

int get_x_axis_from_accel() {
    unsigned char id = 0;
    bool rc;
    char xx[]="     ";
    char yy[]="     ";
    char zz[]="     ";
    unsigned char xyz[6] = {0};
    i2c1_driver_driver_close();
    i2c1_driver_driver_open();
    //    rc = i2cWriteSlaveRegister(0x3A, 0x2D, 8);


    rc = i2cReadRegister(0x3A, 0, &id);

    if (rc == false)
        if(id == 0xE5)
            //DELAY_microseconds(1);
            oledC_DrawString(10, 10, 2, 2, "", OLEDC_COLOR_BLACK);
        else
            errorStop("Acc!Found");
    else
        errorStop("I2C Error");

    rc = i2cWriteRegister(0x3A, 0x2D, 8);
    for (i=0 ; i<6 ; ++i) {
        rc=i2cReadRegister(0x3A, 0x32+i, &xyz[i]);
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
//        if(obstacles[i].hit == true) {
//            amount_of_obstacles--;
//            continue;
//        }
        
        // when the obstacle arrives to the floor
        if(obstacles[i].y_end > 75) {
            oledC_DrawRectangle(obstacles[i].x_start,obstacles[i].y_start-jump_down,obstacles[i].x_end,obstacles[i].y_end-jump_down,OLEDC_COLOR_BLACK);
            oledC_DrawRectangle(obstacles[i].x_start,obstacles[i].y_start,obstacles[i].x_end,obstacles[i].y_end,OLEDC_COLOR_BLACK);
            obstacles[i].hit = true;
            screen_game_over();
//            continue;
        }
        else if(obstacles[i].hit == true) {
            continue;
        }
        else {
            obstacles[i].y_start = obstacles[i].y_start+jump_down;
            obstacles[i].y_end = obstacles[i].y_end+jump_down;   
        }
    }
    
    for(i = 0 ; i < amount_of_lasers ; i++) {
        
        //when the laser arrives to the ceil
        if(laser_beams[i].y_start < 6) {
            oledC_DrawRectangle(laser_beams[i].x_start,laser_beams[i].y_start+jump_up,laser_beams[i].x_end,laser_beams[i].y_end+jump_up,OLEDC_COLOR_BLACK);
            oledC_DrawRectangle(laser_beams[i].x_start,laser_beams[i].y_start,laser_beams[i].x_end,laser_beams[i].y_end,OLEDC_COLOR_BLACK);
            laser_beams[i].hit = true;
            continue;
        }
        else if(laser_beams[i].hit == true) {
            continue;
        }
        else {
            laser_beams[i].y_start -= jump_up;
            laser_beams[i].y_end -= jump_up; 
        }
    }
    
    // updating player's location
//    oledC_DrawRectangle(x_adapt ,90,x_adapt + 10,95,OLEDC_COLOR_BLACK);
//    x_adapt = get_x_axis_from_accel();
//    player.x_start = x_adapt;
//    player.x_end = x_adapt + player.length;
    
    
   // laser_beams[amount_of_lasers++]
}

void flags_init() {
    for (i = 0; i < SIZE_ARRAY; i++) {
        obstacles[i] = null_shape;
        laser_beams[i] = null_shape;
    }
    flag = false; // global variable flag that represents the transition from screen_2 to the game when it becomes true
    timer_counter = 1;
    update_location_flag = false;
    generated_obstacle = false;
    generated_laser_beam = false;
    is_pressed_any_key = false;
    x_adapt = 60;
    i = 0;
    amount_of_obstacles = 0;
    amount_of_lasers = 0;
    shift =5;
    x = 0; 
    y = 0; 
    z = 0;
}

void screen_game_over() {
    oledC_clearScreen();
    DELAY_milliseconds(1000);
    oledC_DrawString(0, 20, 2, 2, "Game Over", OLEDC_COLOR_DARKRED);
    oledC_DrawString(10, 65, 1, 1, "Press S1 key", OLEDC_COLOR_WHITE); 
    oledC_DrawString(20, 80, 1, 1, "to restart", OLEDC_COLOR_WHITE); 
    flags_init();
}


// The following variable will save the location of the player so whenever i clear the screen 

void drawShapes() {
    for(i = 0 ; i <= amount_of_lasers ; i++) {
        /*
         I think we need to make sure we delete the laser beam in the correct way here
         */
        if(laser_beams[i].hit == true) {
            continue;
        }
        oledC_DrawRectangle(laser_beams[i].x_start,laser_beams[i].y_start+jump_up,laser_beams[i].x_end,laser_beams[i].y_end+jump_up,OLEDC_COLOR_BLACK);
        oledC_DrawRectangle(laser_beams[i].x_start,laser_beams[i].y_start,laser_beams[i].x_end,laser_beams[i].y_end,OLEDC_COLOR_RED); // the enemy
    }
    for(i = 0 ; i <= amount_of_obstacles ; i++) {
        
        if(obstacles[i].hit == true){
            continue;
        }
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

void setup_timer1(void) {
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

void setup_timer2(void) {
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

void Init() {
    
    OC1RS = 0;
    OC2RS = 0;
    OC3RS = 0;
    
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
    
    setup_timer1();
    
    setup_timer2();
    
    oledC_setBackground(OLEDC_COLOR_BLACK);
    display_screen1();
    DELAY_milliseconds(3000);
    display_screen2();

}

void errorStop(char *msg) {
    oledC_DrawString(0, 20, 2, 2, msg, OLEDC_COLOR_DARKRED);
    for (;;);
}

int random_int345() {
    srand(time(NULL));
    return (rand() % 3) + 3;
}


int randomNumber() {
    static int seeded = 0;
    if (!seeded) {
        srand(time(NULL));
        seeded = 1;
    }
    return rand() % 79;
}

void __attribute__((__interrupt__, auto_psv)) _T1Interrupt(void) {
    if (flag && update_location_flag == false) { // the first object starts to fall
        update_location_flag = true;
//        if(timer_counter % 3) generated_obstacle = false;
        timer_counter++;
    }
    IFS0bits.T1IF = 0;
}


/*Timer 2 is in charge of turning the flag on every 3 seconds when generating a new obstacle*/
void __attribute__((__interrupt__, auto_psv)) _T2Interrupt(void) {
    if (flag && generated_obstacle == false) { // the first object starts to fall
        generated_obstacle = true;
    }
    IFS0bits.T2IF = 0;
}


/*IO interrupt - whenever we click any button at the beggeining it will 
transfer us to the starting game and whenever we click the starting game the button s2 
then it will put flag to generate laser*/
void __attribute__((__interrupt__, auto_psv)) _IOCInterrupt(void) {
//    if (PORTAbits.RA12 == 0) {
//        flag = true;
//    } else if (PORTAbits.RA11 == 0) {
//        flag = true;
//    }
    if (flag && generated_laser_beam == false) {
        generated_laser_beam = true;
    }
    if (is_pressed_any_key == false) {
         is_pressed_any_key = true;   
    }
    IFS1bits.IOCIF = 0;
}
typedef enum  {SHOWING,RED,GREEN,BLUE,YELLOW,SIZE} mode;

void checkNumberOfObstacles(int num){
    mode curr;
    int red=0, green=0,blue =0;

    if (num <= 1) curr = GREEN;
    else if (num == 2) curr = YELLOW;
    else if (num >= 3) curr = RED;
    
    
    int pot = 1023 ;
    switch(curr){
            case SHOWING:
                red = pot;
                green = pot;
                blue= pot;
                break;
            case RED :
                red = pot;
                green = 0;
                blue= 0;
                break;
             case GREEN :
                red = 0;
                green = pot;
                blue= 0;
                break;
             case BLUE :
                red = 0;
                green = 0;
                blue= pot;
                break;
             case YELLOW :
                red = pot;
                green = pot;
                blue= 0;
                break;
            default:
                break; 
    }
    
    OC1R = red;
    OC2R = green;
    OC3R = blue;
    uint16_t color[10];
    int8_t x;
    for (x=10 ; x<20 ; ++x)
      oledC_ReadPoint(x, 10);
    int j= color[0];
    DELAY_milliseconds(100);
    return;
}
int random_size345() {
    return 3;
}

int main(void) {
    // initialize the system
    SYSTEM_Initialize();
    Init();
    Led_Initialize();
    while(1) {
        
        // First we need to check if any IO interrupts occured from screen_2 so we can start the game 
        if (is_pressed_any_key == true && flag == false) {
//            is_pressed_any_key = false;
            flag = true;
            start_game();
            generate_player();
        }
        if (flag) {
            DELAY_microseconds(400000); 
            drawSpaceShip();
            if (update_location_flag) {
                update_location_flag = false;
                checkNumberOfObstacles(countObs);
                check_collisions();
                update_frame();
                drawShapes();
            }
            //every 3 seconds generated_obstacle flag will be true from the start of the game
            if (generated_obstacle == true) {
                generated_obstacle = false;
                if (amount_of_obstacles == 30) {
                    amount_of_obstacles = 0;
                }
                uint8_t random_number = randomNumber();
                SHAPE obstacle;
                obstacle.color = OLEDC_COLOR_PURPLE;
                obstacle.x_start = random_number;
                obstacle.y_start = 0;
                obstacle.x_end = random_number+5*random_int345();
                obstacle.y_end = 5;
                obstacle.length = 5;
                obstacle._type = 0x1;
                obstacle.hit = false;
                obstacles[amount_of_obstacles++] = obstacle;
                countObs++;
            }
            if (generated_laser_beam == true) {
                generated_laser_beam = false;
                if (amount_of_lasers == 30) {
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
        }
    }
    return 1;
}