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
