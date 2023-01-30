
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>


#include "./i2c1_driver.h"

#ifndef FCY
#define FCY _XTAL_FREQ/2
#endif

#include <xc.h>

void (*i2c1_driver_busCollisionISR)(void);
void (*i2c1_driver_Masteri2cISR)(void);
void (*i2c1_driver_Slavei2cISR)(void);

void i2c1_driver_close(void)
{
    I2C1CONLbits.I2CEN = 0;
}

bool i2c1_driver_driver_open(void)
{
    if(!I2C1CONLbits.I2CEN)
    {
        // initialize the hardware
        // STAT Setting 
        I2C1STAT = 0x0;
        
        // CON Setting
        I2C1CONL = 0x8000;
        
        // Baud Rate Generator Value: I2CBRG 100000;   
        I2C1BRG = 18;
        
        return true;
    }
    else
        return false;
}

bool i2c1_driver_driver_close(void)
{
    I2C1CONLbits.I2CEN=0;
}

void i2c1_driver_start(void)
{
    I2C1CONLbits.SEN = 1;
    while (I2C1CONLbits.SEN)
        ;
}

void i2c1_driver_restart(void)
{
    I2C1CONLbits.RSEN = 1;
    while (I2C1CONLbits.RSEN)
        ;
}

void i2c1_driver_stop(void)
{
    I2C1CONLbits.PEN = 1;
    while (I2C1CONLbits.PEN)
        ;
}

bool i2c1_driver_isNACK(void)
{
    return I2C1STATbits.ACKSTAT;
}

void i2c1_driver_startRX(void)
{
    I2C1CONLbits.RCEN = 1;
}

void i2c1_driver_waitRX(void)
{
    while(I2C1CONLbits.RCEN)
        ;
}

char i2c1_driver_getRXData(void)
{
    return I2C1RCV;
}

void i2c1_driver_TXData(uint8_t d)
{
    I2C1TRN = d;
    while (I2C1STATbits.TRSTAT)
        ;
}

void i2c1_driver_sendACK(void)
{
    I2C1CONLbits.ACKDT = 0;
    I2C1CONLbits.ACKEN = 1; // start the ACK/NACK
    while (I2C1CONLbits.ACKEN)
        ;
}

void i2c1_driver_sendNACK(void)
{
    I2C1CONLbits.ACKDT = 1;
    I2C1CONLbits.ACKEN = 1; // start the ACK/NACK
//    while (I2C1CONLbits.ACKEN)
//        ;
}

void i2c1_driver_clearBusCollision(void)
{
    I2C1STATbits.BCL = 0; // clear the bus collision.
}
