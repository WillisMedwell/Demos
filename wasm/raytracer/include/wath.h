#ifndef _WATH_H_
#define _WATH_H_
/*
*  Webassembly Math library for c.
*  Created by Willis Medwell.
*/

// Basic math functs.
float wsqrt(const float x);
float wisqrt(const float x);  
float wtan(const float x);    
// TODO: float wsin(const float x);
// TODO: float wcos(const float x);
float wmin(const float x, const float y); 
float wmax(const float x, const float y); 
float wabs(const float x);
float rand();

/*
// Linear Algebra related types.
typedef struct { float i, j, k; } vec3_t;
typedef struct { float x, y, z; } pos3_t;
typedef struct { vec3_t direction; pos3_t origin; } ray3_t;

float  vec3_length(const vec3_t* v);
float  vec3_dot(const vec3_t* v1, const vec3_t* v2);
vec3_t vec3_cross(const vec3_t* v1, const vec3_t* v2);
vec3_t vec3_norm(const vec3_t* v);
vec3_t vec3_add(const vec3_t* v1, const vec3_t* v2);
vec3_t vec3_sub(const vec3_t* v1, const vec3_t* v2);
vec3_t vec3_scale(const vec3_t* v, const float factor);
pos3_t vec3_to_pos3(const vec3_t v);
vec3_t pos3_to_vec3(const pos3_t p);


// Shape types.
typedef struct { pos3_t position; float radius; } sphere_t;
typedef struct { pos3_t vertices[3]; } triangle_t;

// Intersection functions.
#define NO_HIT 3.299999965482712e+38

// Calulate distance from ray origin and closet intersection between shapes.
float  intersection_dist_ray3_sphere(const ray3_t* ray, const sphere_t* sphere);
float  intersection_dist_ray3_triangle(const ray3_t* ray, const triangle_t* triangle);
// Calculate position given travel x distance along a ray.
pos3_t ray_dist_pos(const ray3_t* ray, const float dist);
*/
#endif