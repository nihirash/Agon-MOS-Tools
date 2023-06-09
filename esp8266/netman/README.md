# Network Manager for ESP8266 on UART1

## About

Simple network manager that inits your ESP-chip and connects it to your access point.

I'm using it with [Olimex esp8266 dongle](https://www.olimex.com/Products/IoT/ESP8266/MOD-WIFI-ESP8266/open-source-hardware).  

## How to use it?

Copy binary into `/mos/` directory and call netman command. 

In case error you can try restart tool. 

## Requirements

 * ESP8266 chip(Espressif AT firmware out of the box - 115200 baud speed) on UART1. It's important! In other case my app will freeze

 * MOS 1.03 RC3 or above(cause UART api appears here first time)

## Compilation

Code written directly on Agon Light and compiled with [agon-ez80asm](https://github.com/envenomator/agon-ez80asm).

Enter sources directory and call `asm netman.asm`. 

It will produce `netman.bin` binary. *IMPORTANT* It will be built for using as MOS command - won't work after `LOAD` and `RUN`.

You can install into your system just by calling `copy netman.bin /mos/netman.bin`

If you want use it as usual program via `LOAD` and `RUN` you should change origin address from `$B0000` to `40000` in `crt.inc` 

## License

Licensed under MIT License