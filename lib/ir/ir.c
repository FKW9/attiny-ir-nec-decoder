/**
 * @file    ir.c
 * @author  Florian W
 * @brief   Very leightweight routine to decode a received IR signal with NEC protocol
 * @version 0.1
 * @date    2021-12-06
 *
 */
#include "ir.h"

volatile static uint8_t ir_data[32];
volatile static uint8_t ir_data_ready;

/**
 * @brief function to get the received ir data
 *
 * @param data pointer to the received data, value between 0-255
 * @return uint8_t 1 is successfull
 *
 * NEC Protocol:
 * 9ms HIGH Pulse followed by 4.5ms LOW is our starting signal. Then our 32 Bits of data is transmitted
 *
 *                                     |    HIGH BYTE    |     LOW BYTE     |
 * |<-------- 16 Bit Address --------->|< 8 Bit Command >|<8Bit inv.Command>|
 * | 0 0 0 0 0 0 0 0 | 1 1 1 1 1 1 1 1 | 0 0 0 0 0 1 0 1 | 1 1 1 1 1 0 1 0  |
 *
 */
uint8_t ir_get_data(uint8_t *data) {

    uint8_t ir_data_received = 0;

    if(ir_data_ready){

        uint8_t command[2] = {0, 0};
        uint8_t power = 1;

        // sum up low byte of data (invers of highbyte)
        for (uint8_t i=31; i>23; i--) {
            command[0] += ir_data[i]*power;
            power *= 2;
        }

        // sum up high byte of data
        power = 1;
        for (uint8_t i=23; i>15; i--) {
            command[1] += ir_data[i]*power;
            power *= 2;
        }

        // check data if it is correct
        if(command[0]+command[1]==255){
            *data = command[0];
             ir_data_received = 1;
        } else ir_data_received = 0;

        ir_data_ready=0;

        // enable pin change interrupt again
        GIMSK = (1<<PCIE);
        PCMSK = (1<<PCINT3);

    }

    return ir_data_received;
}

/**
 * @brief indicates in which state we are while receiving bits
 * 0 = waiting for rising edge of starting pulse
 * 1 = valid falling edge detected after ~9ms
 * 2 = valid rising edge after ~4.5ms
 * 3 = valid falling edge after ~562us
 * 4 = valid rising edge after 562us OR 1687us, check if it is a 0 or 1, then go back to status 3
 */
volatile static uint8_t status;
volatile static uint8_t overflows; // timer overflows

ISR(TIMER0_OVF_vect) {
    if(status!=0)    overflows++;
    if(overflows>38) status = 0;
}

ISR(PCINT0_vect) {
    static uint8_t time;
    static uint8_t index;

    if(status != 0){
        time      = overflows;
        overflows = 0;
    }

    switch(status){
        case 0:
            index  = 0;
            status = 1;             // data transmission started
            overflows = 0;
            TCCR0B    = (1<<CS00);  // timer starts counting
            break;

        case 1: // end of 9ms pulse
            if(time >= 34){
                status = 2;
            } else {
                TCCR0B = 0;
                status = 0;
            }
            break;

        case 2: // end of 4.5ms pulse
            if((time >= 16) && (time <= 18)){
                status = 3;
            } else {
                TCCR0B = 0;
                status = 0;
            }
            break;

        case 3: // end of 512us pulse
            if((time >= 1) && (time <= 3)){
                status = 4;
            } else {
                TCCR0B = 0;
                status = 0;
            }
            break;

        case 4:
            if(time <= 8){

                if (time >= 4){
                    ir_data[index] = 1; // received a logic one
                } else {
                    ir_data[index] = 0; // reveived a logic zero
                }

                index++;
                status = 3;

                if(index>31){           // ir data ready when we received our 32 bits
                    GIMSK  = 0;         // disable pin change interrupt
                    PCMSK  = 0;
                    status = 0;
                    ir_data_ready = 1;
                    break;
                }

            } else{
                TCCR0B = 0;
                status  = 0;
            }
    }
}
