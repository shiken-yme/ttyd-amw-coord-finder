#include <common.h>
#include <stdio.h>

/*
    Program to find working floats that write to a desired address in Great Tree AMW
    Exclusively supports the JP version of the game at this time
    Usage: amw [address]
    Address should be in the 0xC0000000 range for an 8-byte write
    Outputs matching floats in hexadecimal format
*/

// Rough, generous estimates
#define ZPOS_MIN 0x43250000
#define ZPOS_MAX 0xC3090000

// based on math from Zephiles
u32 get_addr_from_coord(u32 coord) {
    return (0x806E0640 + ((coord * 0x88) & 0xFFFFFFFF)) & 0xFFFFFFFF;
}

s32 main(s32 argc, char * argv[]) {
    if (argc < 2) {
        printf("please input an address\n");
        return 0;
    }
    u32 desiredAddr;
    if (sscanf(argv[1], "%x", &desiredAddr) != 1) {
        printf("error parsing address\n");
        return 0;
    }
    u32 curCoord = ZPOS_MIN;
    s32 matches = 0;
    do {
        if (desiredAddr == get_addr_from_coord(curCoord)) {
            printf("0x%X\n", curCoord);
            matches += 1;
        }
        curCoord += 1;
    } while (curCoord < ZPOS_MAX);
    printf("found %d matches for address 0x%X\n", matches, desiredAddr);
    return 0;
}
