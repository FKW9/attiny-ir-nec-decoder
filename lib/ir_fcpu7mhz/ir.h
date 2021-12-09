/**
 * @file    ir.h
 * @author  Florian W
 * @brief   Very leightweight routine to decode a received IR signal with NEC protocol
 * @version 0.1
 * @date    2021-12-06
 *
 */
#include <inttypes.h>
#include <avr/interrupt.h>

#ifndef LIB_IR_IR_H_
#define LIB_IR_IR_H_

#define NEC_START_H 9000
#define NEC_START_L 4500
#define NEC_ONE     562
#define NEC_ZERO    1687
#define TIMER_RES   8

#ifndef F_CPU
    #error F_CPU not defined!
#endif

#ifdef F_CPU
    #if F_CPU > 7000000
        #error This version only supports clock speeds below 7MHz!
    #endif
#endif

#define TIMER_OVERFLOW (1000000/(F_CPU/(1 << (TIMER_RES))))
#define OVF_S_H ((uint8_t) (NEC_START_H) / (TIMER_OVERFLOW))
#define OVF_S_L ((uint8_t) (NEC_START_L) / (TIMER_OVERFLOW))
#define OVF_H   ((uint8_t) (NEC_ONE)     / (TIMER_OVERFLOW))
#define OVF_L   ((uint8_t) (NEC_ZERO)    / (TIMER_OVERFLOW))

/**
 * @brief function to get the received ir data
 * It only returns the high byte of the data and does not cross-check it with the invers byte.
 * This method uses the least flash space and is the fastest.
 *
 * @param data value between 0-255
 * @return uint8_t 1 is successfull
 */
uint8_t ir_get_data(uint8_t *data);

/**
 * @brief function to get the received ir data
 * It returns the Address and Data. Both are checked with their received invers.
 *
 * @param data value between 0-255
 * @param addr value between 0-255
 * @return uint8_t 1 is successfull
 */
uint8_t ir_get_all_data(uint8_t *data, uint8_t *addr);

#endif /* LIB_IR_IR_H_ */
