#include "./Test_Graphix.X/i2c1_driver/i2c1_driver.h"
#include "./Accel.h"


//  === Helper Function ===========================================
static I2Cerror _i2cMasterSend(unsigned char b)
{
    i2c1_driver_TXData(b);       //Send Address (to Write)
    return i2c1_driver_isNACK() ? NACK : ACK;
}


//  === I2C API ====================================================
void i2c1_open(void)
{
    i2c1_driver_driver_open();
}

I2Cerror i2cReadSlaveRegister(unsigned char devAddW, unsigned char regAdd, unsigned char * reg )
{
    i2c1_driver_start();
    if(_i2cMasterSend(devAddW) == NACK)
        return BAD_ADDR;
    if(_i2cMasterSend(regAdd) == NACK)
        return BAD_REG;

    i2c1_driver_restart();
    if(_i2cMasterSend(devAddW | 1) == NACK)
        return BAD_ADDR;

    i2c1_driver_startRX();
    i2c1_driver_waitRX();
    *reg = i2c1_driver_getRXData();
    i2c1_driver_sendNACK();
    i2c1_driver_stop();
    return OK;
}


I2Cerror i2cWriteSlaveRegister(unsigned char devAddW, unsigned char regAdd, uint8_t data )
{
    i2c1_driver_start();
    if(_i2cMasterSend(devAddW) == NACK)
        return BAD_ADDR;
    if(_i2cMasterSend(regAdd) == NACK)
        return BAD_REG;
     if(_i2cMasterSend(data) == NACK)
        return BAD_REG;
    i2c1_driver_stop();
  
    return OK;
}
