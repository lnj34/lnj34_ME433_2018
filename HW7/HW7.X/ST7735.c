// functions to operate the ST7735 on the PIC32
// adapted from https://github.com/sumotoy/TFT_ST7735
// and https://github.com/adafruit/Adafruit-ST7735-Library

// pin connections:
// VCC - 3.3V
// GND - GND
// CS - B7
// RESET - 3.3V
// A0 - B15
// SDA - A1
// SCK - B14
// LED - 3.3V

// B8 is turned into SDI1 but is not used or connected to anything

#include <xc.h>
#include "ST7735.h"

void SPI1_init() {
  SDI1Rbits.SDI1R = 0b0100; // B8 is SDI1
  RPA1Rbits.RPA1R = 0b0011; // A1 is SDO1
  TRISBbits.TRISB7 = 0; // SS is B7
  LATBbits.LATB7 = 1; // SS starts high

  // A0 / DAT pin
  ANSELBbits.ANSB15 = 0;
  TRISBbits.TRISB15 = 0;
  LATBbits.LATB15 = 0;

	SPI1CON = 0; // turn off the spi module and reset it
  SPI1BUF; // clear the rx buffer by reading from it
  SPI1BRG = 0; // baud rate to 12 MHz [SPI1BRG = (48000000/(2*desired))-1]
  SPI1STATbits.SPIROV = 0; // clear the overflow bit
  SPI1CONbits.CKE = 1; // data changes when clock goes from hi to lo (since CKP is 0)
  SPI1CONbits.MSTEN = 1; // master operation
  SPI1CONbits.ON = 1; // turn on spi1
}

unsigned char spi_io(unsigned char o) {
  SPI1BUF = o;
  while(!SPI1STATbits.SPIRBF) { // wait to receive the byte
    ;
  }
  return SPI1BUF;
}

void LCD_command(unsigned char com) {
    LATBbits.LATB15 = 0; // DAT
    LATBbits.LATB7 = 0; // CS
    spi_io(com);
    LATBbits.LATB7 = 1; // CS
}

void LCD_data(unsigned char dat) {
    LATBbits.LATB15 = 1; // DAT
    LATBbits.LATB7 = 0; // CS
    spi_io(dat);
    LATBbits.LATB7 = 1; // CS
}

void LCD_data16(unsigned short dat) {
    LATBbits.LATB15 = 1; // DAT
    LATBbits.LATB7 = 0; // CS
    spi_io(dat>>8);
    spi_io(dat);
    LATBbits.LATB7 = 1; // CS
}

