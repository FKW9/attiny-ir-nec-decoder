/**
 * @file    ir.c
 * @author  Florian W
 * @brief   Very leightweight routine to decode a received IR signal with NEC protocol
 * @version 0.1
 * @date    2021-12-06
 *
 * NEC Protocol:
 * 9ms HIGH Pulse followed by 4.5ms LOW is our starting signal. Then our 32 Bits of data is transmitted
 *
 *                   |  logical invers |                 |  logical invers  |
 * |< 8 Bit Address >|< 8 Bit Address >|< 8 Bit Command >|<8Bit inv.Command>|
 * | 0 0 0 0 0 0 0 0 | 1 1 1 1 1 1 1 1 | 0 0 0 0 0 1 0 1 | 1 1 1 1 1 0 1 0  |
 *
 */
#include "ir.h"

volatile static uint8_t ir_data[32];
volatile static uint8_t ir_data_ready;

/**
 * @brief function to get the received ir data
 * It only returns the high byte of the data and does not cross-check it with the invers byte.
 * This method uses the least flash space and is the fastest.
 *
 * @param data pointer to the received data, value between 0-255
 * @return uint8_t 1 is successfull
 */
uint8_t ir_get_data(uint8_t *data) {

    if(ir_data_ready){

        uint8_t command = 0;
        uint8_t power = 1;

        // sum up data of command byte
        for (uint8_t i=23; i>15; i--) {
            command += ir_data[i]*power;
            power *= 2;
        }

        *data = command;
        ir_data_ready=0;

        // enable pin change interrupt again
        GIMSK = (1<<PCIE);
        PCMSK = (1<<PCINT3);

        return 1;
    }
    return 0;
}

/**
 * @brief function to get the received ir data
 * It returns the Address and Data. Both are checked with their received invers.
 *
 * @param data pointer to the received data, value between 0-255
 * @param addr pointer to the received address, value between 0-255
 * @return uint8_t 1 is successfull
 */
uint8_t ir_get_all_data(uint8_t *data, uint8_t *addr) {

    uint8_t ir_data_received = 0;

    if(ir_data_ready){

        uint8_t command[2] = {0, 0};
        uint8_t address[2] = {0, 0};
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

        // sum up low byte of addr (invers of highbyte)
        power = 1;
        for (uint8_t i=15; i>7; i--) {
            address[0] += ir_data[i]*power;
            power *= 2;
        }

        // sum up high byte of addr
        power = 1;
        for (uint8_t i=8; i>0; i--) {
            address[1] += ir_data[i-1]*power;
            power *= 2;
        }

        // check data if it is correct
        if(command[0]+command[1]+address[0]+address[1]==510){
            *data = command[0];
            *addr = address[0];
             ir_data_received = 1;
        } else ir_data_received = 0;

        ir_data_ready = 0;

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
volatile static uint16_t overflows; // timer overflows

ISR(TIMER0_OVF_vect) {
    if(status!=0)
        overflows++;
    if(overflows>(OVF_S_H+OVF_H))
        status = 0;
}

ISR(PCINT0_vect) {
    static uint16_t time;
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
            if(time >= OVF_S_H){
                status = 2;
            } else {
                TCCR0B = 0;
                status = 0;
            }
            break;

        case 2: // end of 4.5ms pulse
            if(time >= OVF_S_L){
                status = 3;
            } else {
                TCCR0B = 0;
                status = 0;
            }
            break;

        case 3: // end of 512us pulse
            if(time >= OVF_H){
                status = 4;
            } else {
                TCCR0B = 0;
                status = 0;
            }
            break;

        case 4:
            if(time < OVF_L+OVF_H){

                if (time > OVF_L-OVF_H){
                    ir_data[index] = 1; // received a logic one
                } else {
                    ir_data[index] = 0; // received a logic zero
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
