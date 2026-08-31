# PM:TTYD Great Tree AMW Coordinate Finder
Finds valid floats that write zeroes to an inputted address when setting up Arbitrary Memory Write in Chapter 2.
## Building
`gcc -Ofast -o amw -I ./include amw.c`
## Usage
`amw [addr]`
examples: `amw 0xC0305BE8` `amw C0305BE8`

Writing to an address in uncached memory (0xC0000000 - 0xC17FFFFF) will write 8 bytes of 00.
## G*nerative AI Disclosure
I did not use g*n AI in any way to create this very simple C program
## Credits
* [SeekyCt/spm-headers](https://github.com/SeekyCt/spm-headers) for the included common.h, used for typedef mnemonics
* [Zephiles](https://github.com/Zephiles/) for the math used in get_addr_from_coord()
* Various Paper Mario: The Thousand-Year Door reverse engineers for discovering AMW and finding uses of it in speedruns
