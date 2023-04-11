# ESP8266 Updater

## Why we need it?

Olimex ships ESP8266 modules with outdated firmware.

I've checked autoupdate routine and it worked fine and this utility will execute this autoupdate routine.

## How to use?

Copy `esp-update.bin` into `/mos/` directory.

Connect to access point via Network Manager(see netman directory) and execute `esp-update`.

After successful update reconnect with `netman` to access point.

## Requirements

 * ESP8266 chip(Espressif AT firmware out of the box - 115200 baud speed) on UART1. It's important! In other case my app will freeze

 * MOS 1.03 RC3 or above(cause UART api appears here first time)

## Compilation

Code written directly on Agon Light and compiled with [agon-ez80asm](https://github.com/envenomator/agon-ez80asm).

Enter sources directory and call `asm esp-update.asm`. 

It will produce `esp-update.bin` binary. *IMPORTANT* It will be built for using as MOS command - won't work after `LOAD` and `RUN`.

You can install into your system just by calling `copy esp-update.bin /mos/esp-update.bin`

If you want use it as usual program via `LOAD` and `RUN` you should change origin address from `$B0000` to `40000` in `crt.inc` 

## License

Licensed under MIT License