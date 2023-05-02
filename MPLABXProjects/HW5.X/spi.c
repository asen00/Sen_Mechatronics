#include <proc/p32mx170f256b.h>
#include "spi.h"
#include "nu32dip.h"
#include <math.h>

void wait(int time_ms);

// initialize SPI1
void initSPI() {
    // Pin B14 has to be SCK1
    // Turn off analog pins
    ANSELB = 0;
    // Make an output pin for CS
    TRISBbits.TRISB15 = 0;
    LATBbits.LATB15 = 1;
    // Set SDO1
    RPB11Rbits.RPB11R = 0b0011;
    // Set SDI1
    SDI1Rbits.SDI1R = 0b0011;

    // setup SPI1
    SPI1CON = 0; // turn off the spi module and reset it
    SPI1BUF; // clear the rx buffer by reading from it
    SPI1BRG = 1; // 1000 for 24kHz, 1 for 12MHz; // baud rate to 10 MHz [SPI1BRG = (48000000/(2*desired))-1]
    SPI1STATbits.SPIROV = 0; // clear the overflow bit
    SPI1CONbits.CKE = 1; // data changes when clock goes from hi to lo (since CKP is 0)
    SPI1CONbits.MSTEN = 1; // master operation
    SPI1CONbits.ON = 1; // turn on spi 
}

// send a byte via spi and return the response
unsigned char spi_io(unsigned char o) {
  SPI1BUF = o;
  while(!SPI1STATbits.SPIRBF) { // wait to receive the byte
    ;
  }
  return SPI1BUF;
}

void main(){
    // call all init
    
    while(1){
        for(int i=0; i<100; i++){
        
        // figure out voltage for triangle wave
        unsigned int sinbit = (sin(2*M_PI*(float)i/100) + 1) * 1024;
        
        // send voltage with spi
        unsigned short s = 0; // [1-bit channelbit]111[10-bit sinbit]00
        s = 0b111<<12;
        unsigned char channelbit = 0; // this is 0b0 or 0b1 depending on whether we're using channel A or B
        s = s|(channelbit<<15);
        s = s|(sinbit<<2);
        
        LATBbits.LATB15 = 0;
        spi_io(s>>8); // send first 8 bits
        spi_io(s); // send last 8 bits
        LATBbits.LATB15 = 1;
        
        // figure out voltage for triangle wave
        //unsigned int tribit = (abs((i % (2*M_PI)) - 3.3)) * (1023/3.3);
        unsigned int tribit = abs((i % 1023) - 512);
        
        // send voltage with spi
        unsigned short t = 0; // [1-bit channelbit]111[10-bit tribit]00
        t = 0b111<<12;
        channelbit = 1; // this is 0b0 or 0b1 depending on whether we're using channel A or B
        t = t|(channelbit<<15);
        t = t|(tribit<<2);
        
        LATBbits.LATB15 = 0;
        spi_io(t>>8); // send first 8 bits
        spi_io(t); // send last 8 bits
        LATBbits.LATB15 = 1;
        
        wait(1000); // delay
        }
    }
}

void wait(int time_ms){
	unsigned int t;
	// the core timer ticks at half the SYSCLK, so 24000000 times per second
	// so each millisecond is 24000 ticks
	// wait 10 milliseconds in each delay
	while(_CP0_GET_COUNT() < t + 2400*time_ms){}
	}