/**
 * @file
 * @brief This file contains implementation of orbit camera manipulator.
 *
 * @author Tomáš Milet, imilet@fit.vutbr.cz
 * @author Dominik Harmim <harmim6@gmail.com>
 */


#include <assert.h>
#include <math.h>

#include <student/camera.h>


void frustum_Mat4(
	Mat4 *const output, const float left, const float right,
	const float bottom, const float top,
	const float near, const float far
)
{
	assert(output != NULL);
	output->column[0].data[0] = 2.f * near / (right - left);
	output->column[0].data[1] = 0.f;
	output->column[0].data[2] = 0.f;
	output->column[0].data[3] = 0.f;

	output->column[1].data[0] = 0.f;
	output->column[1].data[1] = 2.f * near / (top - bottom);
	output->column[1].data[2] = 0.f;
	output->column[1].data[3] = 0.f;

	output->column[2].data[0] = (right + left) / (right - left);
	output->column[2].data[1] = (top + bottom) / (top - bottom);
	output->column[2].data[2] = -(far + near) / (far - near);
	output->column[2].data[3] = -1.f;

	output->column[3].data[0] = 0.f;
	output->column[3].data[1] = 0.f;
	output->column[3].data[2] = -2.f * far * near / (far - near);
	output->column[3].data[3] = 0.f;
}


void perspective_Mat4(
	Mat4 *const output, const float fovy, const float aspect,
	const float near, const float far
)
{
	assert(output != NULL);
	float leftRight = near * tanf(fovy / 2.f) * aspect;
	float bottomTop = near * tanf(fovy / 2.f);
	frustum_Mat4(output, -leftRight, leftRight, -bottomTop, bottomTop, near, far);
}


void cpu_computeOrbitCamera(
	Mat4 *const viewMatrix, const float angleX, const float angleY,
	const float distance
)
{
	assert(viewMatrix != NULL);
	Mat4 xRotation;
	rotate_Mat4(&xRotation, 1.f, 0.f, 0.f, angleX);
	Mat4 yRotation;
	rotate_Mat4(&yRotation, 0.f, 1.f, 0.f, angleY);
	Mat4 viewRotation;
	multiply_Mat4_Mat4(&viewRotation, &xRotation, &yRotation);
	Mat4 translate;
	translate_Mat4(&translate, 0.f, 0.f, -distance);
	multiply_Mat4_Mat4(viewMatrix, &translate, &viewRotation);
}


void cpu_getCameraPositionFromViewMatrix(
	Vec3 *const cameraPosition, const Mat4 *const viewMatrix
)
{
	assert(cameraPosition != NULL);
	assert(viewMatrix != NULL);
	Vec4 cameraPositionInViewspace;
	init_Vec4(&cameraPositionInViewspace, 0.f, 0.f, 0.f, 1.f);
	Vec4 cameraPositionInWorldSpace;
	Mat4 invertedViewMatrix;
	invert_Mat4(&invertedViewMatrix, viewMatrix);
	multiply_Mat4_Vec4(
		&cameraPositionInWorldSpace, &invertedViewMatrix,
		&cameraPositionInViewspace
	);
	copy_Vec4_To_Vec3(cameraPosition, &cameraPositionInWorldSpace);
}
