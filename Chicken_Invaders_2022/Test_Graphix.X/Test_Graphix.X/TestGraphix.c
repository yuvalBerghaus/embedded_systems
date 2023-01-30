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
int timer = 0;
uint8_t jump_down = 10;
uint8_t current_start_y_enemy = 0;
uint8_t current_end_y_enemy = 10;

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

typedef struct Node {
    SHAPE *data;
    struct Node* next;
} Node;

Node* head = NULL;


//The following variable will save the location of the player so whenever i clear the screen 

void drawShapes() {
    Node* temp = head;
    Node* prev = NULL;
    while (temp != NULL) {
        oledC_DrawRectangle(temp->data->x_start,temp->data->y_start-jump_down,temp->data->x_end,temp->data->y_end-jump_down,OLEDC_COLOR_BLACK);
        oledC_DrawRectangle(temp->data->x_start,temp->data->y_start,temp->data->x_end,temp->data->y_end,OLEDC_COLOR_PURPLE); // the enemy
        prev = temp;
        temp = temp->next;
    }
}


void insert_shape(SHAPE* shape) {
    uint8_t random_number = 3 % 71;
    shape->color = OLEDC_COLOR_PURPLE;
    shape->x_start = random_number;
    shape->y_start = 0;
    shape->x_end = random_number+20;
    shape->y_end = 10;
    shape->length = 1;
    shape->_type = 0x1;
    shape->hit = false;
    Node* newNode = (Node*) malloc(sizeof(Node));
    newNode->data = shape;
    newNode->next = head;
    head = newNode;
}

void delete(SHAPE *shape) {
    Node* temp = head;
    Node* prev = NULL;
    while (temp != NULL) {
        if (temp->data->x_start == shape->x_start && temp->data->y_start == shape->y_start) {
            if (prev != NULL) {
                prev->next = temp->next;
            } else {
                head = temp->next;
            }
            free(temp);
            return;
        }
        prev = temp;
        temp = temp->next;
    }
}

void update_list() {
    Node* temp = head;
    while (temp != NULL) {
        if (temp->data->_type == 1) {
            temp->data->y_start = temp->data->y_start+jump_down;
            temp->data->y_end = temp->data->y_end+jump_down;
        } else if (temp->data->_type == 2) {
            temp->data->y_start=temp->data->y_start-jump_down;
            temp->data->y_end=temp->data->y_end-jump_down;        
        }
            temp = temp->next;
    }
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

//void start_reading()
//{
//    uint8_t device_id;
//
//    i2c1_driver_driver_open();
//
//    // start communication with the ADXL345
//    i2c1_driver_start();
//    i2c1_driver_TXData(ADXL345_ADDRESS_WRITE);
//    i2c1_driver_waitRX();
//    if(i2c1_driver_isNACK())
//    {
//        printf("Error: NACK received\n");
//        i2c1_driver_stop();
//        return;
//    }
//
//    // send the register address to read from
//    i2c1_driver_TXData(ADXL345_REG_DEVID);
//    i2c1_driver_waitRX();
//    if(i2c1_driver_isNACK())
//    {
//        printf("Error: NACK received\n");
//        i2c1_driver_stop();
//        return;
//    }
//
//    // restart communication
//    i2c1_driver_restart();
//    i2c1_driver_TXData(ADXL345_ADDRESS_READ);
//    i2c1_driver_waitRX();
//    if(i2c1_driver_isNACK())
//    {
//        printf("Error: NACK received\n");
//        i2c1_driver_stop();
//        return;
//    }
//
//    // read the device ID
//    i2c1_driver_startRX();
//    i2c1_driver_waitRX();
//    device_id = i2c1_driver_getRXData();
//    i2c1_driver_sendNACK();
//
//    i2c1_driver_stop();
//
//    // check if the device ID matches 0xE5
//    if(device_id == 0xE5)
//        printf("Device ID match: 0x%02X\n", device_id);
//    else
//        printf("Error: Device ID mismatch, read 0x%02X\n", device_id);
//
//    i2c1_driver_close();
//}

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
    
    
    oledC_setBackground(OLEDC_COLOR_BLACK);
    display_screen1();
    DELAY_milliseconds(3000);
    display_screen2();
    
    
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

}


/*
                         Main application
 */

int shift =10;
void __attribute__((__interrupt__,auto_psv)) _T1Interrupt(void)
{
    if (flag && (timer % 3 == 0)) // the first object starts to fall
    {
        SHAPE* new_shape = (SHAPE*) malloc(sizeof(SHAPE));
        uint8_t random_number = 3 % 71;
        new_shape->color = OLEDC_COLOR_PURPLE;
        new_shape->x_start = random_number;
        new_shape->y_start = 0;
        new_shape->x_end = random_number+20;
        new_shape->y_end = 10;
        new_shape->length = 1;
        new_shape->_type = 0x1;
        new_shape->hit = false;
        Node* newNode = (Node*) malloc(sizeof(Node));
        newNode->data = new_shape;
        newNode->next = head;
        head = newNode;
        if (shift>70) shift=0;

        shift+=10;
    }
    if (flag) // the first object starts to fall
    {
        timer++;
        //////////////////// updateList())
        Node* temp = head;
        while (temp != NULL) {
        if (temp->data->_type == 1) {
            temp->data->y_start = temp->data->y_start+jump_down;
            temp->data->y_end = temp->data->y_end+jump_down;
        } else if (temp->data->_type == 2) {
            temp->data->y_start=temp->data->y_start-jump_down;
            temp->data->y_end=temp->data->y_end-jump_down;        
            }
            temp = temp->next;
        }
        //////////////////////////////////// drawShapes()
            temp = head;
            Node* prev = NULL;
            while (temp != NULL) {
                oledC_DrawRectangle(temp->data->x_start,temp->data->y_start-jump_down,temp->data->x_end,temp->data->y_end-jump_down,OLEDC_COLOR_BLACK);
                oledC_DrawRectangle(temp->data->x_start,temp->data->y_start,temp->data->x_end,temp->data->y_end,OLEDC_COLOR_PURPLE); // the enemy
                prev = temp;
                temp = temp->next;
            }
        ////////////////////////////////////
    }
    IFS0bits.T1IF=0;
}

//void __attribute__((__interrupt__)) _T2Interrupt(void)
//{
//    if (flag == 1) // the first object starts to fall
//    {
//        oledC_DrawRectangle(20,current_start_y_enemy-jump_down,30,current_end_y_enemy-jump_down,OLEDC_COLOR_BLACK);
//        oledC_DrawRectangle(20,current_start_y_enemy,30,current_end_y_enemy,OLEDC_COLOR_PURPLE); // the enemy
//        current_start_y_enemy += jump_down;
//        current_end_y_enemy += jump_down;
//    }
//    IFS0bits.T1IF=0;
//}
//void __attribute__((__interrupt__)) __T2Interrupt(void){
//    if(flag) {
//         oledC_DrawRectangle(20,current_start_y_enemy-jump_down,30,current_end_y_enemy-jump_down,OLEDC_COLOR_BLACK);
//    oledC_DrawRectangle(20,current_start_y_enemy,30,current_end_y_enemy,OLEDC_COLOR_PURPLE); // the enemy
//    current_start_y_enemy += jump_down;
//    current_end_y_enemy += jump_down;   
//    }
//    flag++;
//    IFS0bits.T2IF=0;
//}



void __attribute__((__interrupt__,auto_psv)) _IOCInterrupt(void) {
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
//    Main loop
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

