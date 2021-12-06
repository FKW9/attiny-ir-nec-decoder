/**
 * @file    ir.c
 * @author  Florian W
 * @brief   Very leightweight routine to decode a received IR signal with NEC protocol
 * @version 0.1
 * @date    2021-12-06
 *
 */
#include "ir.h"

volatile static uint8_t overflows;
volatile static uint8_t status;
volatile static uint8_t data[32];
volatile static uint8_t data_ready;

/**
 * @brief Funktion um empfangene IR Daten zu verarbeiten
 *
 * @param data Zeiger auf empfangene Daten
 * @return uint8_t 1 wenn erfolgreich
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

    uint8_t data_received = 0;

    if(data_ready){

        uint8_t command_HB = 0;
        uint8_t command_LB = 0;
        uint8_t power_HB   = 1;
        uint8_t power_LB   = 1;

        // sum up low byte of data (invers of highbyte)
        for (uint8_t i=31; i>23; i--) {
            command_LB += data[i]*power_LB;
            power_LB *= 2;
        }

        // sum up high byte of data
        for (uint8_t i=23; i>15; i--) {
            command_HB += data[i]*power_HB;
            power_HB *= 2;
        }

        // check data if it is correct
        if(command_LB+command_HB==255){
            *data = command_LB;
             data_received = 1;
        } else data_received = 0;

        data_ready = 0;
        GIMSK = (1<<PCIE);
        PCMSK = (1<<PCINT3);

    }

    return data_received;
}

ISR(TIM0_OVF_vect) {
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
            if((time >= 30) && (time <= 40)){
                status = 2;
            } else {
                TCCR0B = 0;
                status = 0;
            }
            break;

        case 2: // end of 4.5ms pulse
            if((time >= 13) && (time <= 21)){
                status = 3;
            } else {
                TCCR0B = 0;
                status = 0;
            }
            break;

        case 3: // end of 512us pulse
            if((time >= 0) && (time <= 4)){
                status = 4;
            } else {
                TCCR0B = 0;
                status = 0;
            }
            break;

        case 4:
            if((time >= 0) && (time <= 9)){

                if (time >= 4){
                    data[index] = 1;
                } else {
                    data[index] = 0;
                }

                index++;

                if(index>31){
                    GIMSK  = 0;
                    PCMSK  = 0;
                    status = 0;
                    data_ready = 1;
                    break;
                }
                status = 3;

            } else{
                TCCR0B = 0;
                status  = 0;
            }
    }
}
