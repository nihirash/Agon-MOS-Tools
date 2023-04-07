# bin2asm Agon MOS command

## Important notes!

**DEPRECATED** Cause agon-ez80asm implement this feature native. 

**CAUTION** This code contains error - it lost last byte! But feel free discover and hack it. 

## About it(but anyway it isn't actual)

Allows you convert any binary to assembly source to include.

I did it cause [agon-ez80asm](https://github.com/envenomator/agon-ez80asm) didn't support `incbin` directive yet.

I just wanted make bouncing ball demo in assembly but written this instead.

Code written directly on Agon Light and compiled with [agon-ez80asm](https://github.com/envenomator/agon-ez80asm).

## Usage

Copy `bin2asm.bin` into your `/mos` directory of SD Card.

## Compilation

Enter sources directory and call `asm bin2asm.asm`. 

It will produce `bin2asm.bin` binary. *IMPORTANT* It will be built for using as MOS command - won't work after `LOAD` and `RUN`.

You can install into your system just by calling `copy bin2asm.bin /mos/bin2asm.bin`

If you want use it as usual program via `LOAD` and `RUN` you should change origin address from `$B0000` to `40000` in `starcrt.inc` 

## License

Licensed under MIT License