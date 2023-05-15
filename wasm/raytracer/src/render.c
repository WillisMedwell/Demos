#include "render.h"
// For depth buffer
#define HEIGHT_MAX 1920
#define WIDTH_MAX  1080
// For the scene
#define SPHERE_MAX   10
#define TRIANGLE_MAX 10
#define LIGHT_MAX    10

#define NULL 0

float img_depth_data[HEIGHT_MAX*WIDTH_MAX];

typedef enum {
    DIFFUSE,
    GLASS,
    MIRROR
} material_t;
typedef struct {
    pos3_t point;
    vec3_t normal;
    float  dist;
    void*  shape;
    material_t material;
} scene_hit_t;
typedef struct {
    pos3_t position;
    float intensity;
} light_t;
typedef struct {
    uint32_t sphere_cnt;
    sphere_t spheres[SPHERE_MAX];

    uint32_t triangle_cnt;
    triangle_t triangles[TRIANGLE_MAX];

    uint32_t light_cnt;
    light_t lights[LIGHT_MAX];

    material_t sphere_material[SPHERE_MAX];
    material_t triangle_material[TRIANGLE_MAX];
    uint8_t light_material[LIGHT_MAX];
} scene_t;
const static scene_t scene = {
    .spheres[0] = {
        .position = (pos3_t){ 0,1,5 },
        .radius = 1.0f,
    },
    .spheres[1] = {
        .position = (pos3_t){ 2,0.5,3 },
        .radius = 0.5f,
    },
    .spheres[2] = {
        .position = (pos3_t){ -2,0,5 },
        .radius = 0.5f,
    },
    .spheres[3] = {
        .position = (pos3_t){ 0,0.25,2 },
        .radius = 0.25f,
    },
    .sphere_cnt = 4,

    .triangles[0] = {
        .vertices[0] = (pos3_t) {  10, 0, 6 },
        .vertices[1] = (pos3_t) { -10, 0, 6 },
        .vertices[2] = (pos3_t) {  10, 0, 0 }
    },
    .triangles[1] = {
        .vertices[0] = (pos3_t) { -10, 0, 6 },
        .vertices[1] = (pos3_t) { -10, 0, 0 },
        .vertices[2] = (pos3_t) {  10, 0, 0 }
    },
    .triangle_cnt = 2,

    .lights[0] = {
        .position = (pos3_t){ -5,20,10 },
        .intensity = 1.0f
    },
    .light_cnt = 1
};

// Basic colours
static const rgba_t RED        = (rgba_t){ 255,   0,   0, 255 };
static const rgba_t GREEN      = (rgba_t){   0, 255,   0, 255 };
static const rgba_t BLUE       = (rgba_t){   0,   0, 255, 255 };
static const rgba_t BLACK      = (rgba_t){   0,   0,   0, 255 };
static const rgba_t BACKGROUND = (rgba_t){ 255, 255, 255, 255 };

scene_hit_t get_scene_hit(const ray3_t* ray, void* invalid_shape);
ray3_t get_camera_ray(const viewport_t* viewport, uint16_t x, uint16_t y);

