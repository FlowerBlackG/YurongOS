// SPDX-License-Identifier: MulanPSL-2.0

/*
 * SVGA
 *
 * 创建于 2023年7月5日 上海市嘉定区安亭镇
 */

#pragma once

#include <lib/sys/types.h>

namespace video {
namespace svga {

/**
 *
 * https://wiki.osdev.org/VESA_Video_Modes
 */
struct VbeInfo {
   int8_t signature[4];
   uint16_t version;
   uint16_t oemStringPtr[2];
   uint8_t capabilities[4];
   uint16_t videoModePtr[2];
   uint16_t totalMemory;
   uint8_t reserved[492];
} __packed;

/**
 *
 * https://wiki.osdev.org/VESA_Video_Modes
 */
struct VbeModeInfo {

    /**
     *
     * @deprecated only bit 7 should be of interest to you, and it indicates the mode supports a linear frame buffer.
     */
	uint16_t attributes;

    /**
     *
     * @deprecated
     */
	uint8_t windowA;

    /**
     *
     * @deprecated
     */
	uint8_t windowB;

    /**
     *
     * @deprecated used while calculating bank numbers
     */
	uint16_t granularity;
	uint16_t windowSize;
	uint16_t segmentA;
	uint16_t segmentB;

    /**
     *
     * @deprecated used to switch banks from protected mode without returning to real mode
     */
	uint32_t winFuncPtr;

    /**
     * number of bytes per horizontal line
     */
	uint16_t pitch;

    /**
     * width in pixels
     */
	uint16_t width;
	uint16_t height;
	uint8_t wChar;
	uint8_t yChar;
	uint8_t planes;

    /**
     * bits per pixel
     */
	uint8_t bpp;
	uint8_t banks;
	uint8_t memoryModel;
	uint8_t bankSize;
	uint8_t imagePages;
	uint8_t reserved0;

	uint8_t redMask;
	uint8_t redPosition;
	uint8_t greenMask;
	uint8_t greenPosition;
	uint8_t blueMask;
	uint8_t bluePosition;
	uint8_t reservedMask;
	uint8_t reservedPosition;
	uint8_t directColorAttributes;

	uint32_t framebuffer;
	uint32_t offScreenMemOff;

    /**
     * size of memory in the framebuffer but not being displayed on the screen
     */
	uint16_t offScreenMemSize;
	uint8_t reserved1[206];
} __packed;

extern int8_t* screen;
extern int8_t bytesPerPixel;
extern VbeModeInfo* modeInfo;

void init(VbeModeInfo* modeInfo);

inline void putPixel(int32_t x, int32_t y, int32_t color) {
    auto targetAddr = screen + modeInfo->pitch * y + x * bytesPerPixel;
    targetAddr[0] = color & 0xff;
    targetAddr[1] = (color >> 8) & 0xff;
    targetAddr[2] = color >> 16;
}


}
}
