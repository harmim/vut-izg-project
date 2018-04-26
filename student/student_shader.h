/**
 * @file
 * @brief This file contains function declarations of vertex and fragment shader
 * for phong shading/lighting.
 *
 * @author Tomáš Milet, imilet@fit.vutbr.cz
 * @author Dominik Harmim <harmim6@gmail.com>
 */


#pragma once


#include <student/program.h>
#include <student/fwd.h>


#ifdef __cplusplus
extern "C" {
#endif


/**
 * @brief This function represents vertex shader for phong lighting/shading.
 *
 * @param output output vertex
 * @param input input vertex
 * @param gpu GPU handle
 */
void phong_vertexShader(
	GPUVertexShaderOutput *output, const GPUVertexShaderInput *input, GPU gpu
);

/**
 * @brief This function represents fragment shader for phong lighting/shading.
 *
 * @param output output fragment
 * @param input input fragment
 * @param gpu GPU handle
 */
void phong_fragmentShader(
	GPUFragmentShaderOutput *output, const GPUFragmentShaderInput *input,
	GPU gpu
);


#ifdef __cplusplus
}
#endif
