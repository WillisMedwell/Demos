#include "wath.h"
#include "winearAlgebra.h"

#include <assert.h>
#include <stdio.h>
#include <stdint.h>

#define MOE 0.0005f // MARGIN OF ERROR.
#define CYCLES 100

//  declarations for all wath.h tests.
void test_wsqrt();  
void test_wisqrt(); 
void test_wtan();    
void test_wmin(); 
void test_wmax(); 
void test_vec3_length();
void test_vec3_dot();
void test_vec3_cross();
void test_vec3_norm();
void test_vec3_add();
void test_vec3_sub();
void test_vec3_scale();

void test_intersection_dist_ray3_sphere();
void test_intersection_dist_ray3_triangle();
void test_pos3_along_ray3();

void test_normal_sphere();
void test_normal_triangle();


int main() {
    printf("-----------------------------\n");
    printf("Beginning \"wath.h\" tests...\n");
    printf("-----------------------------\n");

    test_wsqrt();
    test_wisqrt();
    test_wtan();
    test_wmin();
    test_wmax();

    test_vec3_length();
    test_vec3_dot();
    test_vec3_cross();
    test_vec3_norm();
    test_vec3_add();
    test_vec3_sub();
    test_vec3_scale();

    test_intersection_dist_ray3_sphere();
    test_intersection_dist_ray3_triangle();

    test_pos3_along_ray3();
    test_normal_sphere();
    test_normal_triangle();

    

    printf("-------------------------------\n");
    printf("Beginning \"render.h\" tests...\n");
    printf("-------------------------------\n");
    //assert(0 && "TODO: make rendering tests");
    return 0;
}

