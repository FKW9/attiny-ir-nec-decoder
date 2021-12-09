# ATTiny IR NEC Decoder

Very light-weight & small-size library for ATTiny13/25/45/85 to decode an IR signal with NEC Protocol.

## Features

The library includes two functions to process the ir data.
- A simple ```ir_get_data``` which simply returns the value of the received command. The command is not checked with its invers and the address is also omitted.
- A function ```ir_get_all_data``` which returns the command and the address of the received message. Both are checked with their invers if errors occured.

## Memory Usage

Standard version for clock frequencies from 1-16MHz

| Function      | RAM         | Flash |
| ------------- |-------------| ----- |
| ```ir_get_data``` | 40 bytes | 516 bytes |
| ```ir_get_all_data``` | 41 bytes | 648 bytes |
| both  | 41 bytes | 706 bytes |

Modified version for clock frequencies up to 7MHz

| Function      | RAM         | Flash |
| ------------- |-------------| ----- |
| ```ir_get_data``` | 36 bytes | 340 bytes |
| ```ir_get_all_data``` | 37 bytes | 472 bytes |
| both  | 37 bytes | 530 bytes |

## Hardware setup

- Used an Arduino Uno as ISP to program the ATTiny45
- Tested with 1MHz and 8MHz
- Used [IR Sensor](https://www.google.com/search?q=vs1838b+datasheet)
