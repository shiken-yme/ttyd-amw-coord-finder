#include <common.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
    Program to find working floats that write to a desired address in AMW
    Usage: amw [version] [address] (NO_EPSILON)
    Valid version inputs are JP, US, EU1, EU2 (see readme for explanation of EU1 and EU2)
    Input an address in the 0x80000000 range and it will check for writes to both cached and uncached memory
*/

typedef union {
    u32 u;
    f32 f;
} Coord;

s32 bytesWritten[3] = {2, 8, 8};

const char * versionNames[4] = {"JP", "US", "EU1", "EU2"};

typedef enum : s32 {
    JP,
    US,
    EU1,
    EU2
} Region;

#define ZPOS_MIN -2000.0f
#define ZPOS_MAX 2000.0f
#define EPSILON 0.00001f

// based on math from Zephiles
u32 get_addr_from_coord(Region region, u32 coord) {
    switch (region) {
    case JP:
        return (0x806E0640 + ((coord * 0x88) & 0xFFFFFFFF)) & 0xFFFFFFFF;
        break;
    case US:
        return (0x806EED40 + ((coord * 0x88) & 0xFFFFFFFF)) & 0xFFFFFFFF;
        break;
    case EU1:
        return (0x8072FC60 + ((coord * 0x88) & 0xFFFFFFFF)) & 0xFFFFFFFF;
        break;
    case EU2:
        return (0x806FB860 + ((coord * 0x88) & 0xFFFFFFFF)) & 0xFFFFFFFF;
        break;
    }
}

Region get_region(const char * name) {
    for (s32 i = 0; i < 4; i += 1) {
        if (strcmp(name, versionNames[i]) == 0)
            return (Region)i;
    }
    printf("Error parsing region input\n");
    abort();
}

s32 main(s32 argc, char * argv[]) {
    if (argc < 3) {
        printf("Please input a region and an address\n");
        return 0;
    }
    Region region = get_region(argv[1]);
    u32 baseAddr;
    if (sscanf(argv[2], "%x", &baseAddr) != 1) {
        printf("Error parsing address input\n");
        return 0;
    }
    bool chkEpsilon = true;
    if (argc >= 4)
        chkEpsilon = strcmp(argv[3], "NO_EPSILON");
    u32 addr[3] = {baseAddr, baseAddr + 0x40000000, baseAddr + 0x3FFFFFFC};
    Coord curCoord;
    curCoord.u = 0;
    s32 matches = 0;
    do {
        for (s32 i = 0; i < 3; i += 1) {
            if (curCoord.f >= ZPOS_MIN && curCoord.f <= ZPOS_MAX) {
                if (!chkEpsilon || (curCoord.f >= EPSILON || curCoord.f <= -EPSILON)) {
                    if (addr[i] == get_addr_from_coord(region, curCoord.u)) {
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
