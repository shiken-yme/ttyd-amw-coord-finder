# PM:TTYD AMW Coordinate Finder
Finds valid Z positions that write arbitrary bytes to an inputted address when setting up **Arbitrary Memory Write**, a speedrun technique in Paper Mario: The Thousand-Year Door that can allow for unintended, esoteric behavior. The most notable result of AMW that has been found is being able to set Mario's hitbox size to 0 upon exiting Paper Mode after entering any room.

For an explanation of how AMW works in the scope of TTYD, please refer to [this Google Doc by SolidifiedGaming](https://docs.google.com/document/d/1-YE5YzRGwCECkK8GBotE-4sqT-ODZ-3nHBHJbACtVW0) and [this definition by the Common Weakness Enumeration](https://cwe.mitre.org/data/definitions/123.html) for technical details on the nature of AMW.

This tool is built to get AMW results from the functions `SoundEfxStop`, `SoundSSStopCh`, and `SoundSongStopCh`.  

`SoundEfxStop` is currently the only function that can be used for speedruns; the other two functions can provide different AMW results, but practical usage methods for these functions have yet to be found.

## Building
`gcc -Ofast -o amw -I ./include amw.c`

## Usage
`amw (FunctionName) (NO_EPSILON) [region] [addr]`

examples: `amw JP 0x80305BE8` `amw SoundSSStopCh NO_EPSILON EU1 8042CB10` `amw SoundEfxStop JP 80305BC8` `amw NO_EPSILON SoundSongStopCh JP 0x803180b0`

The program will automatically check for addresses in both cached and uncached memory when provided an address in the 0x8XXXXXXX range. Writing to an address in uncached memory (0xC0000000 - 0xC17FFFFF) with `SoundEfxStop` or `SoundSSStopCh` will write 8 bytes of 00, otherwise they will write 2 bytes of 00. `SoundSongStopCh` behaves differently in that it writes 4 bytes of 0xFF to both cached and uncached memory (tested on Dolphin, please verify on console!)

If a `FunctionName` is not provided, it will default to `SoundEfxStop`.

Position results that are too close to 0 will be excluded by default; passing `NO_EPSILON` before or after `FunctionName` (if provided) will include these results in the output. Results lower than -2000 or higher than 2000 are excluded unconditionally.

### PAL Version
The PAL version has two separate cases that change the results of AMW.

Use **EU1** for:
* After booting the game, regardless of 50/60Hz selection
* After resetting the game while on 50Hz

Use **EU2** for:
* After resetting the game while on 60Hz

## G*nerative AI Disclosure
G*n AI was not used in any way by any contributors to create this program.

## Credits
* [Zephiles](https://github.com/Zephiles/) for various code/documentation contributions
* [SeekyCt/spm-headers](https://github.com/SeekyCt/spm-headers) for the included common.h, used for typedef mnemonics
* Various Paper Mario: The Thousand-Year Door reverse engineers for discovering AMW and finding uses of it in speedruns