// expected result on left and function result on right.
void validate_result(float expted, float actual, const char* fn) {
    if( (expted - MOE) < actual && actual < (expted + MOE) ) {
        return; // Passed.
    } else {
        printf("%s = FAILED\n", fn);
        printf("-> expected = %f\n-> actual = %f\n", expted, actual);
        assert(0);
    }
}
void validate_vec_result(vec3_t expted, vec3_t actual, const char* fn) { 
    int i = (expted.i - MOE) < actual.i && actual.i < (expted.i + MOE);
    int j = (expted.j - MOE) < actual.j && actual.j < (expted.j + MOE);
    int k = (expted.k - MOE) < actual.k && actual.k < (expted.k + MOE);

    if(i && j && k) {  
        return; // Passed.
    } else {
        printf("%s = FAILED", fn);
        printf(" = FAILED\n");
        printf("-> expected = {%f,%f,%f}\n", expted.i, expted.j, expted.k);
        printf("-> actual   = {%f,%f,%f}\n", actual.i, actual.j, actual.k);
        assert(0);
    }
}
// definitions
void test_wsqrt() {
    printf("wsqrt(x)");
    validate_result( 0.0f,          wsqrt(0.0f),     " test 0");
    validate_result( 0.5f,          wsqrt(0.25f),    " test 1");
    validate_result( 1.0f,          wsqrt(1.0f),     " test 2");
    validate_result( 1.4142135624f, wsqrt(2.0f),     " test 3");
    validate_result( 2.2360679775f, wsqrt(5.0f),     " test 4");
    validate_result( 7.0f,          wsqrt(49.0f),    " test 5");
    validate_result( 10.0f,         wsqrt(100.0f),   " test 6");
    validate_result( 31.622776602f, wsqrt(1000.0f),  " test 7");
    validate_result( 100.0f,        wsqrt(10000.0f), " test 8");

    uint64_t total, count;
    total = 0;
    for(count = 0; count < CYCLES; count++) {
        uint64_t start = __builtin_ia32_rdtsc();
        float __attribute__((unused)) result = 1.02342 * result;
        uint64_t end = __builtin_ia32_rdtsc();
        total += end - start;
    }
    printf("\t\t\t\t= passed\tCPU Cycles: %d\n", total/count);
    return;
}
void test_wisqrt() {
    printf("wisqrt(x)");
    validate_result( 1/0.5f,          wisqrt(0.25f),    " test 1");
    validate_result( 1/1.0f,          wisqrt(1.0f),     " test 2");
    validate_result( 1/1.4142135624f, wisqrt(2.0f),     " test 3");
    validate_result( 1/2.2360679775f, wisqrt(5.0f),     " test 4");
    validate_result( 1/7.0f,          wisqrt(49.0f),    " test 5");
    validate_result( 1/10.0f,         wisqrt(100.0f),   " test 6");
    validate_result( 1/31.622776602f, wisqrt(1000.0f),  " test 7");
    validate_result( 1/100.0f,        wisqrt(10000.0f), " test 8");
    
    uint64_t total, count;
    total = 0;
    for(count = 0; count < CYCLES; count++) {
        uint64_t start = __builtin_ia32_rdtsc();
        float result __attribute__((unused)) = wisqrt(56.01f);
        uint64_t end = __builtin_ia32_rdtsc();
        total += end - start;
    }
    printf("\t\t\t\t= passed\tCPU Cycles: %d\n", total/count);
}
void test_wtan() {
    printf("wtan(x)");
    validate_result( 0.5773505684f, wtan(0.523599f), " test 1"); // 30 degrees
    validate_result( 1.0f,          wtan(0.785398f), " test 2"); // 45 degrees
    validate_result( 1.7320606028f, wtan(1.0472f),   " test 3"); // 60 degrees
    uint64_t total, count;
    total = 0;
    for(count = 0; count < CYCLES; count++) {
        uint64_t start = __builtin_ia32_rdtsc();
        float __attribute__((unused)) result = wtan(0.785398f);
        uint64_t end = __builtin_ia32_rdtsc();
        total += end - start;
    }
    printf("\t\t\t\t\t= passed\tCPU Cycles: %d\n", total/count);
}
void test_wmin() {
    printf("wmin(x)");
    validate_result( -1.0f, wmin(0.0f, -1.0f), " test 1");
    validate_result( -1.0f, wmin(-1.0f, 0.0f), " test 2");
    validate_result( 1.0f,  wmin(1.5f, 1.0f),  " test 3");
    validate_result( 1.0f,  wmin(1.0f, 1.5f),  " test 4");
    uint64_t total, count;
    total = 0;
    for(count = 0; count < CYCLES; count++) {
        uint64_t start = __builtin_ia32_rdtsc();
        float result __attribute__((unused)) = wmin(1.0f, 1.5f);
        uint64_t end = __builtin_ia32_rdtsc();
        total += end - start;
    }
    printf("\t\t\t\t\t= passed\tCPU Cycles: %d\n", total/count);
} 
void test_wmax() {
    printf("wmin(x)");
    validate_result( 0.0f, wmax(0.0f, -1.0f), " test 1");
    validate_result( 0.0f, wmax(-1.0f, 0.0f), " test 2");
    validate_result( 1.5f, wmax(1.5f, 1.0f),  " test 3");
    validate_result( 1.5f, wmax(1.0f, 1.5f),  " test 4");
    uint64_t total, count;
    total = 0;
    for(count = 0; count < CYCLES; count++) {
        uint64_t start = __builtin_ia32_rdtsc();
        float __attribute__((unused)) result =  wmax(1.0f, 1.5f);
        uint64_t end = __builtin_ia32_rdtsc();
        total += end - start;
    }
    printf("\t\t\t\t\t= passed\tCPU Cycles: %d\n", total/count);
} 
void test_vec3_length() {
    printf("vec3_length(x)");
    const vec3_t a = (vec3_t) {   0.0f,   0.0f,   0.0f };
    const vec3_t b = (vec3_t) {   1.0f,   0.0f,   0.0f };
    const vec3_t c = (vec3_t) {   1.0f,  -1.0f,   0.0f };
    const vec3_t d = (vec3_t) { -32.0f,  69.0f,  89.0f };
    const vec3_t e = (vec3_t) {  32.0f, -69.0f,  89.0f };
    const vec3_t f = (vec3_t) {  32.0f,  69.0f, -89.0f };
    const vec3_t g = (vec3_t) {  32.0f,  69.0f,  89.0f };

    validate_result( 0.0f,          vec3_length(&a), " test 1");
    validate_result( 1.0f,          vec3_length(&b), " test 2");
    validate_result( 1.4142135623f, vec3_length(&c), " test 3");
    validate_result( 117.07262703f, vec3_length(&d), " test 4");
    validate_result( 117.07262703f, vec3_length(&e), " test 5");
    validate_result( 117.07262703f, vec3_length(&f), " test 6");
    validate_result( 117.07262703f, vec3_length(&g), " test 7");
    
    uint64_t total, count;
    total = 0;
    for(count = 0; count < CYCLES; count++) {
        uint64_t start = __builtin_ia32_rdtsc();
        float result = vec3_length(&g);
        uint64_t end = __builtin_ia32_rdtsc();
        total += end - start;
    }
    printf("\t\t\t\t= passed\tCPU Cycles: %d\n", total/count);
}
void test_vec3_dot() {
    printf("vec3_dot(x)");
    const vec3_t a = (vec3_t) {   0.0f,   0.0f,   0.0f };
    const vec3_t b = (vec3_t) {   1.0f,   0.0f,   0.0f };
    const vec3_t c = (vec3_t) {   1.0f,  -1.0f,   0.0f };
    const vec3_t d = (vec3_t) { -32.0f,  69.0f,  89.0f };
    const vec3_t e = (vec3_t) {  32.0f, -69.0f,  89.0f };
    const vec3_t f = (vec3_t) {  32.0f,  69.0f, -89.0f };
    const vec3_t g = (vec3_t) {  32.0f,  69.0f,  89.0f };

    validate_result(   0.0f,   vec3_dot(&a, &a), " test 1");
    validate_result(   0.0f,   vec3_dot(&a, &g), " test 2");
    validate_result(   1.0f,   vec3_dot(&b, &c), " test 3");
    validate_result( 13706.0f, vec3_dot(&d, &d), " test 4");
    validate_result(-11658.0f, vec3_dot(&e, &f), " test 5");

    uint64_t total, count;
    total = 0;
    for(count = 0; count < CYCLES; count++) {
        uint64_t start = __builtin_ia32_rdtsc();
        float result =  vec3_dot(&b, &c);
        uint64_t end = __builtin_ia32_rdtsc();
        total += end - start;
    }
    printf("\t\t\t\t= passed\tCPU Cycles: %d\n", total/count);
}
void test_vec3_cross() {
    printf("vec3_cross(x)");
    const vec3_t a = (vec3_t) {   0.0f,   0.0f,   0.0f };
    const vec3_t b = (vec3_t) { -32.0f,  69.0f,  89.0f };
    const vec3_t c = (vec3_t) {  32.0f, -69.0f,  89.0f };
    const vec3_t d = (vec3_t) {  32.0f,  69.0f, -89.0f };
    const vec3_t e = (vec3_t) {  32.0f,  69.0f,  89.0f };

    validate_vec_result( (vec3_t){0,0,0},           vec3_cross(&a,&a), " test 1");
    validate_vec_result( (vec3_t){12282, 5696, 0},  vec3_cross(&b,&c), " test 2");
    validate_vec_result( (vec3_t){0, 5696, 4416},   vec3_cross(&c,&d), " test 3");
    validate_vec_result( (vec3_t){12282, -5696, 0}, vec3_cross(&d,&e), " test 4");
    validate_vec_result( (vec3_t){0, 0, 0},         vec3_cross(&e,&e), " test 5");
    printf("\t\t\t\t= passed \n");
}
void test_vec3_norm() {
    printf("vec3_norm(x)");
    const vec3_t a = (vec3_t) {   0.0f,   0.0f,   0.0f };
    const vec3_t b = (vec3_t) { -32.0f,  69.0f,  89.0f };
    const vec3_t c = (vec3_t) {  32.0f, -69.0f,  89.0f };
    const vec3_t d = (vec3_t) {  32.0f,  69.0f, -89.0f };
    const vec3_t e = (vec3_t) {  32.0f,  69.0f,  89.0f };
    const vec3_t f = (vec3_t) {   0.1f,   0.2f,   1.0f };

    validate_vec_result( (vec3_t){0,0,0},                                       vec3_norm(&a), " test 1");
    validate_vec_result( (vec3_t){-0.273334602f,  0.589377737f,  0.760211863f}, vec3_norm(&b), " test 2");
    validate_vec_result( (vec3_t){ 0.273334602f, -0.589377737f,  0.760211863f}, vec3_norm(&c), " test 3");
    validate_vec_result( (vec3_t){ 0.273334602f,  0.589377737f, -0.760211863f}, vec3_norm(&d), " test 4");
    validate_vec_result( (vec3_t){ 0.273334602f,  0.589377737f,  0.760211863f}, vec3_norm(&e), " test 5");
    validate_vec_result( (vec3_t){ 0.097590007f,  0.195180014f,  0.975900072f}, vec3_norm(&f), " test 6");
    printf("\t\t\t\t= passed \n");
}
void test_vec3_add() {
    printf("vec3_add(x)");
    const vec3_t a = (vec3_t) {   0.0f,   0.0f,   0.0f };
    const vec3_t b = (vec3_t) {   1.0f,   0.0f,   0.0f };
    const vec3_t c = (vec3_t) {   0.0f,   1.0f,   0.0f };
    const vec3_t d = (vec3_t) {   0.0f,   0.0f,   1.0f };
    const vec3_t e = (vec3_t) {   1.0f,   1.0f,   1.0f };
    validate_vec_result((vec3_t){ 0.0f, 0.0f, 0.0f}, vec3_add(&a, &a), " test 1");
    validate_vec_result((vec3_t){ 1.0f, 1.0f, 1.0f}, vec3_add(&a, &e), " test 2");
    validate_vec_result((vec3_t){ 1.0f, 1.0f, 0.0f}, vec3_add(&b, &c), " test 3");
    validate_vec_result((vec3_t){ 0.0f, 1.0f, 1.0f}, vec3_add(&c, &d), " test 4");
    validate_vec_result((vec3_t){ 2.0f, 2.0f, 2.0f}, vec3_add(&e, &e), " test 5");
    printf("\t\t\t\t= passed \n");
}
void test_vec3_sub() {
    printf("vec3_sub(x)");
    const vec3_t a = (vec3_t) {   0.0f,   0.0f,   0.0f };
    const vec3_t b = (vec3_t) {   1.0f,   0.0f,   0.0f };
    const vec3_t c = (vec3_t) {   0.0f,   1.0f,   0.0f };
    const vec3_t d = (vec3_t) {   0.0f,   0.0f,   1.0f };
    const vec3_t e = (vec3_t) {   1.0f,   1.0f,   1.0f };
    validate_vec_result((vec3_t){  0.0f,  0.0f,  0.0f}, vec3_sub(&a, &a), " test 1");
    validate_vec_result((vec3_t){ -1.0f, -1.0f, -1.0f}, vec3_sub(&a, &e), " test 2");
    validate_vec_result((vec3_t){  1.0f, -1.0f,  0.0f}, vec3_sub(&b, &c), " test 3");
    validate_vec_result((vec3_t){  0.0f,  1.0f, -1.0f}, vec3_sub(&c, &d), " test 4");
    validate_vec_result((vec3_t){  0.0f,  0.0f,  0.0f}, vec3_sub(&e, &e), " test 5");
    uint64_t total, count;
    total = 0;
    for(count = 0; count < CYCLES; count++) {
        uint64_t start = __builtin_ia32_rdtsc();
        vec3_t result = vec3_sub(&c, &d);
        uint64_t end = __builtin_ia32_rdtsc();
        total += end - start;
    }
    printf("\t\t\t\t= passed\tCPU Cycles: %d\n", total/count);
}
void test_vec3_scale() {
    printf("vec3_scale(x)");
    const vec3_t a = (vec3_t) {   0.0f,   0.0f,   0.0f };
    const vec3_t b = (vec3_t) {   1.0f,   0.0f,   0.0f };
    const vec3_t c = (vec3_t) {   0.0f,   1.0f,   0.0f };
    const vec3_t d = (vec3_t) {   0.0f,   0.0f,   1.0f };
    const vec3_t e = (vec3_t) {   1.0f,   1.0f,   1.0f };
    validate_vec_result((vec3_t){     0.0f,     0.0f,     0.0f}, vec3_scale(&a,    0.0f), " test 1");
    validate_vec_result((vec3_t){     0.0f,     0.0f,     0.0f}, vec3_scale(&a,    0.0f), " test 2");
    validate_vec_result((vec3_t){     1.0f,     1.0f,     1.0f}, vec3_scale(&e,    1.0f), " test 3");
    validate_vec_result((vec3_t){     0.0f,    0.25f,     0.0f}, vec3_scale(&c,   0.25f), " test 4");
    validate_vec_result((vec3_t){  1000.0f,  1000.0f,  1000.0f}, vec3_scale(&e, 1000.0f), " test 5");
    uint64_t total, count;
    total = 0;
    for(count = 0; count < CYCLES; count++) {
        uint64_t start = __builtin_ia32_rdtsc();
        vec3_t result = vec3_scale(&e, 1000.0f);
        uint64_t end = __builtin_ia32_rdtsc();
        total += end - start;
    }
    printf("\t\t\t\t= passed\tCPU Cycles: %d\n", total/count);
}

