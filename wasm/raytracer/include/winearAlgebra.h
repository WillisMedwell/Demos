#ifndef _WINEAR_ALGEBRA_H_
#define _WINEAR_ALGEBRA_H_
/*
*  Webassembly Linear Algebra library for c.
*  Created by Willis Medwell.
*/

// Basic datatypes.
typedef struct { float i, j, k; } vec3_t;
typedef struct { float x, y, z; } pos3_t;
typedef struct { vec3_t direction; pos3_t origin; } ray3_t;

// Operations.
float  vec3_length(const vec3_t* v);
float  vec3_dot(const vec3_t* v1, const vec3_t* v2);
vec3_t vec3_cross(const vec3_t* v1, const vec3_t* v2);
vec3_t vec3_norm(const vec3_t* v);
vec3_t vec3_add(const vec3_t* v1, const vec3_t* v2);
vec3_t vec3_sub(const vec3_t* v1, const vec3_t* v2);
vec3_t vec3_scale(const vec3_t* v, const float factor);
// Conversions.
pos3_t vec3_to_pos3(const vec3_t v);
vec3_t pos3_to_vec3(const pos3_t p);

// Shape types.
typedef struct { pos3_t position; float radius; } sphere_t;
typedef struct { pos3_t vertices[3]; } triangle_t;
typedef struct { pos3_t vertices[4]; } square_t; 

// Intersections.
float intersection_dist_ray3_sphere(const ray3_t* ray, const sphere_t* sphere);
float intersection_dist_ray3_triangle(const ray3_t* ray, const triangle_t* triangle);
float intersection_dist_ray3_square(const ray3_t* ray, const square_t* square);
#define NO_HIT 3.299999965482712e+38

vec3_t normal_sphere(const pos3_t* pos, const sphere_t* sphere);
vec3_t normal_triangle(const vec3_t* intersection_direction, const triangle_t* triangle);

// Point as traveling a distance from a ray.
pos3_t pos3_along_ray3(const ray3_t* ray, float dist);

#endif
