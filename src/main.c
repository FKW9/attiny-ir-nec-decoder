#include <inttypes.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include "../lib/ir/ir.h"

uint8_t ir_data = 0;

int main() {
    /** Pin Change Interrupt initalisieren für IR Receiver **/
    MCUCR = (1<<ISC00);     // Interrupt on any edge
    GIMSK = (1<<PCIE);      // Enable pin change interrupt
    PCMSK = (1<<PCINT3);    // Enable pin change interrupt auf PB3

    DDRB  |= (1<<2);
    PORTB |= (1<<2);        // Enable IR Receiver

    sei();                  // Enable global interrupts

    while(1) {

        if(ir_get_data(&ir_data)){

        }

    }
}

