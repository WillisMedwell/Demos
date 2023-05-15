#include "winearAlgebra.h"
#include "wath.h"

inline float vec3_length(const vec3_t* v) {
    return wsqrt( vec3_dot(v,v) );
}
inline float vec3_dot(const vec3_t* v1, const vec3_t* v2) {
    return (v1->i*v2->i) + (v1->j*v2->j) + (v1->k*v2->k);
}
inline vec3_t vec3_cross(const vec3_t* v1, const vec3_t* v2) {
    return (vec3_t) {
        .i = v1->j*v2->k - v1->k*v2->j,
        .j = v1->k*v2->i - v1->i*v2->k,
        .k = v1->i*v2->j - v1->j*v2->i
    };
}
inline vec3_t vec3_norm(const vec3_t* v) {
    float factor = wisqrt(vec3_dot(v,v));
    return vec3_scale(v, factor);
}
inline vec3_t vec3_add(const vec3_t* v1, const vec3_t* v2) {
    return (vec3_t){
        .i = v1->i + v2->i, 
        .j = v1->j + v2->j,
        .k = v1->k + v2->k
    };
}
inline vec3_t vec3_sub(const vec3_t* v1, const vec3_t* v2) {
    return (vec3_t) {
        .i = v1->i - v2->i, 
        .j = v1->j - v2->j,
        .k = v1->k - v2->k
    };
}
inline vec3_t vec3_scale(const vec3_t* v, const float factor) {
    return (vec3_t) {
        .i = v->i * factor, 
        .j = v->j * factor,
        .k = v->k * factor
    };
}
inline pos3_t vec3_to_pos3(const vec3_t v) {
    return (pos3_t) { v.i, v.j, v.k };
}
inline vec3_t pos3_to_vec3(const pos3_t p) {
    return (vec3_t) { p.x, p.y, p.z };
}
inline pos3_t pos3_along_ray3(const ray3_t* ray, float dist){ 
    return (pos3_t) {
        .x = ray->origin.x + (dist * ray->direction.i),
        .y = ray->origin.y + (dist * ray->direction.j),
        .z = ray->origin.z + (dist * ray->direction.k)
    };
}
inline float intersection_dist_ray3_sphere(const ray3_t* ray, const sphere_t* sphere) {
    const vec3_t displacement = vec3_sub((vec3_t*)(&ray->origin), (vec3_t*)(&sphere->position) );
	const float A = vec3_dot(&ray->direction, &ray->direction);
	const float B = vec3_dot(&displacement, &ray->direction) * 2.0f;
	const float C = vec3_dot(&displacement, &displacement) - (sphere->radius * sphere->radius);
	const float D = (B * B) - (4.0f * A * C);

    float t = NO_HIT;
    float t1;
    float t2;
	if (D > 0) {
        t2 = (-B + wsqrt(D)) / (2.0f * A);
        t1 = (-B - wsqrt(D)) / (2.0f * A);
        if(t1 > 0.0f && t2 > 0.0f) {
            t = wmin(t1, t2);
        } else if(t1 > 0.0f && t2 < 0.0f) {
            t = t1;
        } else if(t1 < 0.0f && t2 > 0.0f) {
            t = t2;
        }
	}
    return t;
}
inline float intersection_dist_ray3_triangle(const ray3_t* ray, const triangle_t* triangle) { 
    // Ref: https://en.wikipedia.org/wiki/M%C3%B6ller%E2%80%93Trumbore_intersection_algorithm
    vec3_t vertex0 = *(vec3_t*)(&triangle->vertices[0]);
    vec3_t vertex1 = *(vec3_t*)(&triangle->vertices[1]); 
    vec3_t vertex2 = *(vec3_t*)(&triangle->vertices[2]);

    vec3_t edge1, edge2, h, s, q;
    float a,f,u,v;
    edge1 = vec3_sub(&vertex1, &vertex0);
    edge2 = vec3_sub(&vertex2, &vertex0);

    h = vec3_cross(&ray->direction, &edge2);
    a = vec3_dot(&edge1, &h);
    static const float TINY = 0.000001f;
    if (a > -TINY && a < TINY) {
        return NO_HIT;
    }
    f = 1.0/a;
    s = vec3_sub((vec3_t*)&ray->origin, &vertex0);
    u = f * vec3_dot(&s, &h); 

    if (u < 0.0 || u > 1.0) {
        return NO_HIT;
    }
    q = vec3_cross(&s, &edge1);

    v = f * vec3_dot(&ray->direction, &q);
    if (v < 0.0 || u + v > 1.0) {
        return NO_HIT;
    }
    float t = f * vec3_dot(&edge2, &q);
    if (t > TINY) {
        return t;
    }
    else {
        return NO_HIT;
    }
}
inline float intersection_dist_ray3_square(const ray3_t* ray, const square_t* square) { 

}
// need position in which the sphere intersects to calculate normal.
inline vec3_t normal_sphere(const pos3_t* pos, const sphere_t* sphere) {
    return vec3_sub((vec3_t*)pos, (vec3_t*)(&sphere->position));
}
// need direction in which the triangle intersects to calculate normal.
inline vec3_t normal_triangle(const vec3_t* intersection_direction, const triangle_t* triangle) {
    vec3_t edge1 = vec3_sub((vec3_t*)(&triangle->vertices[0]), (vec3_t*)(&triangle->vertices[1]));
    vec3_t edge2 = vec3_sub((vec3_t*)(&triangle->vertices[0]), (vec3_t*)(&triangle->vertices[2]));
    vec3_t normal = vec3_cross(&edge1, &edge2);
    if(vec3_dot(&normal, intersection_direction) > 0){
        normal = vec3_cross(&edge2, &edge1);
    }
    return vec3_norm(&normal);
}