/**
 * @file
 * @brief this file contains implementation of useful linear algebra operations.
 *
 * @author Tomáš Milet, imilet@fit.vutbr.cz
 * @author Dominik Harmim <harmim6@gmail.com>
 */


#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include <student/linearAlgebra.h>


void zero_Vec2(Vec2 *const target)
{
	assert(target != NULL);
	for (size_t i = 0; i < 2; ++i)
	{ target->data[i] = 0.f; }
}


void zero_Vec3(Vec3 *const target)
{
	assert(target != NULL);
	for (size_t i = 0; i < 3; ++i)
	{ target->data[i] = 0.f; }
}


void zero_Vec4(Vec4 *const target)
{
	assert(target != NULL);
	for (size_t i = 0; i < 4; ++i)
	{ target->data[i] = 0.f; }
}


void init_Vec2(Vec2 *const target, const float x, const float y)
{
	assert(target != NULL);
	target->data[0] = x;
	target->data[1] = y;
}


void init_Vec3(Vec3 *const target, const float x, const float y, const float z)
{
	assert(target != NULL);
	target->data[0] = x;
	target->data[1] = y;
	target->data[2] = z;
}


void init_Vec4(
	Vec4 *const target, const float x, const float y, const float z,
	const float w
)
{
	assert(target != NULL);
	target->data[0] = x;
	target->data[1] = y;
	target->data[2] = z;
	target->data[3] = w;
}


void copy_Vec2(Vec2 *const target, const Vec2 *const source)
{
	assert(target != NULL);
	assert(source != NULL);
	for (size_t i = 0; i < 2; ++i)
	{ target->data[i] = source->data[i]; }
}


void copy_Vec3(Vec3 *const target, const Vec3 *const source)
{
	assert(target != NULL);
	assert(source != NULL);
	for (size_t i = 0; i < 3; ++i)
	{ target->data[i] = source->data[i]; }
}


void copy_Vec4(Vec4 *const target, const Vec4 *const source)
{
	assert(target != NULL);
	assert(source != NULL);
	for (size_t i = 0; i < 4; ++i)
	{ target->data[i] = source->data[i]; }
}


void copy_Vec4_To_Vec2(Vec2 *const target, const Vec4 *const source)
{
	assert(target != NULL);
	assert(source != NULL);
	for (size_t i = 0; i < 2; ++i)
	{ target->data[i] = source->data[i]; }
}


void copy_Vec4_To_Vec3(Vec3 *const target, const Vec4 *const source)
{
	assert(target != NULL);
	assert(source != NULL);
	for (size_t i = 0; i < 3; ++i)
	{ target->data[i] = source->data[i]; }
}


void copy_Vec3_To_Vec2(Vec2 *const target, const Vec3 *const source)
{
	assert(target != NULL);
	assert(source != NULL);
	for (size_t i = 0; i < 2; ++i)
	{ target->data[i] = source->data[i]; }
}


void copy_Vec3Float_To_Vec4(
	Vec4 *const target, const Vec3 *const part0, const float part1
)
{
	assert(target != NULL);
	assert(part0 != NULL);
	target->data[0] = part0->data[0];
	target->data[1] = part0->data[1];
	target->data[2] = part0->data[2];
	target->data[3] = part1;
}


void copy_Vec2FloatFloat_To_Vec4(
	Vec4 *const target, const Vec2 *const part0,
	const float part1, const float part2
)
{
	assert(target != NULL);
	assert(part0 != NULL);
	target->data[0] = part0->data[0];
	target->data[1] = part0->data[1];
	target->data[2] = part1;
	target->data[3] = part2;
}


void copy_Vec2Vec2_To_Vec4(
	Vec4 *const target, const Vec2 *const part0, const Vec2 *const part1
)
{
	assert(target != NULL);
	assert(part0 != NULL);
	assert(part1 != NULL);
	target->data[0] = part0->data[0];
	target->data[1] = part0->data[1];
	target->data[2] = part1->data[0];
	target->data[3] = part1->data[1];
}


void copy_FloatVec2Float_To_Vec4(
	Vec4 *const target, const float part0,
	const Vec2 *const part1, const float part2
)
{
	assert(target != NULL);
	assert(part1 != NULL);
	target->data[0] = part0;
	target->data[1] = part1->data[0];
	target->data[2] = part1->data[1];
	target->data[3] = part2;
}


