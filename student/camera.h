/**
 * @file
 * @brief This file contains function declarations for orbit camera manipulator.
 *
 * @author Tomáš Milet, imilet@fit.vutbr.cz
 * @author Dominik Harmim <harmim6@gmail.com>
 */


#pragma once


#ifdef __cplusplus
extern "C" {
#endif


#include <student/fwd.h>
#include <student/linearAlgebra.h>


/**
 * @brief This functions initializes matrix to general perspective matrix.
 *
 * \image html images/frustum.svg "General perspective frustum" width=10cm
 *
 * @param output resulting mat4
 * @param left   left coord of frustum
 * @param right  right coord of frustum
 * @param bottom bottom coord of frustum
 * @param top    top coord of frustum
 * @param near   naer coord of frustum
 * @param far    far  coord of frustum
 */
void frustum_Mat4(
	Mat4 *output, float left, float right, float bottom, float top,
	float near, float far
);

/**
 * @brief This functions initializes matrix to perspective matrix.
 *
 * @param output resulting mat4
 * @param fovy   field of view in y direction
 * @param aspect aspect ratio - width/height of screen
 * @param near   near plane distance
 * @param far    far plane distance
 */
void perspective_Mat4(
	Mat4 *output, float fovy, float aspect, float near, float far
);

/**
 * @brief This function computes view matrix of orbit camera manipulator.
 *
 * \image html images/orbit_camera.svg "orbit camera" width=10cm
 *
 * @param viewMatrix output matrix
 * @param angleX x angle
 * @param angleY y angle
 * @param distance distance between camera center and focus point
 */
void cpu_computeOrbitCamera(
	Mat4 *viewMatrix, float angleX, float angleY, float distance
);

/**
 * @brief This function gets camere position from view matrix.
 *
 * @param cameraPosition output camera position
 * @param viewMatrix view matrix
 */
void cpu_getCameraPositionFromViewMatrix(
	Vec3 *cameraPosition, const Mat4 *viewMatrix
);


#ifdef __cplusplus
}
#endif