void render_intersection(rgba_t* img, const viewport_t* viewport) {
    // For the specified viewport, determine the rgba value.
    for(uint16_t y = viewport->y_start; y < viewport->y_end; ++y) {
        for(uint16_t x = 0; x < viewport->x_max; ++x) {
            uint64_t img_index = (y - viewport->y_start)*viewport->x_max + x;
            ray3_t ray = get_camera_ray(viewport, x, y);
            float t_sphere = NO_HIT;
            float t_triangle = NO_HIT;
            for(uint32_t i = 0; i < scene.sphere_cnt; ++i) {
                float t = intersection_dist_ray3_sphere(&ray, &scene.spheres[i]);
                if(t != NO_HIT && t < t_sphere) {
                    t_sphere = t;
                }
            }
            for(uint32_t i = 0; i < scene.triangle_cnt; ++i) {
                float t = intersection_dist_ray3_triangle(&ray, &scene.triangles[i]);
                if(t != NO_HIT && t < t_triangle) {
                    t_triangle = t;
                }
            }
            if(t_sphere == NO_HIT && t_triangle == NO_HIT) {
                img[img_index] = BLUE;
            }
            else if(t_sphere < t_triangle) {
                img[img_index] = RED;
            }
            else if( t_sphere > t_triangle) {
                img[img_index] = GREEN;
            }
            else {
                img[img_index] = BLACK;
            }
        }
    }
    return;
}
void render_depth(rgba_t* img, const viewport_t* viewport) {
    for(uint16_t y = 0; y < viewport->y_max; ++y) {
        for(uint16_t x = 0; x < viewport->x_max; ++x) {
            uint64_t img_index = y*viewport->x_max + x;
            ray3_t ray = get_camera_ray(viewport, x, y);

            // Find smallest intersection between spheres.
            float t = NO_HIT;
            for(uint32_t i = 0; i<scene.sphere_cnt; i++) {
                t = wmin(t,  intersection_dist_ray3_sphere(&ray, &scene.spheres[i]));
            }
            for(uint32_t i = 0; i<scene.triangle_cnt; i++) {
                t = wmin(t, intersection_dist_ray3_triangle(&ray, &scene.triangles[i]));
            }

            if( t != NO_HIT) {
                img_depth_data[img_index] = t;
            } else {
                img_depth_data[img_index] = -1;
            }
        }
    }
    float max = 0;
    float min = NO_HIT;
    for(uint64_t i = 0; i < viewport->y_max*viewport->x_max; i++ ) {
        max = wmax(img_depth_data[i], max);
        if(img_depth_data[i] > 0) {
            min = wmin(img_depth_data[i], min);
        }
    }
    max -= min;
    for(uint64_t i = 0; i < viewport->y_max*viewport->x_max; i++ ) {
        uint8_t c;
        if(img_depth_data[i] > 0) {
            c = (uint8_t)wabs(((img_depth_data[i]-min) / max * 255) -255);
        }
        else {
            c = 0;
        }
        rgba_t colour = (rgba_t) { c, c, c, 255 };
        img[i] = colour;
    }
    return;
}
void render_basic(rgba_t* img, const viewport_t* viewport) {
    for(uint16_t y = viewport->y_start; y < viewport->y_end; ++y) {
        for(uint16_t x = 0; x < viewport->x_max; ++x) {
            uint64_t img_index = (y - viewport->y_start)*viewport->x_max + x;
            ray3_t ray = get_camera_ray(viewport, x, y);
            scene_hit_t hit = get_scene_hit(&ray, 0);
            if(hit.dist != NO_HIT) {
                // construct rays towards lights. (scene.light_cnt)
                for(uint32_t i = 0; i < 1; ++i) {
                    ray3_t ray_towards_light = (ray3_t) {
                        .direction = vec3_sub(((vec3_t*)&scene.lights[i]), ((vec3_t*)&hit.point)),
                        .origin = hit.point
                    };
                    scene_hit_t new_hit = get_scene_hit(&ray_towards_light, hit.shape);
                    if(new_hit.dist == NO_HIT) {
                        img[img_index] = RED;
                    }
                    else {
                        img[img_index] = BLACK;  
                    }
                }
            }
            else {
                img[img_index] = BACKGROUND;
            }
        }
    }
    return;
}
void render_lambert(rgba_t* img, const viewport_t* viewport) {
    for(uint16_t y = viewport->y_start; y < viewport->y_end; ++y) {
        for(uint16_t x = 0; x < viewport->x_max; ++x) {
            uint64_t img_index = (y - viewport->y_start)*viewport->x_max + x;
            ray3_t ray = get_camera_ray(viewport, x, y);
            scene_hit_t hit = get_scene_hit(&ray, NULL);

            if(hit.dist != NO_HIT) {
                // construct rays towards lights. (scene.light_cnt)
                for(uint32_t i = 0; i < 1; ++i) {
                    ray3_t ray_towards_light = (ray3_t) {
                        .direction = vec3_sub(((vec3_t*)&scene.lights[i]), ((vec3_t*)&hit.point)),
                        .origin = hit.point
                    };
                    scene_hit_t new_hit = get_scene_hit(&ray_towards_light, hit.shape);
                    if(new_hit.dist == NO_HIT) {
                        img[img_index] = RED;
                    }
                    else {
                        img[img_index] = BLACK;  
                    }
                }
            }
            else {
                img[img_index] = BACKGROUND;
            }
        }
    }
    return;
}

inline ray3_t get_camera_ray(const viewport_t* viewport, uint16_t x, uint16_t y) {
    // Produce normalised ray camera ray for given x & y.
    ray3_t ray = (ray3_t) {
        (vec3_t) { (((float)x/viewport->x_max) - 0.5f)*viewport->aspect_ratio, 0.5f - ((float)y/viewport->y_max), viewport->z },
        (pos3_t) { 0.0f, 1.0f, 0.0f }
    };
    ray.direction = vec3_norm(&ray.direction);
    return ray;
}
inline scene_hit_t get_scene_hit(const ray3_t* ray, void* invalid_shape) {
    float t_sphere   = NO_HIT;
    float t_triangle = NO_HIT;
    uint32_t hit_sphere_index   = 0;
    uint32_t hit_triangle_index = 0;

    for(uint32_t i = 0; i < scene.sphere_cnt; ++i) {
        if(invalid_shape != (void*)&scene.spheres[i]) {
            float t = intersection_dist_ray3_sphere(ray, &scene.spheres[i]);
            if(t != NO_HIT && t < t_sphere) {
                t_sphere = t;
                hit_sphere_index = i;
            }
        }
        else {
            // travel slightly along ray to check for second intersection, blocking the way
            ray3_t r = *ray;
            r.origin = pos3_along_ray3(ray, 0.0001f);
            float t = intersection_dist_ray3_sphere(&r, &scene.spheres[i]);
            if(t != NO_HIT && t < t_sphere) {
                t_sphere = t;
                hit_sphere_index = i;
            }
        }
    }
    for(uint32_t i = 0; i < scene.triangle_cnt; ++i) {
        if(invalid_shape != (void*)&scene.triangles[i]) {
            float t = intersection_dist_ray3_triangle(ray, &scene.triangles[i]);
            if(t != NO_HIT && t < t_triangle) {
                t_triangle = t;
                hit_triangle_index = i;
            }
        }
    }

    scene_hit_t hit;
    if(t_sphere == NO_HIT && t_triangle == NO_HIT){
        hit.dist = NO_HIT;
    }
    else if(t_sphere < t_triangle) {
        hit.dist   = t_sphere;
        hit.point  = pos3_along_ray3(ray, t_sphere);
        hit.normal = normal_sphere(&hit.point, &scene.spheres[hit_sphere_index]);
        hit.material = scene.sphere_material[hit_sphere_index];
        hit.shape = (void*)&scene.spheres[hit_sphere_index];
    }
    else{
        hit.dist   = t_triangle;
        hit.point  = pos3_along_ray3(ray, t_triangle);
        hit.normal = normal_triangle(&ray->direction, &scene.triangles[hit_triangle_index]);
        hit.material = scene.triangle_material[hit_triangle_index];
        hit.shape = (void*)&scene.triangles[hit_triangle_index];
    }
    return hit;
}




