// I2C Master utilities, using polling rather than interrupts
// The functions must be called in the correct order as per the I2C protocol
// I2C pins need pull-up resistors, 2k-10k
#include "i2c_master_noint.h"
#include "nu32dip.h"

void wait(int time);
void turnOn_GP7();
void turnOff_GP7();

void i2c_master_setup(void) {
    // using a large BRG to see it on the nScope, make it smaller after verifying that code works
    // look up TPGD in the datasheet
    I2C1BRG = 1000; // I2CBRG = [1/(2*Fsck) - TPGD]*Pblck - 2 (TPGD is the Pulse Gobbler Delay)
    I2C1CONbits.ON = 1; // turn on the I2C1 module
}

void i2c_master_start(void) {
    I2C1CONbits.SEN = 1; // send the start bit
    while (I2C1CONbits.SEN) {
        ;
    } // wait for the start bit to be sent
}

void i2c_master_restart(void) {
    I2C1CONbits.RSEN = 1; // send a restart 
    while (I2C1CONbits.RSEN) {
        ;
    } // wait for the restart to clear
}

void i2c_master_send(unsigned char byte) { // send a byte to slave
    I2C1TRN = byte; // if an address, bit 0 = 0 for write, 1 for read
    while (I2C1STATbits.TRSTAT) {
        ;
    } // wait for the transmission to finish
    if (I2C1STATbits.ACKSTAT) { // if this is high, slave has not acknowledged
        // ("I2C1 Master: failed to receive ACK\r\n");
        while(1){} // get stuck here if the chip does not ACK back
        // blink LED normally and if stops blinking, we're stuck
        // could get stuck if send/receive are out of phase
        // solution: make this chip reset at the same time as the PIC
    }
}

unsigned char i2c_master_recv(void) { // receive a byte from the slave
    I2C1CONbits.RCEN = 1; // start receiving data
    while (!I2C1STATbits.RBF) {
        ;
    } // wait to receive the data
    return I2C1RCV; // read and return the data
}

void i2c_master_ack(int val) { // sends ACK = 0 (slave should send another byte)
    // or NACK = 1 (no more bytes requested from slave)
    I2C1CONbits.ACKDT = val; // store ACK/NACK in ACKDT
    I2C1CONbits.ACKEN = 1; // send ACKDT
    while (I2C1CONbits.ACKEN) {
        ;
    } // wait for ACK/NACK to be sent
}

void i2c_master_stop(void) { // send a STOP:
    I2C1CONbits.PEN = 1; // comm is complete and master relinquishes bus
    while (I2C1CONbits.PEN) {
        ;
    } // wait for STOP to complete
}

int main() {
    NU32DIP_Startup();
    i2c_master_setup();
    
    // set GP0 as input, GP7 as output (read from GP0; if high, turn on GP7)
    i2c_master_start();
    i2c_master_send(0b01000000); // OP + ADDR + W
    i2c_master_send(0b10000000); // GP7 (bit 7)
    i2c_master_send(0b00000000); // output
    i2c_master_stop();
    
    i2c_master_start();
    i2c_master_send(0b01000000); // OP + ADDR + W
    i2c_master_send(0b00000001); // GP0 (bit 0)
    i2c_master_send(0b00000001); // input
    i2c_master_stop();
    
    while(1) {
        // blink yellow LED on PIC (turn on, delay, turn off, delay)
        NU32DIP_YELLOW = 0;
        wait(1000);
        NU32DIP_YELLOW = 1;
        wait(1000);
        
        // first try to blink GP7
        turnOn_GP7();
//        wait(100);
//        turnOff_GP7();
//        wait(100);
        
        // if GP7 blinks, we know we can write to it
        
//        r = read_from_GP0;
//        if(r) {
//            turnOn_GP7();
//        }
//        else{
//            turnOff_GP7();
//        }
    }
}

void turnOn_GP7() {
    // send start bit
    i2c_master_start();
    // send address of chip
    i2c_master_send(0b01000000); // 0b01000000 to write; 0b01000001 to read
    // send register name
    i2c_master_send(0b00001010); // OLAT register
    // send value to turn on GP7
    i2c_master_send(0b00000001); // 0b10000000 turns on GP7; 0b00000000 turns off GP7
    // send stop bit
    i2c_master_stop();
}

void turnOff_GP7() {
    // send start bit
    i2c_master_start();
    // send address of chip
    i2c_master_send(0b01000000); // 0b01000000 to write; 0b01000001 to read
    // send register name
    i2c_master_send(0b00001010); // OLAT register
    // send value to turn on GP7
    i2c_master_send(0b00000000); // 0b10000000 turns on GP7; 0b00000000 turns off GP7
    // send stop bit
    i2c_master_stop();
}

int read_from_GP0(){
    // send start bit
    // send address with write bit
    // send register you want to write from
    // restart
    // send address with read bit
    unsigned char r = i2c_master_recv(); // value of input bits
    // send ACK bit
    // send stop bit
    return (r&0b00000001);
}

void wait(int time_ms){
	unsigned int t = _CP0_GET_COUNT();
	// the core timer ticks at half the SYSCLK, so 24000000 times per second
	// so each millisecond is 24000 ticks
	// wait 10 milliseconds in each delay
	while(_CP0_GET_COUNT() < t + 2400*time_ms){}
	}