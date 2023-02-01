typedef enum {OK, NACK, ACK, BAD_ADDR, BAD_REG} I2Cerror;

void i2c1_open(void);
I2Cerror i2cReadSlaveRegister(unsigned char devAddW, unsigned char regAdd, unsigned char *reg);