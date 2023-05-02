#include "nu32dip.h" // constants, functions for startup and UART
#include <stdio.h>
#include <math.h>
#include <string.h>

// to communicate with PIC, type the following into command line: 
// screen /dev/tty.usbserial-110 230400
// press ctrl-a then ctrl-z to exit screen

void wait(int time_ms);

// Stack Overflow (tri wave): y = abs((x++ % 6) - 3); This gives a triangular wave of period 6, oscillating between 3 and 0.

int main() {
  // call all init
  
  NU32DIP_Startup();
  while (1) {
      for(int i=0; i<100; i++){
        unsigned int sinbit = ((3.3/2) * sin(2*M_PI*(float)i/100) + (3.3/2)) * (1023/3.3);
        
        unsigned short bittosend = 0; // [1-bit channelbit]111[10-bit sinbit]00
        bittosend = 0b111<<12;
        unsigned char channelbit; // this is 0b0 or 0b1 depending on whether we're using channel A or B
        bittosend = bittosend|(channelbit<<15);
        bittosend = bittosend|(sinbit<<2);
        
        CS = 0;
        spi_io(bittosend>>8) // send first 8 bits
        spi_io(bittosend) // send last 8 bits
        CS = 1;
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
		