void test_intersection_dist_ray3_sphere() {
    printf("intersection_dist_ray3_sphere(x)");
    const sphere_t s1 = (sphere_t) {
        .position = (pos3_t) { 0, 0, 2 },
        .radius = 1.0f
    };
    const ray3_t r1 = (ray3_t) {
        .origin = (pos3_t) { 0,0,0 },
        .direction = (vec3_t) {0,0,1}
    };
    const sphere_t s2 = (sphere_t) {
        .position = (pos3_t) { 2, 2, 2 },
        .radius = 1.0f
    };
    ray3_t r2 = (ray3_t) {
        .origin = (pos3_t) { 0,0,0 },
        .direction = (vec3_t) {1,1,1 }
    };

    validate_result(   1.0f, intersection_dist_ray3_sphere(&r1, &s1), " test 1");
    validate_result(1.4226f, intersection_dist_ray3_sphere(&r2, &s2), " test 2");
    float length = vec3_length(&r2.direction);
    r2.direction = vec3_norm(&r2.direction);
    validate_result(1.4226f*length, intersection_dist_ray3_sphere(&r2, &s2), " test 3");

    uint64_t total, count;
    total = 0;
    for(count = 0; count < CYCLES; count++) {
        uint64_t start = __builtin_ia32_rdtsc();
        float t = intersection_dist_ray3_sphere(&r1, &s1);
        uint64_t end = __builtin_ia32_rdtsc();
        total += end - start;
    }
    printf("\t= passed\tCPU Cycles: %d \n", total/count);
}

