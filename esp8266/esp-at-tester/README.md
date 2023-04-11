# ESP8266 AT Commands tester

## How to use?

Load(`load tester.bin`) and run it(`run`) as usual application.

Enter any AT-command(they're case sensitive - use caps lock) and get results.

When you finished your session call `BYE` command.

## Requirements

 * ESP8266 chip(Espressif AT firmware out of the box - 115200 baud speed) on UART1. It's important! In other case my app will freeze

 * MOS 1.03 RC3 or above(cause UART api appears here first time)

## Compilation

Code written directly on Agon Light and compiled with [agon-ez80asm](https://github.com/envenomator/agon-ez80asm).

Enter sources directory and call `asm tester.asm`. 

It will produce `tester.bin` binary.

## License

Licensed under MIT License