void LCD_init() {
    SPI1_init();
  int time = 0;
  LCD_command(ST7735_SWRESET);//software reset
  time = _CP0_GET_COUNT();
  while (_CP0_GET_COUNT() < time + 48000000/2/2) {}

	LCD_command(ST7735_SLPOUT);//exit sleep
  time = _CP0_GET_COUNT();
	while (_CP0_GET_COUNT() < time + 48000000/2/200) {}

	LCD_command(ST7735_FRMCTR1);//Frame Rate Control (In normal mode/Full colors)
	LCD_data(0x01);
	LCD_data(0x2C);
	LCD_data(0x2D);
	time = _CP0_GET_COUNT();
	while (_CP0_GET_COUNT() < time + 48000000/2/1000) {}

	LCD_command(ST7735_FRMCTR2);//Frame Rate Control (In normal mode/Full colors)
	LCD_data(0x01);
	LCD_data(0x2C);
	LCD_data(0x2D);
	time = _CP0_GET_COUNT();
	while (_CP0_GET_COUNT() < time + 48000000/2/1000) {}

	LCD_command(ST7735_FRMCTR3);//Frame Rate Control (In normal mode/Full colors)
	LCD_data(0x01);
	LCD_data(0x2C);
	LCD_data(0x2D);
	LCD_data(0x01);
	LCD_data(0x2C);
	LCD_data(0x2D);
	time = _CP0_GET_COUNT();
	while (_CP0_GET_COUNT() < time + 48000000/2/1000) {}

	LCD_command(ST7735_INVCTR);//display inversion
	LCD_data(0x07);
	time = _CP0_GET_COUNT();
	while (_CP0_GET_COUNT() < time + 48000000/2/1000) {}

	LCD_command(ST7735_PWCTR1);
	LCD_data(0x0A);//4.30 - 0x0A
	LCD_data(0x02);//0x05
	LCD_data(0x84);//added auto mode
	time = _CP0_GET_COUNT();
	while (_CP0_GET_COUNT() < time + 48000000/2/1000) {}

	LCD_command(ST7735_PWCTR2);
	LCD_data(0xC5);
	time = _CP0_GET_COUNT();
	while (_CP0_GET_COUNT() < time + 48000000/2/1000) {}

	LCD_command( ST7735_PWCTR3);
	LCD_data(0x0A);
	LCD_data(0x00);
	time = _CP0_GET_COUNT();
	while (_CP0_GET_COUNT() < time + 48000000/2/1000) {}

	LCD_command( ST7735_PWCTR4);
	LCD_data(0x8A);
	LCD_data(0x2A);
	time = _CP0_GET_COUNT();
	while (_CP0_GET_COUNT() < time + 48000000/2/1000) {}

	LCD_command( ST7735_PWCTR5);
	LCD_data(0x8A);
	LCD_data(0xEE);
	time = _CP0_GET_COUNT();
	while (_CP0_GET_COUNT() < time + 48000000/2/1000) {}

	LCD_command(ST7735_VMCTR1);
	LCD_data(0x0E);
	time = _CP0_GET_COUNT();
	while (_CP0_GET_COUNT() < time + 48000000/2/1000) {}

	LCD_command(ST7735_INVOFF);

	LCD_command(ST7735_MADCTL);
	LCD_data(0xC8);
	time = _CP0_GET_COUNT();
	while (_CP0_GET_COUNT() < time + 48000000/2/1000) {}

	LCD_command(ST7735_COLMOD);
	LCD_data(0x05);
	time = _CP0_GET_COUNT();
	while (_CP0_GET_COUNT() < time + 48000000/2/1000) {}

	LCD_command(ST7735_CASET);
	LCD_data(0x00);
	LCD_data(0x00);
	LCD_data(0x00);
	LCD_data(0x7F);
	time = _CP0_GET_COUNT();
	while (_CP0_GET_COUNT() < time + 48000000/2/1000) {}

	LCD_command(ST7735_RASET);
	LCD_data(0x00);
	LCD_data(0x00);
	LCD_data(0x00);
	LCD_data(0x9F);
	time = _CP0_GET_COUNT();
	while (_CP0_GET_COUNT() < time + 48000000/2/1000) {}

	LCD_command(ST7735_GMCTRP1);
	LCD_data(0x02);
	LCD_data(0x1C);
	LCD_data(0x07);
	LCD_data(0x12);
	LCD_data(0x37);
	LCD_data(0x32);
	LCD_data(0x29);
	LCD_data(0x2D);
	LCD_data(0x29);
	LCD_data(0x25);
	LCD_data(0x2B);
	LCD_data(0x39);
	LCD_data(0x00);
	LCD_data(0x01);
	LCD_data(0x03);
	LCD_data(0x10);
	time = _CP0_GET_COUNT();
	while (_CP0_GET_COUNT() < time + 48000000/2/1000) {}

	LCD_command(ST7735_GMCTRN1);
	LCD_data(0x03);
	LCD_data(0x1D);
	LCD_data(0x07);
	LCD_data(0x06);
	LCD_data(0x2E);
	LCD_data(0x2C);
	LCD_data(0x29);
	LCD_data(0x2D);
	LCD_data(0x2E);
	LCD_data(0x2E);
	LCD_data(0x37);
	LCD_data(0x3F);
	LCD_data(0x00);
	LCD_data(0x00);
	LCD_data(0x02);
	LCD_data(0x10);
	time = _CP0_GET_COUNT();
	while (_CP0_GET_COUNT() < time + 48000000/2/1000) {}

	LCD_command(ST7735_NORON);
	time = _CP0_GET_COUNT();
	while (_CP0_GET_COUNT() < time + 48000000/2/100) {}

	LCD_command(ST7735_DISPON);
	time = _CP0_GET_COUNT();
	while (_CP0_GET_COUNT() < time + 48000000/2/10) {}

	LCD_command(ST7735_MADCTL); // rotation
    LCD_data(MADCTL_MX | MADCTL_MY | MADCTL_RGB);
}

