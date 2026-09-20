#include <common.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
    Program to find working floats that write to a desired address in AMW
    Usage: amw (function) (NO_EPSILON) [version] [address]
    Valid version inputs are JP, US, EU1, EU2 (see readme for explanation of EU1 and EU2)
    Input an address in the 0x8XXXXXXX range and it will check for writes to both cached and uncached memory
*/

#define ZPOS_MIN -2000.0f
#define ZPOS_MAX 2000.0f
#define EPSILON 0.00001f

typedef union {
    u32 u;
    f32 f;
} Coord;

typedef enum : s32 {
    INVALID_REGION = -1,
    JP,
    US,
    EU1,
    EU2
} Region;

typedef enum : s32 {
    INVALID_FUNCTION = -1,
    SOUND_EFX_STOP,     // SoundEfxStop
    SOUND_SS_STOP_CH,   // SoundSSStopCh
    SOUND_SONG_STOP_CH, // SoundSongStopCh
} Function;

typedef struct {
    const char * name;
    u32 base[4];         // One for each region
    s32 bytesWritten[3]; // SoundSongStopCh writes 4 bytes to cached memory while the others write 2
    u32 multiplier;      // Amount that the coordinate is multipled by
} FunctionData;

const char * versionNames[4] = {"JP", "US", "EU1", "EU2"};

const FunctionData functions[3] = {"SoundEfxStop",
                                   0x806E0640, 0x806EED40, 0x8072FC60, 0x806FB860,
                                   2, 8, 8,
                                   0x88,

                                   "SoundSSStopCh",
                                   0x806E1E80, 0x806F0580, 0x807314A0, 0x806FD0A0,
                                   2, 8, 8,
                                   0x138,

                                   "SoundSongStopCh",
                                   0x806E05E0, 0x80679260, 0x806BA180, 0x80685D80,
                                   4, 4, 4,
                                   0x10};

Function get_function(const char * name) {
    for (s32 i = 0; i < 3; i += 1) {
        if (strcmp(name, functions[i].name) == 0)
            return (Function)i;
    }
    return INVALID_FUNCTION;
}

Region get_region(const char * name) {
    for (s32 i = 0; i < 4; i += 1) {
        if (strcmp(name, versionNames[i]) == 0)
            return (Region)i;
    }
    return INVALID_REGION;
}

s32 main(s32 argc, char * argv[]) {
    // There must be a minimum of three args, including the executable name
    if (argc < 3) {
        printf("Please input a region and an address\n");
        return 0;
    }

    // Loop through all of the args to find the used ones
    bool ignoreEpsilon = false;
    Function func = INVALID_FUNCTION;
    Region region = INVALID_REGION;
    u32 baseAddr = 0;

    for (s32 i = 0; i < argc; i++) {
        // Check if the epsilon should be used
        if (!ignoreEpsilon) {
            if (strcmp(argv[i], "NO_EPSILON") == 0) {
                // Found the arg, so continue to the next one
                ignoreEpsilon = true;
                continue;
            } else {
                // Assume the arg is not for specifying if the epsilon should be used, so continue to the next check
            }
        }

        // Check if specifying the function to use
        if (func == INVALID_FUNCTION) {
            Function tempFunc = get_function(argv[i]);

            if (tempFunc != INVALID_FUNCTION) {
                // Found the arg, so continue to the next one
                func = tempFunc;
                continue;
            } else {
                // Assume the arg is not for specifying the function to use, so continue to the next check
            }
        }

        // Get the region
        if (region == INVALID_REGION) {
            Region tempRegion = get_region(argv[i]);

            if (tempRegion != INVALID_REGION) {
                // Found the arg, so continue to the next one
                region = tempRegion;
                continue;
            } else {
                // Assume the arg is not for specifying the region, so continue to the next check
            }
        }

        // Get the base address
        if (baseAddr == 0) {
            u32 tempBaseAddr;

            if (sscanf(argv[i], "%x", &tempBaseAddr) == 1) {
                // Found the arg, so continue to the next one
                baseAddr = tempBaseAddr;
                continue;
            } else {
                // Assume the arg is not for specifying the base address, so continue to the next check
            }
        }
    }

    // Make sure all of the required args are present
    if (func == INVALID_FUNCTION) {
        // Assume that a function was not specified, so default to SoundEfxStop
        func = SOUND_EFX_STOP;
    }

    if (region == INVALID_REGION) {
        printf("Error parsing region input\n");
        return 0;
    }

    if (baseAddr == 0) {
        printf("Error parsing address input\n");
        return 0;
    }

    // Make sure the base address is a multiple of 0x4, as lower multiples are not possible
    baseAddr &= ~3;

    const FunctionData * funcDataPtr = &functions[(s32)func];
    const u32 startAddr = funcDataPtr->base[(s32)region];
    const s32 * bytesWrittenPtr = funcDataPtr->bytesWritten;
    const u32 multiplier = funcDataPtr->multiplier;

    const u32 addr[3] = {baseAddr, baseAddr + 0x40000000, baseAddr + 0x3FFFFFFC};
    s32 matches = 0;

    Coord curCoord;
    curCoord.u = 0;

    do {
        for (s32 i = 0; i < 3; i += 1) {
            if (curCoord.f >= ZPOS_MIN && curCoord.f <= ZPOS_MAX) {
                if (ignoreEpsilon || (curCoord.f >= EPSILON || curCoord.f <= -EPSILON)) {
                    if (addr[i] == ((startAddr + ((curCoord.u * multiplier) & 0xFFFFFFFF)) & 0xFFFFFFFF)) {
                        printf("Coord 0x%08X (%.8f) writes %d bytes to addr 0x%08X\n",
                               curCoord.u,
                               curCoord.f,
                               bytesWrittenPtr[i],
                               addr[i]);

                        matches += 1;
                    }
                }
            }
        }
        curCoord.u += 1;
    } while (curCoord.u != 0xFFFFFFFF);

    printf("Found %d matches for base address 0x%08X\n", matches, baseAddr);
    return 0;
}
