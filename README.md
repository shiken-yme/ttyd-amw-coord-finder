# PM:TTYD AMW Coordinate Finder
Finds valid positions that write zeroes to an inputted address when setting up Arbitrary Memory Write.

This tool is built to get results from `SoundEfxStop` AMW; there are other functions that can theoretically provide different AMW results, but practical uses for them have yet to be found.
## Building
`gcc -Ofast -o amw -I ./include amw.c`
## Usage
`amw [region] [addr] (NO_EPSILON)`
examples: `amw JP 0x80305BE8` `amw EU1 8042CB10 NO_EPSILON`

The program will automatically check for addresses in both cached and uncached memory when provided an address in the 0x8XXXXXXX range.

Writing to an address in uncached memory (0xC0000000 - 0xC17FFFFF) will write 8 bytes of 00; otherwise it will write 2 bytes of 00.

The program automatically excludes position results that are too close to 0; appending `NO_EPSILON` to the input will include these results in the output.

Results lower than -2000 or higher than 2000 are excluded unconditionally.

The PAL version has two separate cases: the EU1 case is true upon resetting the game on 50Hz & upon booting the game on either 50/60Hz; EU2 is true exclusively upon resetting the game on 60Hz.
## G*nerative AI Disclosure
I did not use g*n AI in any way to create this C program.
## Credits
* [SeekyCt/spm-headers](https://github.com/SeekyCt/spm-headers) for the included common.h, used for typedef mnemonics
* [Zephiles](https://github.com/Zephiles/) for math used to derive an address from a coordinate & for guidance on how AMW works
* Various Paper Mario: The Thousand-Year Door reverse engineers for discovering AMW and finding uses of it in speedruns
