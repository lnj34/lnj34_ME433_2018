/* 
 * File:   main.c
 * Author: ljibr
 *
 * Created on April 30, 2018, 8:21 AM
 */

#include<xc.h>           // processor SFR definitions
#include<sys/attribs.h>  // __ISR macro
#include<math.h>
#include"I2c_master_noint.h"

// DEVCFG0
#pragma config DEBUG = OFF // no debugging (1 disable?)
#pragma config JTAGEN = OFF // no jtag
#pragma config ICESEL = ICS_PGx1 // use PGED1 and PGEC1
#pragma config PWP = OFF // no write protect
#pragma config BWP = OFF // no boot write protect
#pragma config CP = OFF // no code protect

// DEVCFG1
#pragma config FNOSC = PRIPLL // use primary oscillator with pll 011
#pragma config FSOSCEN = OFF // turn off secondary oscillator
#pragma config IESO = OFF // no switching clocks
#pragma config POSCMOD = HS // high speed crystal mode
#pragma config OSCIOFNC = OFF // disable secondary osc
#pragma config FPBDIV = DIV_1 // divide sysclk freq by 1 for peripheral bus clock
#pragma config FCKSM = CSDCMD // do not enable clock switch
#pragma config WDTPS = PS1048576 // use slowest wdt 10100
#pragma config WINDIS = OFF // wdt no window mode
#pragma config FWDTEN = OFF // wdt disabled
#pragma config FWDTWINSZ = WINSZ_25 // wdt window at 25% 11

// DEVCFG2 - get the sysclk clock to 48MHz from the 8MHz crystal
#pragma config FPLLIDIV = DIV_2 // divide input clock to be in range 4-5MHz 001
#pragma config FPLLMUL = MUL_24 // multiply clock after FPLLIDIV 111
#pragma config FPLLODIV = DIV_2 // divide clock after FPLLMUL to get 48MHz
#pragma config UPLLIDIV = DIV_12 // divider for the 8MHz input clock, then multiplied by 12 to get 48MHz for USB
#pragma config UPLLEN = ON// USB clock on

// DEVCFG3
#pragma config USERID = 0 // some 16bit userid, doesn't matter what
#pragma config PMDL1WAY = OFF // allow multiple reconfigurations
#pragma config IOL1WAY = OFF // allow multiple reconfigurations
#pragma config FUSBIDIO = ON // USB pins controlled by USB module
#pragma config FVBUSONIO = ON // USB BUSON controlled by USB module

// Demonstrates spi by accessing external ram
// PIC is the master, ram is the slave
// Uses microchip MCP4912 DAC chip (see the data sheet for protocol details)
// SDO1(A1) -> SDI (pin 3 -> pin 5) (A1 0b0011)
// SCK1 -> SCK (pin 25 -> pin 4)
// SS1 -> CS (pin 17(B8) -> pin 3)
// Vss (Pin 12) -> GND
// VDD (Pin 1) -> 3.3 V
// SHDN (pin 9) -> 3.3 V (avoid shutting down)
// LDAC (pin 8) -> 3.3V(?) ( LOW both VOUTA and VOUTB are updated at the same time)
// VREFA & VREFB (Pin 11, 13) -> 3.3 V

int main() {

    __builtin_disable_interrupts();

    // set the CP0 CONFIG register to indicate that kseg0 is cacheable (0x3)
    __builtin_mtc0(_CP0_CONFIG, _CP0_CONFIG_SELECT, 0xa4210583);

    // 0 data RAM access wait states
    BMXCONbits.BMXWSDRM = 0x0;

    // enable multi vector interrupts
    INTCONbits.MVEC = 0x1;

    // disable JTAG to get pins back
    DDPCONbits.JTAGEN = 0;
    
    i2c_master_setup();
    init_expander();  
    init_accelerometer();
            
    __builtin_enable_interrupts();
    
 

    while(1) {
        
        
        if(get_expander(0x09) >> 7) { //GPIO = PORT, check if GP7 high
            set_expander(0x0A, 0b1); //Tell OLAT to make GP0 high, LED on
        }
        
        else { //if GP7 low
            set_expander(0x0A, 0b0); //Tell OLAT to make GP0 low, LED off
        }
        
    }
    
    
}

