#include "magnometer.h"

void init_HMC5883L(void){

    i2c_start(HMC5883L_WRITE);
    i2c_write(0x00); // set pointer to CRA
    i2c_write(0x70); // write 0x70 to CRA
    i2c_stop();

    i2c_start(HMC5883L_WRITE);
    i2c_write(0x01); // set pointer to CRB
    i2c_write(0xA0);
    i2c_stop();

    i2c_start(HMC5883L_WRITE);
    i2c_write(0x02); // set pointer to measurement mode
    i2c_write(0x00); // continous measurement
    i2c_stop();
}
float getHeading(int16_t *x, int16_t *y, int16_t *z){

    float headingDegrees;
    
    i2c_start(HMC5883L_WRITE);
    i2c_write(0x03); // set pointer to X axis MSB
    i2c_stop();

    i2c_start(HMC5883L_READ);

    *x = ((uint8_t)i2c_read_ack())<<8;
    *x |= i2c_read_ack();

    *z = ((uint8_t)i2c_read_ack())<<8;
    *z |= i2c_read_ack();

    *y = ((uint8_t)i2c_read_ack())<<8;
    *y |= i2c_read_nack();

    i2c_stop();

    headingDegrees = atan2((double)*y,(double)*x) * 180 / 3.141592654 + 180;

    return headingDegrees;
}






