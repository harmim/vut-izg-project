/**
 * @file
 * @brief This file contains implementation of swapBuffers function.
 *
 * @author Tomáš Milet, imilet@fit.vutbr.cz
 * @author Dominik Harmim <harmim6@gmail.com>
 */


#include <assert.h>

#include <student/gpu.h>
#include <student/swapBuffers.h>


uint8_t floatColorToUint32(const float value)
{
	return (uint8_t) (value * 255.f);
}


void cpu_swapBuffers(SDL_Surface *const surface, const GPU gpu)
{
	assert(surface != NULL);
	assert(gpu != NULL);
	const size_t w = (size_t) surface->w;
	const size_t h = (size_t) surface->h;
	uint8_t *const pixels = (uint8_t *) surface->pixels;
	for (size_t y = 0; y < h; ++y)
	{
		const size_t reversedY = h - y - 1;
		const size_t rowStart = reversedY * w;
		for (size_t x = 0; x < w; ++x)
		{
			const size_t pixelCoord = rowStart + x;
			const Vec4 *const color = cpu_getColor(gpu, x, y);
			for (size_t c = 0; c < CHANNELS_PER_COLOR; ++c)
			{
				pixels[pixelCoord * CHANNELS_PER_COLOR + c] =
					floatColorToUint32(color->data[c]);
			}
		}
	}
}
