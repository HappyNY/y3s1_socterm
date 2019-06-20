#include <system.h>
#include "mathc.h"

// @note. x = right, y = up, z = fwd

// To prototyping, only one instance will be drawn
struct swk_mesh
{
    struct vec3* vertices;
    struct vec3i* indices;
    uint16_t num_vertices;
    uint16_t num_indices;
};

struct swk_meshinfo
{
    struct swk_mesh mesh;
    struct vec3 location;
    struct vec3 rotation;
    struct vec3 scale;
};

struct swk_cam
{
    struct vec3 location;
    struct vec3 rotation;
};

struct swk_object_constant
{
    struct mat4 worldmat;
};

// generate matrix