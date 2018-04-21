/**
 * @file
 * @brief This file contains function declaration for swapBuffers function.
 *
 * @author Tomáš Milet, imilet@fit.vutbr.cz
 * @author Dominik Harmim <harmim6@gmail.com>
 */


#pragma once


#include <stdint.h>

#include <SDL2/SDL.h>
#include <student/fwd.h>


#ifdef __cplusplus
extern "C" {
#endif


/**
 * @brief This functions converts one color channel in float to byte.
 *
 * @param value color channel
 *
 * @return color in byte
 */
uint8_t floatColorToUint32(float value);

/**
 * @brief This function swaps framebuffer to window surface.
 * This function should be called at the end of frame.
 *
 * @param surface SDL surface
 * @param gpu GPU handle
 */
void cpu_swapBuffers(SDL_Surface *surface, GPU gpu);


#ifdef __cplusplus
}
#endif
