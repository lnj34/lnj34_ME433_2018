#include<xc.h>           // processor SFR definitions
#include<sys/attribs.h>  // __ISR macro
#include<math.h>

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

//B8 is the chip select
//A1 is SDO1
//

#define CS LATBbits.LATB2  

void init_SPI(){
    // set up the chip select pin as an output
    // the chip select pin is used when a command is beginning (clear CS to low)
    // and when it is ending (set CS high)
    TRISBbits.TRISB2 = 0;
    CS = 1;
    
    RPA1Rbits.RPA1R = 0b0011; //set A1 as SDO1
    
    
    // setup spi0
    SPI1CON = 0;              // turn off the spi module and reset it
    SPI1BUF;                  // clear the rx buffer by reading from it
    SPI1BRG = 0x1;            // baud rate to 12 MHz [SPI4BRG = (80000000/(2*desired))-1]
    SPI1STATbits.SPIROV = 0;  // clear the overflow bit
    SPI1CONbits.CKE = 1;      // data changes when clock goes from hi to lo (since CKP is 0)
    SPI1CONbits.MSTEN = 1;    // master operation
    SPI1CONbits.ON = 1;       // turn on spi 1
}

unsigned char spi_io(unsigned char o) {
  SPI1BUF = o;
  while(!SPI1STATbits.SPIRBF) { // wait to receive the byte
    ;
  }
  return SPI1BUF;
}

void setVoltage(int channel, int voltage){
    unsigned short t =0;
    t = channel << 15;
    t = t | (0b111 << 12);
    t = t | (voltage << 2); 
    
    CS = 0;                   // enable the ADC
    spi_io(t>>8);             // ADC write status
    spi_io(t);             // sequential mode (mode = 0b01), hold disabled (hold = 0)
    CS = 1;                   // finish the command
}
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

    
    //push button is an input pin
    TRISBbits.TRISB4 = 1;
    
    
    //led is an output pin
    TRISAbits.TRISA4 = 0;//0 for output, 1 for input
    LATAbits.LATA4 = 1;
    
    
    
    __builtin_enable_interrupts();
    
    init_SPI();
    
    int i = 0;

    while(1) {
	// use _CP0_SET_COUNT(0) and _CP0_GET_COUNT() to test the PIC timing
	// remember the core timer runs at half the sysclk
        //set core timer to 0
        
        
        _CP0_SET_COUNT(0);
        
        float f1 = 512.0 + 512.0 * sin(i * 2.0 * 3.14 / 1000.0 * 10.0);
        //float f2 = 512.0 * i 
        i++;
        
        
        
        setVoltage(0,(int)f1);
        setVoltage(1,(254);
        
        //while(_CP0_GET_COUNT() < 24000000/1000)
        while(_CP0_GET_COUNT() < 24000000/100000)
        {
            
        }
        
    }
    
    
}