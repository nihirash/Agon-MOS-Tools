# Snail - fast gopher browser for Agon Light(and Agon Light 2)

It's expetimental software that uses ESP8266 with Espressif AT firmware for networking.

![Screen photo](gh-docs/screen.jpeg)

## Requirements

 * Agon Quark(MOS and VDP) 1.03 or later
 * ESP8266 with Espressif AT firmware. Version should be 1.7.5.0 or later(see updating topic). I'm using [this module](https://www.olimex.com/Products/IoT/ESP8266/MOD-WIFI-ESP8266/open-source-hardware). It can be updated OTA without any issues and you don't need any soldering/using programmers to get your Agon online.

## Usage

### First time preparing(should be made only once)

 * Download `netman` and connect to access point via it

 * Download `esp-at-tester` and execute it(This step should be replaced with `esp-update` but it don't work correctly and I don't know why)
 
 * Check firmware version with `AT+GMR` command(they're case sensitive). If it less than `1.7.5.0(Oct 20 2021)`:
 
    + Call Over The Air update with command `AT+CIUPDATE`
 
    + When it will be completed check firmware version with `AT+GMR` - it should be `1.7.5.0(Oct 20 2021)` or later
 
    + Reset your wifi module with `AT+RST` command and restore "factory defaults" with command `AT+RESTORE`
 
    + Exit from this utility with `BYE` command
 
    + Run `netman` again and connect access point.

### Actual usage

 * Download `snail.bin` and put it into some directory of Agon's SD card

 * Enter directory from MOS and call `LOAD SNAIL.BIN` and `RUN`

 * Enjoy gopherspace

## Development

Code written directly on Agon Light and compiled with [agon-ez80asm](https://github.com/envenomator/agon-ez80asm). I'm strongly recommend use latest version of ez80asm - cause it updates often and goes better and better every day.

I'm using [Nano](https://github.com/lennart-benschop/agon-utilities) text editor for Agon Light for writting code directly on Agon.

## Known bugs and limitations

 * TCP error handling isn't implemented correctly yet. But in most cases it should work fine

 * History don't care about "Home" page

 * Downloading files can't be larger than free ram size(cause all files downloading into RAM and saving after it).

## Licensing

Snail browser licensed with [Nihirash's Coffeware License](LICENSE). It isn't hard to respect it.

Happy hacking!

## Supporting project

Feel free support project by code, misspell fixes, sharing info about it or even via [Ko-Fi service](https://ko-fi.com/nihirash).

Stay save and never support war aggression! 