void LCD_drawPixel(unsigned short x, unsigned short y, unsigned short color) {
  // check boundary
  LCD_setAddr(x,y,x+1,y+1);
  LCD_data16(color);
}

void LCD_setAddr(unsigned short x0, unsigned short y0, unsigned short x1, unsigned short y1) {
  LCD_command(ST7735_CASET); // Column
  LCD_data16(x0);
	LCD_data16(x1);

	LCD_command(ST7735_RASET); // Page
	LCD_data16(y0);
	LCD_data16(y1);

	LCD_command(ST7735_RAMWR); // Into RAM
}

void LCD_clearScreen(unsigned short color) {
  int i;
  LCD_setAddr(0,0,_GRAMWIDTH,_GRAMHEIGH);
	for (i = 0;i < _GRAMSIZE; i++){
		LCD_data16(color);
	}
}

void LCD_drawChar(unsigned short x, unsigned short y, unsigned char mess, unsigned short color1, unsigned short color2){
    char row = mess - 0x20;
    char col;
    for(col = 0; col < 5; col++){ // 5 pixels per col
        if ((x + col) < 128 && x > 0){ 
            char pixel = ASCII[row][col];
            int i;
            for(i = 0; i < 8; i++){
                if(y + i < 160 && y + i > 0){
                    if (pixel >> i & 1)
                        LCD_drawPixel(x+col, y+i, color1);
                    else
                        LCD_drawPixel(x+col, y+i, color2);
                }
            }
        }   
    }
}

void LCD_drawString(unsigned short x, unsigned short y, char* message, unsigned short color1, unsigned short color2){
    int i =0;
    while(message[i] != 0){
        LCD_drawChar(x, y, message[i], color1, color2);
        i++;
        x+=5;
    }
}


void LCD_drawProgressBar(unsigned short x, unsigned short y, unsigned short h, unsigned short len1, unsigned short color1, unsigned short len2, unsigned short color2){
    int i, j;
    for(i=0; i<len2; i++){
        for(j=0; j<h; j++){
            if(i<len1)
                LCD_drawPixel(x+i, y+j, color1);
            else
                LCD_drawPixel(x+i, y+j, color2);
        }
    }
}

void LCD_drawXBar(unsigned short x0, unsigned short y0, unsigned short h, unsigned short data, unsigned short color1, unsigned short color2){
    char index;
    index = data+64;
    int i, j;
    if(index < 0){
        index = 0;
        for(i=0; i<128; i++){
            for(j=0; j<h; j++){
                if(i<64)
                    LCD_drawPixel(x0+i, y0+j, color1);
                else
                    LCD_drawPixel(x0+i, y0+j, color2);
            }
        }
    }
    else if(index>0 && index<64){
        for(i=0; i<128; i++){
            for(j=0; j<h; j++){
                if(i<index)
                    LCD_drawPixel(x0+i, y0+j, color2);
                else if(i>=index && i<64)
                    LCD_drawPixel(x0+i, y0+j, color1);
                else
                    LCD_drawPixel(x0+i, y0+j, color2);
            }
        }    
    }
    else if(index>=64 && index<128){
        for(i=0; i<128; i++){
            for(j=0; j<h; j++){
                if(i<64)
                    LCD_drawPixel(x0+i, y0+j, color2);
                else if(i>=64 && i<index)
                    LCD_drawPixel(x0+i, y0+j, color1);
                else
                    LCD_drawPixel(x0+i, y0+j, color2);
            }
        }   
    }
    else
        index = 128;
        for(i=0; i<128; i++){
            for(j=0; j<h; j++){
                if(i<64)
                    LCD_drawPixel(x0+i, y0+j, color2);
                else
                    LCD_drawPixel(x0+i, y0+j, color1);
            }
        }
    }
        
    
    
  
    /*for(i=0; i<128; i++){
        for(j=0; j<h; j++){
            if(index<64){
                if(i<index){
                    LCD_drawPixel(x0+i, y0+j, color2);
                }
                else if(i>index && i<64){
                    LCD_drawPixel(x0+i, y0+j, color1);
                }
                else{
                   LCD_drawPixel(x0+i, y0+j, color2);
                }
            }
            else if (index >= 64 && index <=128){
                if(i<64){
                    LCD_drawPixel(x0+i, y0+j, color2);
                }
                else if(i>64 && i<index){
                    LCD_drawPixel(x0+i, y0+j, color1);
                }
                else{
                    LCD_drawPixel(x0+i, y0+j, color2);
                }
            }
            else
                
        }
    }
}*/