void test_intersection_dist_ray3_triangle(){
    printf("intersection_dist_ray3_triangle(x)");

    const triangle_t t1 = (triangle_t) {
        .vertices[0] = (pos3_t) {1,1,5},
        .vertices[1] = (pos3_t) {1,-1,5},
        .vertices[2] = (pos3_t) {-1,1,5}
    };
    const ray3_t r1 = (ray3_t) {
        .direction = (vec3_t) {0,0,1},
        .origin    = (pos3_t) {0,0,0}
    };
    validate_result(5.0f, intersection_dist_ray3_triangle(&r1, &t1), " test 1");
    
    uint64_t total, count;
    total = 0;
    for(count = 0; count < CYCLES; count++) {
        uint64_t start = __builtin_ia32_rdtsc();
        float result = intersection_dist_ray3_triangle(&r1, &t1);
        uint64_t end = __builtin_ia32_rdtsc();
        total += end - start;
    }
    printf("\t= passed\tCPU Cycles: %d \n", total/count);
}

void test_pos3_along_ray3() {
    printf("pos3_along_ray3(x)");
    const sphere_t s1 = (sphere_t) {
        .position = (pos3_t) { 0, 0, 2 },
        .radius = 1.0f
    };
    const ray3_t r1 = (ray3_t) {
        .origin = (pos3_t) { 0,0,0 },
        .direction = (vec3_t) {0,0,1}
    };
    const sphere_t s2 = (sphere_t) {
        .position = (pos3_t) { 2, 2, 2 },
        .radius = 1.0f
    };
    ray3_t r2 = (ray3_t) {
        .origin = (pos3_t) { 0,0,0 },
        .direction = (vec3_t) {1,1,1 }
    };
    validate_vec_result((vec3_t) {      0,      0,      1}, pos3_to_vec3(pos3_along_ray3(&r1, intersection_dist_ray3_sphere(&r1, &s1))), " test 1");
    validate_vec_result((vec3_t) { 1.4226, 1.4226, 1.4226}, pos3_to_vec3(pos3_along_ray3(&r2, intersection_dist_ray3_sphere(&r2, &s2))), " test 2");
    r2.direction = vec3_norm(&r2.direction);
    validate_vec_result((vec3_t) { 1.4226, 1.4226, 1.4226}, pos3_to_vec3(pos3_along_ray3(&r2, intersection_dist_ray3_sphere(&r2, &s2))), " test 3");
    
    uint64_t total, count;
    total = 0;
    for(count = 0; count < CYCLES; count++) {
        uint64_t start = __builtin_ia32_rdtsc();
        pos3_t point = pos3_along_ray3(&r1, intersection_dist_ray3_sphere(&r1, &s1));
        uint64_t end = __builtin_ia32_rdtsc();
        total += end - start;
    }
    printf("\t\t\t= passed\tCPU Cycles: %d \n", total/count);
}

