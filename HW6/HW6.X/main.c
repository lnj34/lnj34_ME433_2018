#include<xc.h>           // processor SFR definitions
#include<sys/attribs.h>  // __ISR macro
#include<math.h>
#include"ST7735.h"


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
    LCD_init(); // send the initializations to the LCD
    LCD_clearScreen(WHITE);
    
    __builtin_enable_interrupts();
    LCD_clearScreen(WHITE);
    unsigned short x = 100; // the initial position starts at (0,0) at the
    //bottom right corner, and positive x is to the left, and positive y is up
    unsigned short y = 100;
    int Letters[] = {40,37,44,44,47,12,44,47,46,39,13,36,41,35,43};
    int i = 0;
    int n = sizeof(Letters)/sizeof(Letters[0]);
        for(i = 0 ; i < n ; i++){
            if( x-5*i < 128){
            draw_Char(x-6*i,y,Letters[i]);
            }
            else { 
                break;
            }
            
            
        }
    }
    
    
//    for (i = 0; i = 5; i++){
//        for(j = 0; j = 8; j++)
//            int xpos = x+i;
//            int ypos = y+j;
//            int ascbit =         
//            LCD_drawPixel(x,y,BLACK);
//        ;
//    }
    
    
    //128 x 160
    //while(1) {
         //end int(main)