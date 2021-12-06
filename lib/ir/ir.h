/**
 * @file ir.h
 * @author Florian W
 * @brief Routine um empfangenes IR Signal mit NEC Protokoll zu dekodieren.
 * @version 0.1
 * @date 2021-12-06
 *
 * @copyright Copyright (c) 2021
 *
 */
#include <inttypes.h>
#include <avr/interrupt.h>

#ifndef LIB_IR_H_
#define LIB_IR_H_

#define TRUE  1
#define FALSE 0

#define VOL_MINUS   93
#define PLAY_PAUSE  157
#define VOL_PLUS    29
#define SETUP       221
#define ARROW_UP    253
#define ARROW_DOWN  103
#define ARROW_LEFT  31
#define ARROW_RIGHT 111
#define ENTER_SAVE  87
#define STOP_MODE   61
#define TEN_PLUS    151
#define GO_BACK     79
#define ONE         207
#define TWO         231
#define THREE       133
#define FOUR        239
#define FIVE        199
#define SIX         165
#define SEVEN       189
#define EIGHT       181
#define NINE        173

/**
 * @brief Funktion um empfangene IR Daten zu verarbeiten
 *
 * @param data Zeiger auf empfangene Daten
 * @return uint8_t 1 wenn erfolgreich
 */
uint8_t ir_get_data(uint8_t * data);

#endif /* LIB_IR_H_ */
