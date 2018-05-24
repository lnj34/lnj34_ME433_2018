/* 
 * File:   I2C_master_imu.h
 * Author: jidingzi
 *
 * Created on April 30, 2018, 4:15 PM
 */

#ifndef I2C_MASTER_IMU_H
#define	I2C_MASTER_IMU_H

#define SLAVE_ADDR 0x6B //derived from opcode
#define CTRL1_XL   0x10
#define CTRL2_G    0x11 
#define CTRL3_C    0x12
#define WHO_AM_I   0x0F
#define OUT_TEMP_L 0x20
#define OUTX_L_XL  0x28

void i2c_master_setup(void);              // set up I2C 2 as a master, at 100 kHz

void i2c_master_start(void);              // send a START signal
void i2c_master_restart(void);            // send a RESTART signal
void i2c_master_send(unsigned char byte); // send a byte (either an address or data)
unsigned char i2c_master_recv(void);      // receive a byte of data
void i2c_master_ack(int val);             // send an ACK (0) or NACK (1)
void i2c_master_stop(void);               // send a stop

void init_expander(void);                           //initialize  accelerometer
void set_expander(unsigned char, unsigned char);            //set pin config
unsigned char get_expander(unsigned char);        

void I2C_read_multiple(unsigned char address, unsigned char register, unsigned char * data, int length);
//read multiple register in series

#endif	/* I2C_MASTER_IMU_H */

