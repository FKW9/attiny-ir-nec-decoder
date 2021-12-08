# ATTiny IR NEC Decoder

Very light-weight & small-size library for ATTiny13/25/45/85 to decode an IR signal with NEC Protocol.

## Features

The library includes two functions to process the ir data.
- A simple ```ir_get_data``` which simply returns the value of the received command. The command is not checked with its invers and the address is also omitted.
- A function ```ir_get_all_data``` which returns the command and the address of the received message. Both are checked with their invers if errors occured.

## Memory Usage

| Function      | RAM         | Flash |
| ------------- |-------------| ----- |
| ```ir_get_data``` | 38 bytes | 456 bytes |
| ```ir_get_all_data``` | 39 bytes | 588 bytes |
| both  | 39 bytes | 646 bytes |

## Hardware setup

Used an Arduino Uno as ISP to program the ATTiny45.