void copy_FloatFloatVec2_To_Vec4(
	Vec4 *const target, const float part0,
	const float part1, const Vec2 *const part2
)
{
	assert(target != NULL);
	assert(part2 != NULL);
	target->data[0] = part0;
	target->data[1] = part1;
	target->data[2] = part2->data[0];
	target->data[3] = part2->data[1];
}


void copy_FloatVec3_To_Vec4(
	Vec4 *const target, const float part0, const Vec3 *const part1
)
{
	assert(target != NULL);
	assert(part1 != NULL);
	target->data[0] = part0;
	target->data[1] = part1->data[0];
	target->data[2] = part1->data[1];
	target->data[3] = part1->data[2];
}


void copy_Vec2Float_ToVec3(
	Vec3 *const target, const Vec2 *const part0, const float part1
)
{
	assert(target != NULL);
	assert(part0 != NULL);
	target->data[0] = part0->data[0];
	target->data[1] = part0->data[1];
	target->data[2] = part1;
}


void copy_FloatVec2_ToVec3(
	Vec3 *const target, const float part0, const Vec2 *const part1
)
{
	assert(target != NULL);
	assert(part1 != NULL);
	target->data[0] = part0;
	target->data[1] = part1->data[0];
	target->data[2] = part1->data[1];
}


void add_Vec2(Vec2 *const output, const Vec2 *const a, const Vec2 *const b)
{
	assert(output != NULL);
	assert(a != NULL);
	assert(b != NULL);
	for (size_t i = 0; i < 2; ++i)
	{ output->data[i] = a->data[i] + b->data[i]; }
}


void add_Vec3(Vec3 *const output, const Vec3 *const a, const Vec3 *const b)
{
	assert(output != NULL);
	assert(a != NULL);
	assert(b != NULL);
	for (size_t i = 0; i < 3; ++i)
	{ output->data[i] = a->data[i] + b->data[i]; }
}


void add_Vec4(Vec4 *const output, const Vec4 *const a, const Vec4 *const b)
{
	assert(output != NULL);
	assert(a != NULL);
	assert(b != NULL);
	for (size_t i = 0; i < 4; ++i)
	{ output->data[i] = a->data[i] + b->data[i]; }
}


void sub_Vec2(Vec2 *const output, const Vec2 *const a, const Vec2 *const b)
{
	assert(output != NULL);
	assert(a != NULL);
	assert(b != NULL);
	for (size_t i = 0; i < 2; ++i)
	{ output->data[i] = a->data[i] - b->data[i]; }
}


void sub_Vec3(Vec3 *const output, const Vec3 *const a, const Vec3 *const b)
{
	assert(output != NULL);
	assert(a != NULL);
	assert(b != NULL);
	for (size_t i = 0; i < 3; ++i)
	{ output->data[i] = a->data[i] - b->data[i]; }
}


void sub_Vec4(Vec4 *const output, const Vec4 *const a, const Vec4 *const b)
{
	assert(output != NULL);
	assert(a != NULL);
	assert(b != NULL);
	for (size_t i = 0; i < 4; ++i)
	{ output->data[i] = a->data[i] - b->data[i]; }
}


void mix_Vec2(
	Vec2 *const output, const Vec2 *const a, const Vec2 *const b, const float t
)
{
	assert(output != NULL);
	assert(a != NULL);
	assert(b != NULL);
	for (size_t i = 0; i < 2; ++i)
	{
		output->data[i] = (1 - t) * a->data[i] + t * b->data[i];
	}
}


void mix_Vec3(
	Vec3 *const output, const Vec3 *const a, const Vec3 *const b, const float t
)
{
	assert(output != NULL);
	assert(a != NULL);
	assert(b != NULL);
	for (size_t i = 0; i < 3; ++i)
	{
		output->data[i] = (1 - t) * a->data[i] + t * b->data[i];
	}
}


void mix_Vec4(
	Vec4 *const output, const Vec4 *const a, const Vec4 *const b, const float t
)
{
	assert(output != NULL);
	assert(a != NULL);
	assert(b != NULL);
	for (size_t i = 0; i < 4; ++i)
	{
		output->data[i] = (1 - t) * a->data[i] + t * b->data[i];
	}
}


float dot_Vec2(const Vec2 *const left, const Vec2 *const right)
{
	assert(left != NULL);
	assert(right != NULL);
	float result = 0;
	for (size_t i = 0; i < 2; ++i)
	{ result += left->data[i] * right->data[i]; }
	return result;
}