void test_normal_sphere() {
    printf("normal_sphere(x)");
    const sphere_t s1 = (sphere_t) {
        .position = (pos3_t) { 0, 0, 0 },
        .radius = 1.0f
    };
    const pos3_t p1 = (pos3_t) { 0, 0, 1 };
    const vec3_t n1 = (vec3_t) { 0, 0, 1 };
    
    validate_vec_result(n1, normal_sphere(&p1, &s1), " test 1");

    uint64_t total, count;
    total = 0;
    for(count = 0; count < CYCLES; count++) {
        uint64_t start = __builtin_ia32_rdtsc();
        vec3_t result = normal_sphere(&p1, &s1);
        uint64_t end = __builtin_ia32_rdtsc();
        total += end - start;
    }
    printf("\t\t\t= passed\tCPU Cycles: %d \n", total/count);
}

void test_normal_triangle() {
    printf("intersection_dist_ray3_triangle(x)");

    const triangle_t t1 = (triangle_t) {
        .vertices[0] = (pos3_t) {1,1,5},
        .vertices[1] = (pos3_t) {1,-1,5},
        .vertices[2] = (pos3_t) {-1,1,5}
    };
    const ray3_t r1 = (ray3_t) {
        .direction = (vec3_t) {0,0,1},
        .origin    = (pos3_t) {0,0,0}
    };
    const vec3_t v1 = (vec3_t) { 0,0,-1};
    
    validate_vec_result(v1, normal_triangle(&r1.direction, &t1), " test 1");
    
    uint64_t total, count;
    total = 0;
    for(count = 0; count < CYCLES; count++) {
        uint64_t start = __builtin_ia32_rdtsc();
        float result = intersection_dist_ray3_triangle(&r1, &t1);
        uint64_t end = __builtin_ia32_rdtsc();
        total += end - start;
    }
    printf("\t= passed\tCPU Cycles: %d \n", total/count);
}