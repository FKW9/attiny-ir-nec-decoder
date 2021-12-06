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

#ifndef LIB_IR_H_
#define LIB_IR_H_

/**
 * @brief function to get the received ir data
 *
 * @param data pointer to the received data
 * @return uint8_t 1 is successfull
 */
uint8_t ir_get_data(uint8_t * data);

#endif /* LIB_IR_H_ */
