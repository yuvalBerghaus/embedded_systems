
#include "./Test_Graphix.X/i2c1_driver/i2c1_driver.h"
#include "./Accel.h"


bool i2cReadRegister(unsigned char devAddW, unsigned char regAdd, unsigned char * reg )
{
    i2c1_driver_start();

    // Send Address (to Write)
    i2c1_driver_TXData(devAddW);
    if(i2c1_driver_isNACK())
        return true;

    // Send Register Address
    i2c1_driver_TXData(regAdd);
    if(i2c1_driver_isNACK())
        return true;

    // Send Repeated Start
    i2c1_driver_restart();

    // Send Address (to Read)
    i2c1_driver_TXData(devAddW | 1);
    if(i2c1_driver_isNACK())
        return true;

    // Receive Data
    i2c1_driver_startRX();
    i2c1_driver_waitRX();
    *reg = i2c1_driver_getRXData();
    i2c1_driver_sendNACK();

    i2c1_driver_stop();
    return false;
}


bool i2cWriteRegister(unsigned char devAddW, unsigned char regAdd, uint8_t data )
{
    
    i2c1_driver_start();
    i2c1_driver_TXData(devAddW);      // Send Address (to Write)
    if(i2c1_driver_isNACK())
        return true;

    i2c1_driver_TXData(regAdd);       // Send Register Address
    if(i2c1_driver_isNACK())
        return true;

    i2c1_driver_TXData(data);         // Send Data
    if(i2c1_driver_isNACK())
        return true;

    i2c1_driver_stop();
    return false;
}