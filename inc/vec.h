#ifndef _VEC_H
#define _VEC_H

#include "mat.h"

#define M_PI 3.141592654
#define TO_RADS (3.14/180.0)

#define VEC2_SIZE (sizeof(float)*2)
typedef float vec2_t[2];

#define VEC3_SIZE (sizeof(float)*3)
typedef float vec3_t[3];

#define VEC4_SIZE (sizeof(float)*4)
typedef float vec4_t[4];

#define INT2_SIZE (sizeof(int)*2)
typedef int int2_t[2];

#define INT3_SIZE (sizeof(int)*3)
typedef int int3_t[3];

#define INT4_SIZE (sizeof(int)*4)
typedef int int4_t[4];

#define SHORT2_SIZE (sizeof(short)*2)
typedef short short2_t[2];



extern void   vec2_set(vec2_t out, float x, float y);
extern void   vec2_clr(vec2_t in);
extern void   vec2_cpy(vec2_t out, vec2_t in);
extern int    vec2_cmp(vec2_t in1, vec2_t in2);
extern void   vec2_add(vec2_t in1, vec2_t in2, vec2_t out);
extern void   vec2_sub(vec2_t in1, vec2_t in2, vec2_t out);
extern void   vec2_scl(vec2_t in1, float f, vec2_t out);
extern float  vec2_mag(vec2_t in);
extern void   vec2_nrm(vec2_t in, vec2_t out);
extern void   vec2_print(vec2_t in);

extern void   vec3_set(vec3_t out, float x, float y, float z);
extern void   vec3_clr(vec3_t in);
extern void   vec3_cpy(vec3_t out, vec3_t in);
extern void   vec3_cpy_v2(vec3_t out, vec2_t in);
extern int    vec3_cmp(vec3_t in1, vec3_t in2);
extern void   vec3_add(vec3_t in1, vec3_t in2, vec3_t out);
extern void   vec3_sub(vec3_t in1, vec3_t in2, vec3_t out);
extern void   vec3_scl(vec3_t in, float f, vec3_t out);
extern void   vec3_inv_scl(vec3_t in, float f, vec3_t out);
extern float  vec3_mag(vec3_t in);
extern void   vec3_nrm(vec3_t in, vec3_t out);
extern float  vec3_dot(vec3_t in1, vec3_t in2);
extern void   vec3_cross(vec3_t in1, vec3_t in2, vec3_t out);
extern void   vec3_rot_x(vec3_t in, float angle, vec3_t out);
extern void   vec3_rot_y(vec3_t in, float angle, vec3_t out);
extern void   vec3_rot_z(vec3_t in, float angle, vec3_t out);
extern void   vec3_rot_axes(vec3_t in, float angle, vec3_t axis, vec3_t out);
extern void   vec3_trans(vec3_t in, mat3_t mat, vec3_t out);
extern void   vec3_print(vec3_t in);

extern float  vec3_barry_centric(vec3_t p1, vec3_t p2, vec3_t p3, vec2_t pos);

#endif
