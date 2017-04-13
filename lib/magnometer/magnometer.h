#include <avr/io.h>
#include <math.h>
#include "../I2C-master-lib/i2c_master.h"

/************************
*   Magnometer information
*   SDA: PD1
*   SCL: PD0
*************************/
#define HMC5883L_WRITE 0x3C
#define HMC5883L_READ 0x3D 


/**************************
*   Magnometer Functions
***************************/
void init_HMC5883L(void);
float getHeading(int16_t *x, int16_t *y, int16_t *z);