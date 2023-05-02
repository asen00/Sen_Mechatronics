#include "nu32dip.h" // constants, functions for startup and UART
#include <stdio.h>
#include <math.h>
#include <string.h>

// to communicate with PIC, type the following into command line: 
// screen /dev/tty.usbserial-110 230400
// press ctrl-a then ctrl-z to exit screen

void wait(int time_ms);

int main(void) {
  char message[100];
  float sine[100];
  
  // needed help from Chris Cravey for sine wave part
  for(int i=0; i<100; i++){
      sine[i] = 3.3/2*sin(2*M_PI*(double)i/100)+3.3/2;
  }
  NU32DIP_Startup();
  while (1) {
      int i = 0;
      if (!NU32DIP_USER){  //True if USER button is pressed
      for(i=0; i<100; i++){
          sprintf(message,"%f\n",sine[i]);
          NU32DIP_WriteUART1(message); // send message back
          wait(1000.0f/100);{}
      }
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
		
