#include<xc.h>           // processor SFR definitions
#include<sys/attribs.h>  // __ISR macro
#include<math.h>
#include"I2C_master_imu.h"
#include"ST7735.h"
#include<stdio.h>

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
    
    SPI1_init();
    i2c_master_setup();
    init_expander();  
    LCD_init();
    
    LCD_clearScreen(YELLOW);
    
    __builtin_enable_interrupts();
    
    unsigned char data[13];
    char address[100];

    sprintf(address, "WHOAMI: %d", get_expander(WHO_AM_I));
    LCD_drawString(28, 32, address, RED, YELLOW);
    while(1) {
        I2C_read_multiple(SLAVE_ADDR, OUT_TEMP_L, data, 14);
        signed short temp = (data[1] << 8)| data[0];
        signed short gyroX = (data[3] << 8) | data[2];
        signed short gyroY = (data[5] << 8) | data[4];
        signed short gyroZ = (data[7] << 8) | data[6];
        signed short acceX = (data[9] << 8) | data[8];
        signed short acceY = (data[11] << 8) | data[10];
        signed short acceZ = (data[13] << 8) | data[12];
        
        float xscale = acceX*0.000061*100; 
        float yscale = acceY*0.000061*100;
        
        sprintf(address , "X: %f", xscale);
        LCD_drawString(25, 10, address, BLUE, YELLOW);
        sprintf(address , "Y: %f", yscale);
        LCD_drawString(25, 150, address, BLUE, YELLOW);
        
        //LCD_drawXBar(0, 80, 10, acceX/100, BLUE, YELLOW);
        draw_xbar(64, 64, 50, 5, BLUE, YELLOW, xscale);
        draw_ybar(64, 64, 5, 50, BLUE, YELLOW, -yscale);
        
        _CP0_SET_COUNT(0);
        while (_CP0_GET_COUNT() < 24000000/10){//10HZ loop
            ;
        }
    }
}