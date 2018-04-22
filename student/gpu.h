/**
 * @file
 * @brief This file contains interface for virtual GPU.
 *
 * @author Tomáš Milet, imilet@fit.vutbr.cz
 * @author Dominik Harmim <harmim6@gmail.com>
 */


#pragma once


#include <stdlib.h>

#include <student/fwd.h>
#include <student/linearAlgebra.h>
#include <student/vertexPuller.h>
#include <student/program.h>


#ifdef __cplusplus
extern "C" {
#endif


/**
 * @brief This function creates GPU handle.
 *
 * This function creates virtual GPU
 *
 * @return GPU handler
 */
GPU cpu_createGPU();

/**
 * @brief This function destroys GPU handle.
 *
 * This function destroys virtual GPU and all its allocated data
 *
 * @param gpu GPU handler
 */
void cpu_destroyGPU(GPU gpu);

/**
 * @brief This function returns handle to uniform values
 *
 * @param gpu GPU handler
 *
 * @return handle to all uniform values
 */
Uniforms gpu_getUniformsHandle(GPU gpu);

/**
 * @brief This function sets viewport size.
 *
 * @param gpu GPU handle
 * @param width width of viewport in pixels
 * @param height height of viewport in pixels
 */
void cpu_setViewportSize(GPU gpu, size_t width, size_t height);

/**
 * @brief This function returns width of viewport.
 *
 * @param gpu GPU handle
 *
 * @return viewport width in pixels
 */
size_t gpu_getViewportWidth(GPU gpu);

/**
 * @brief This function returns height of viewport.
 *
 * @param gpu GPU handle
 *
 * @return viewport height in pixels
 */
size_t gpu_getViewportHeight(GPU gpu);

/**
 * @brief This functions clears color buffer.
 *
 * @param gpu GPU handle
 * @param color clear color
 */
void cpu_clearColor(GPU gpu, const Vec4 *color);

/**
 * @brief This function clears depth buffer.
 *
 * @param gpu GPU handle
 * @param depth depth that will be written to every pixel
 */
void cpu_clearDepth(GPU gpu, float depth);

/**
 * @brief This function returns color of pixel.
 *
 * @param gpu GPU handle
 * @param x x coord of pixel
 * @param y y coord of pixel
 *
 * @return color of pixel
 */
const Vec4 *cpu_getColor(GPU gpu, size_t x, size_t y);

/**
 * @brief This function returns depth of pixel.
 *
 * @param gpu GPU handle
 * @param x x coord of pixel
 * @param y y coord of pixel
 *
 * @return depth of pixel
 */
float gpu_getDepth(GPU gpu, size_t x, size_t y);

/**
 * @brief This function writes depth of pixel into depth buffer on GPU.
 *
 * @param gpu GPU handle
 * @param x x coord of pixel
 * @param y y coord of pixel
 * @param depth new depth of pixel
 */
void gpu_setDepth(GPU gpu, size_t x, size_t y, float depth);

/**
 * @brief This function writes color of pixel into color buffer on GPU.
 *
 * @param gpu GPU handle
 * @param x x coord of pixel
 * @param y y coord of pixel
 * @param color new color of pixel
 */
void gpu_setColor(GPU gpu, size_t x, size_t y, const Vec4 *color);

/**
 * @brief This function returns active vertex puller configuration.
 *
 * @param gpu GPU handle
 *
 * @return vertex puller configuration
 */
const GPUVertexPullerConfiguration *gpu_getActiveVertexPuller(GPU gpu);

/**
 * @brief This function returns active vertex shader.
 *
 * @param gpu GPU handle
 *
 * @return vertex shader
 */
VertexShader gpu_getActiveVertexShader(GPU gpu);

/**
 * @brief This function returns active fragment shader.
 *
 * @param gpu GPU handle
 *
 * @return fragment shader
 */
FragmentShader gpu_getActiveFragmentShader(GPU gpu);

/**
 * @brief This functions returns interpolation type of vertex attributes of
 * output vertex of active program.
 *
 * Vertex attributes are interpolated into fragment attributes.
 *
 * @param gpu GPU handle
 * @param attribIndex attribute index
 *
 * @return interpolation type FLAT NOPERSPECTIVE SMOOTH
 */
InterpolationType gpu_getAttributeInterpolation(GPU gpu, size_t attribIndex);

/**
 * @brief This function returns type of vertex attributes of output vertex of
 * active program.
 *
 * @param gpu GPU handle
 * @param attribIndex attribute index
 *
 * @return type of attribute
 */
AttributeType gpu_getAttributeType(GPU gpu, size_t attribIndex);


#ifdef __cplusplus
}
#endif
