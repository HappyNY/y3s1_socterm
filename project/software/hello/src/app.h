#include <system.h>
#include "mathc.h"

// @note. x = right, y = up, z = bwd ... right-handed coordinate

#define LCD_WIDTH 400
#define LCD_HEIGHT 240
#define ASPECT_RATIO (((float)LCD_WIDTH)/((float)LCD_HEIGHT))

// To prototyping, only one instance will be drawn
typedef struct swk_mesh
{
    struct vec3* vertices;
    uint16_t* indices; // must be 3's multiplicand
    uint16_t num_vertices;
    uint16_t num_indices;
} swk_mesh_t;

typedef struct swk_meshinfo
{
    struct swk_mesh mesh;
    struct vec3 location;
    struct vec3 rotation;
    struct vec3 scale;
} swk_meshinfo_t;

typedef struct swk_cam
{
    struct vec3 location;
    struct vec3 rotation;
    float fov;
    float near;
    float far;
} swk_cam_t;

typedef struct swk_object_constant
{
    mfloat_t world_view_proj[MAT4_SIZE];
    float width;
    float height;
} swk_object_constant_t;

// generate matrix
void app_calc_object_constant(
    struct swk_object_constant* const pp,
    struct swk_cam const* const cam,
    struct swk_meshinfo const* const mesh
);

// initializations
void app_upload_program(
    struct swk_gppcu* const gppcu
);

// upload
void app_upload_vertices(
    struct swk_gppcu* const gppcu,
    struct swk_mesh const* const mesh
);

void app_upload_object_constant(
    struct swk_gppcu* const gppcu,
    struct swk_object_constant const* const obj_constant
);

void app_download_points(
    struct swk_gppcu* const gppcu,
    struct vec3i* const dst, // x, y coord.
    uint32_t capacity
);

// execute
void app_render_on_screen(
    struct vec2i const* const points,
    uint16_t const* const indices,
    uint16_t* const lcdbuff_addr
);