float dot_Vec3(const Vec3 *const left, const Vec3 *const right)
{
	assert(left != NULL);
	assert(right != NULL);
	float result = 0;
	for (size_t i = 0; i < 3; ++i)
	{ result += left->data[i] * right->data[i]; }
	return result;
}


float dot_Vec4(const Vec4 *const left, const Vec4 *const right)
{
	assert(left != NULL);
	assert(right != NULL);
	float result = 0;
	for (size_t i = 0; i < 4; ++i)
	{ result += left->data[i] * right->data[i]; }
	return result;
}


void multiply_Vec2_Float(
	Vec2 *const output, const Vec2 *const left, const float constant
)
{
	assert(output != NULL);
	assert(left != NULL);
	for (size_t i = 0; i < 2; ++i)
	{ output->data[i] = left->data[i] * constant; }
}


void multiply_Vec3_Float(
	Vec3 *const output, const Vec3 *const left, const float constant
)
{
	assert(output != NULL);
	assert(left != NULL);
	for (size_t i = 0; i < 3; ++i)
	{ output->data[i] = left->data[i] * constant; }
}


void multiply_Vec4_Float(
	Vec4 *const output, const Vec4 *const left, const float constant
)
{
	assert(output != NULL);
	assert(left != NULL);
	for (size_t i = 0; i < 4; ++i)
	{ output->data[i] = left->data[i] * constant; }
}


float length_Vec2(const Vec2 *const vec)
{
	assert(vec != NULL);
	return sqrtf(dot_Vec2(vec, vec));
}


float length_Vec3(const Vec3 *const vec)
{
	assert(vec != NULL);
	return sqrtf(dot_Vec3(vec, vec));
}


float length_Vec4(const Vec4 *const vec)
{
	assert(vec != NULL);
	return sqrtf(dot_Vec4(vec, vec));
}


void normalize_Vec2(Vec2 *const output, const Vec2 *const input)
{
	assert(output != NULL);
	assert(input != NULL);
	float length = length_Vec2(input);
	if (length == 0.f)
	{ return; }
	length = 1.f / length;
	multiply_Vec2_Float(output, input, length);
}


void normalize_Vec3(Vec3 *const output, const Vec3 *const input)
{
	assert(output != NULL);
	assert(input != NULL);
	float length = length_Vec3(input);
	if (length == 0.f)
	{ return; }
	length = 1.f / length;
	multiply_Vec3_Float(output, input, length);
}


void normalize_Vec4(Vec4 *const output, const Vec4 *const input)
{
	assert(output != NULL);
	assert(input != NULL);
	float length = length_Vec4(input);
	if (length == 0.f)
	{ return; }
	length = 1.f / length;
	multiply_Vec4_Float(output, input, length);
}


void multiply_Mat4_Vec4(
	Vec4 *const output, const Mat4 *const left, const Vec4 *const right
)
{
	assert(output != NULL);
	assert(left != NULL);
	assert(right != NULL);
	assert(output != right);
	for (size_t y = 0; y < 4; ++y)
	{
		output->data[y] = 0.f;
		for (size_t x = 0; x < 4; ++x)
		{
			output->data[y] += left->column[x].data[y] * right->data[x];
		}
	}
}

void multiply_Mat4_Mat4(
	Mat4 *const output, const Mat4 *const left, const Mat4 *const right
)
{
	assert(output != NULL);
	assert(left != NULL);
	assert(right != NULL);
	assert(output != left);
	assert(output != right);
	for (size_t x = 0; x < 4; ++x)
	{
		multiply_Mat4_Vec4(output->column + x, left, right->column + x);
	}
}


void identity_Mat4(Mat4 *const output)
{
	assert(output != NULL);
	for (size_t y = 0; y < 4; ++y)
	{
		for (size_t x = 0; x < 4; ++x)
		{ output->column[x].data[y] = (float) (y == x); }
	}
}


void translate_Mat4(
	Mat4 *const output, const float tx, const float ty, const float tz
)
{
	assert(output != NULL);
	identity_Mat4(output);
	output->column[3].data[0] = tx;
	output->column[3].data[1] = ty;
	output->column[3].data[2] = tz;
}


void scale_Mat4(
	Mat4 *const output, const float sx, const float sy, const float sz
)
{
	assert(output != NULL);
	identity_Mat4(output);
	output->column[0].data[0] = sx;
	output->column[1].data[1] = sy;
	output->column[2].data[2] = sz;
}