void LCD_drawYBar(unsigned short x0, unsigned short y0, unsigned short h, unsigned short data, unsigned short color1, unsigned short color2){
    char index;
    index = data+80;
    int i, j;
    for(i=0; i<160; i++){
        for(j=0; j<h; j++){
            if(index<80){
                if(i<index){
                    LCD_drawPixel(x0+j, y0+i, color2);
                }
                else if(i>index && i<80){
                    LCD_drawPixel(x0+j, y0+i, color1);
                }
                else{
                    LCD_drawPixel(x0+j, y0+i, color2);
                }
            }
            else{
                if(i<80){
                    LCD_drawPixel(x0+j, y0+i, color2);
                }
                else if(i>64 && i<index){
                    LCD_drawPixel(x0+j, y0+i, color1);
                }
                else{
                    LCD_drawPixel(x0+j, y0+i, color2);
                }
            }
        }
    }
}

void draw_xbar(unsigned short x0, unsigned short y0, unsigned short length, unsigned short height, unsigned short color1, unsigned short color2, float xscale){
    int i,j,xfill,xstep;
    unsigned short x_move,y_move,x,y; // pixel coordinates
    unsigned short l_max, h_max; // absolute value of length and height
    
    //Finding absolute dimensions of both bars
    x_move = (length >= 0)? 1:-1; //set x_move = sign(length)*1 
    y_move = (height >= 0)? 1:-1; //set y_move = sign(height)*1 
    l_max = x_move*length; //abs(length)
    h_max = y_move*height; //abs(height)
    
    xstep = (xscale >= 0)? 1:-1;
    xfill = (int)(0.01*xstep*xscale*l_max); //positive % of l_max
    if(xfill>l_max){
        xfill = l_max; //Capping value if xscale over +-100%
    }
    
    //x bar
    for(i=0;i<xfill;i++){
        x = x0 + xstep*i;
        for(j=0;j<h_max;j++){
            y = y0+y_move*j;
            LCD_drawPixel(x,y,color1); // Fill bar with desired color
        }
    }
    
    for(i=xfill;i<l_max;i++){
        x = x0 + xstep*i;
        for(j=0;j<h_max;j++){
            y = y0+y_move*j;
            LCD_drawPixel(x,y,color2); // Fill bar with bg color
        }
    }
    
}

void draw_ybar(unsigned short x0, unsigned short y0, unsigned short length, unsigned short height, unsigned short color1, unsigned short color2, float yscale){
    int i,j,yfill,ystep;
    unsigned short x_move,y_move,x,y; // pixel coordinates
    unsigned short l_max, h_max; // absolute value of length and height
    
    //Finding absolute dimensions of both bars
    x_move = (length >= 0)? 1:-1; //set x_move = sign(length)*1 
    y_move = (height >= 0)? 1:-1; //set y_move = sign(height)*1 
    l_max = x_move*length; //abs(length)
    h_max = y_move*height; //abs(height)
    
    ystep = (yscale >= 0)? 1:-1;
    yfill = (int)(0.01*ystep*yscale*h_max); //positive % of h_max
    if (yfill>h_max){
        yfill = h_max;  //Capping value if yscale over +-100%
    }
    
    //y bar
    for(i=0;i<yfill;i++){
        y = y0 + ystep*i;
        for(j=0;j<l_max;j++){
            x = x0+j*x_move;
            LCD_drawPixel(x,y,color1); // Fill bar with desired color
        }
    }
    
    for(i=yfill;i<h_max;i++){
        y = y0 + ystep*i;
        for(j=0;j<l_max;j++){
            x = x0+x_move*j;
            LCD_drawPixel(x,y,color2); // Fill bar with bg color
        }
    }
    
}