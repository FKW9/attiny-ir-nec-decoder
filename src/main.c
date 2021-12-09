#include <inttypes.h>
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "../lib/ir_fcpu7mhz/ir.h"
// #include "../lib/ir/ir.h"

uint8_t ir_data = 0;
uint8_t ir_addr = 0;

int main() {
    /** initialize pin change interrupt for the IR Receiver **/
    MCUCR = (1<<ISC00);     // Interrupt on any edge
    GIMSK = (1<<PCIE);      // Enable pin change interrupt
    PCMSK = (1<<PCINT3);    // Pin change interrupt auf PB3

    /** initialize timer 0 overflow interrupt for counting IR pulses **/
    TIMSK = (1<<TOIE0);     // Timer/Counter0 Overflow Interrupt Enable

    sei();                  // Enable global interrupts

    while(1) {
        // if(ir_get_data(&ir_data)){
        //     // do something with the data
        // }

        if(ir_get_all_data(&ir_data, &ir_addr)){
            // do something with the data
        }
    }
}