void copy_Mat4(Mat4 *const output, const Mat4 *const input)
{
	assert(output != NULL);
	assert(input != NULL);
	assert(output != input);
	for (size_t x = 0; x < 4; ++x)
	{
		for (size_t y = 0; y < 4; ++y)
		{
			output->column[x].data[y] = input->column[x].data[y];
		}
	}
}


void rotate_Mat4(
	Mat4 *const output, const float u, const float v,
	const float w, const float angle
)
{
	assert(output != NULL);
	identity_Mat4(output);

	const float ca = cosf(angle);
	const float sa = sinf(angle);

	output->column[0].data[0] = ca + (1 - ca) * u * u;
	output->column[0].data[1] = 0 + (1 - ca) * u * v + sa * w;
	output->column[0].data[2] = 0 + (1 - ca) * u * w - sa * v;

	output->column[1].data[0] = 0 + (1 - ca) * v * u - sa * w;
	output->column[1].data[1] = ca + (1 - ca) * v * v;
	output->column[1].data[2] = 0 + (1 - ca) * v * w + sa * u;

	output->column[2].data[0] = 0 + (1 - ca) * w * u + sa * v;
	output->column[2].data[1] = 0 + (1 - ca) * w * v - sa * u;
	output->column[2].data[2] = ca + (1 - ca) * w * w;
}


void transpose_Mat4(Mat4 *const output, const Mat4 *const input)
{
	assert(output != NULL);
	assert(input != NULL);
	assert(output != input);
	for (size_t x = 0; x < 4; ++x)
	{
		for (size_t y = 0; x < 4; ++x)
		{
			output->column[y].data[x] = input->column[x].data[y];
		}
	}
}


void invert_Mat4(Mat4 *const output, const Mat4 *const input)
{
	assert(output != NULL);
	assert(input != NULL);
	assert(output != input);
	Mat4 c;
	identity_Mat4(output);
	copy_Mat4(&c, input);
	for (size_t x = 0; x < 4; ++x)
	{
		if (c.column[x].data[x] == 0.f)
		{
			int eliminated = 0;
			for (size_t k = 0; k < 4; ++k)
			{
				if (k == x)
				{ continue; }
				if (c.column[x].data[k] != 0.f)
				{
					eliminated = 1;
					float v = c.column[x].data[k];
					for (size_t l = 0; l < 4; ++l)
					{
						c.column[l].data[x] += c.column[l].data[k] / v;
						output->column[l].data[x] +=
							output->column[l].data[k] / v;
					}
					break;
				}
			}
			if (!eliminated)
			{
				fprintf(stderr, "ERROR: matrix cannot be inverted!\n");
				break;
			}
		}
		if (c.column[x].data[x] != 1.f)
		{
			float v = c.column[x].data[x];
			for (size_t l = 0; l < 4; ++l)
			{
				c.column[l].data[x] /= v;
				output->column[l].data[x] /= v;
			}
		}
		for (size_t y = 0; y < 4; ++y)
		{
			if (y == x)
			{ continue; }
			float v = c.column[x].data[y];
			for (size_t l = 0; l < 4; ++l)
			{
				c.column[l].data[y] -= v * c.column[l].data[x];
				output->column[l].data[y] -= v * output->column[l].data[x];
			}
		}
	}
}


void construct2DLine(Vec3 *const line, const Vec2 *const a, const Vec2 *const b)
{
	assert(line != NULL);
	assert(a != NULL);
	assert(b != NULL);
	// line normal
	line->data[0] = a->data[1] - b->data[1];
	line->data[1] = b->data[0] - a->data[0];
	line->data[2] = 0.f;
	// normalization
	normalize_Vec3(line, line);
	// coefficient c
	line->data[2] = -line->data[0] * a->data[0] - line->data[1] * a->data[1];
}


float distanceTo2DLine(const Vec3 *const line, const Vec2 *const vertex)
{
	assert(line != NULL);
	assert(vertex != NULL);
	return line->data[0] * vertex->data[0] + line->data[1] * vertex->data[1] +
		line->data[2];
}


void reflect(
	Vec3 *const output, const Vec3 *const incident, const Vec3 *const normal
)
{
	assert(output != NULL);
	assert(incident != NULL);
	assert(normal != NULL);
	const float scale = 2.f * dot_Vec3(normal, incident);
	Vec3 scaledNormal;
	multiply_Vec3_Float(&scaledNormal, normal, scale);
	sub_Vec3(output, incident, &scaledNormal);
}
