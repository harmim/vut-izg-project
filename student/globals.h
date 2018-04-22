/**
 * @file
 * @brief This file contains global variables.
 *
 * @author Tomáš Milet, imilet@fit.vutbr.cz
 * @author Dominik Harmim <harmim6@gmail.com>
 */


#pragma once


#include <student/fwd.h>
#include <student/linearAlgebra.h>


#ifdef __cplusplus
extern "C" {
#endif


/// This variable contains view matrix of orbit camera.
extern Mat4 viewMatrix;
/// This variable contains projection matrix of orbit camera.
extern Mat4 projectionMatrix;
/// This variable contains camera position in world-space.
extern Vec3 cameraPosition;


#ifdef __cplusplus
}
#endif
