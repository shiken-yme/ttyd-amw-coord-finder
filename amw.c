#include <common.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
    Program to find working floats that write to a desired address in AMW
    Usage: amw (NO_EPSILON) [version] [address]
    Valid version inputs are JP, US, EU1, EU2 (see readme for explanation of EU1 and EU2)
    Input an address in the 0x80000000 range and it will check for writes to both cached and uncached memory
*/

#define ZPOS_MIN -2000.0f
#define ZPOS_MAX 2000.0f
#define EPSILON 0.00001f

typedef union {
    u32 u;
    f32 f;
} Coord;

typedef enum : s32 {
    JP,
    US,
    EU1,
    EU2
} Region;

const char * versionNames[4] = {"JP", "US", "EU1", "EU2"};

s32 bytesWritten[3] = {2, 8, 8};

u32 base[4] = {0x806E0640, 0x806EED40, 0x8072FC60, 0x806FB860};

Region get_region(const char * name) {
    for (s32 i = 0; i < 4; i += 1) {
        if (strcmp(name, versionNames[i]) == 0)
            return (Region)i;
    }
    printf("Error parsing region input\n");
    abort();
}

s32 main(s32 argc, char * argv[]) {
    // There must be a minimum of three args
    if (argc < 3) {
        printf("Please input a region and an address\n");
        return 0;
    }

    // The arg for the epsilon is expected to be first but may also not be passed at all, so use
    // an arbitrary variable to keep track of the current index
    s32 argIndex = 1;

    // Check if the epsilon should be used
    bool chkEpsilon = true;

    if (strcmp(argv[argIndex], "NO_EPSILON") == 0) {
        // Not using the epsilon, so increment argIndex
        chkEpsilon = false;
        argIndex++;
    } else {
        // Assume that the epsilon is being used, so use the current argIndex for the next arg
    }

    // Get the region
    Region region = get_region(argv[argIndex++]);

    // Get the base address
    u32 baseAddr;

    if (sscanf(argv[argIndex++], "%x", &baseAddr) != 1) {
        printf("Error parsing address input\n");
        return 0;
    }

    // Make sure the base address is a multiple of 0x4, as lower multiples are not possible
    baseAddr &= ~3;

    u32 addr[3] = {baseAddr, baseAddr + 0x40000000, baseAddr + 0x3FFFFFFC};
    u32 baseCoord = base[(s32)region];
    Coord curCoord;
    curCoord.u = 0;
    s32 matches = 0;
    do {
        for (s32 i = 0; i < 3; i += 1) {
            if (curCoord.f >= ZPOS_MIN && curCoord.f <= ZPOS_MAX) {
                if (!chkEpsilon || (curCoord.f >= EPSILON || curCoord.f <= -EPSILON)) {
                    if (addr[i] == ((baseCoord + ((curCoord.u * 0x88) & 0xFFFFFFFF)) & 0xFFFFFFFF)) {
                        printf("Coord 0x%X (%.8f) writes %d bytes to addr 0x%X\n", curCoord.u, curCoord.f, bytesWritten[i], addr[i]);
                        matches += 1;
                    }
                }
            }
        }
        curCoord.u += 1;
    } while (curCoord.u != 0xFFFFFFFF);
    printf("Found %d matches for base address 0x%X\n", matches, baseAddr);
    return 0;
}
