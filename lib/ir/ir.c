/**
 * @file ir.c
 * @author Florian W
 * @brief Routine um empfangenes IR Signal mit NEC Protokoll zu dekodieren.
 * @version 0.1
 * @date 2021-12-06
 *
 * @copyright Copyright (c) 2021
 *
 */
#include "ir.h"

volatile static uint8_t overflows;
volatile static uint8_t status;
volatile static uint8_t daten[32];
volatile static uint8_t daten_fertig;

/**
 * @brief Funktion um empfangene IR Daten zu verarbeiten
 *
 * @param data Zeiger auf empfangene Daten
 * @return uint8_t 1 wenn erfolgreich
 */
uint8_t ir_get_data(uint8_t *data) {

    uint8_t daten_empfangen = FALSE;

    if(daten_fertig){

        uint8_t command_HB = 0;
        uint8_t command_LB = 0;
        uint8_t power_HB   = 1;
        uint8_t power_LB   = 1;

        /**
         * NEC Protokoll:
         * 9ms HIGH Puls gefolgt von 4.5ms LOW ist Startsignal, danach 32Bit Daten
         *
         * |<-------- 16 Bit Adresse --------->|< 8 Bit Command >|<8Bit inv.Command>|
         * | 0 0 0 0 0 0 0 0 | 1 1 1 1 1 1 1 1 | 0 0 0 0 0 1 0 1 | 1 1 1 1 1 0 1 0  |
         *                                     |    HIGH BYTE    |     LOW BYTE     |
         */

        // rechte 8 Bit aufsummieren (invers Befehl)
        for (uint8_t i=31; i>23; i--) {
            command_LB += daten[i]*power_LB;
            power_LB *= 2;
        }

        // linken 8 Bit aufsummieren (Befehl)
        for (uint8_t i=23; i>15; i--) {
            command_HB += daten[i]*power_HB;
            power_HB *= 2;
        }

        // Daten korrekt empfangen?
        // Befehl muss mit invertierten Befhl übereinstimmen -> CMD == ~INV_CMD bzw. in Summe beide 255
        if(command_LB+command_HB==255){
            *data = command_LB;
             daten_empfangen = TRUE;
        } else daten_empfangen = FALSE;

        daten_fertig = 0;
        GIMSK = (1<<PCIE);
        PCMSK = (1<<PCINT3);

    }

    return daten_empfangen;
}

ISR(TIM0_OVF_vect) {
    if(status!=0)    overflows++;
    if(overflows>38) status = 0;
}

ISR(PCINT0_vect) {
    static uint8_t zeit;
    static uint8_t index;

    if(status != 0){
        zeit      = overflows;
        overflows = 0;
    }

    switch(status){

        case 0:
            index  = 0;
            status = 1;             //Datenübertragung beginnt
            overflows = 0;          //
            TCCR0B    = (1<<CS00);  //Zählung starten
            break;

        case 1: //Ende 9ms Puls
            if((zeit >= 30) && (zeit <= 40)){
                status = 2;
            } else {
                TCCR0B = 0;
                status = 0;
            }
            break;

        case 2: //Ende 4.5ms Puls
            if((zeit >= 13) && (zeit <= 21)){
                status = 3;
            } else {
                TCCR0B = 0;
                status = 0;
            }
            break;

        case 3: //Ende 512us Puls
            if((zeit >= 0) && (zeit <= 4)){
                status = 4;
            } else {
                TCCR0B = 0;
                status = 0;
            }
            break;

        case 4:

            if((zeit >= 0) && (zeit <= 9)){

                if (zeit >= 4){
                    daten[index] = 1;
                } else {
                    daten[index] = 0;
                }

                index++;

                if(index>31){
                    GIMSK  = 0;
                    PCMSK  = 0;
                    status = 0;
                    daten_fertig = 1;
                    break;
                }
                status = 3;

            } else{
                TCCR0B = 0;
                status  = 0;
            }
    }
}
