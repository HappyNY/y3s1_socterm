#include <system.h>
#include "mathc.h"

///////////////////////////////////////
// 3D 
///////////////////////////////////////

// @note. x = right, y = up, z = bwd ... right-handed coordinate

#define LCD_WIDTH 400
#define LCD_HEIGHT 240
#define ASPECT_RATIO (((float)LCD_WIDTH)/((float)LCD_HEIGHT))

// To prototyping, only one instance will be drawn
typedef struct swk_mesh
{
    struct vec3* vertices;
    struct vec3* vertices_color;
    uint16_t* indices; // must be 3's multiplicand
    uint16_t num_vertices;
    uint16_t num_indices;

    uint16_t cap_vtx, cap_idx;
} swk_mesh_t;

// mesh generation
void mesh_init(
    swk_mesh_t* const mesh,
    uint16_t cap_vtx,
    uint16_t cap_idx
);

void mesh_subdevide(
    swk_mesh_t* const mesh
);

void mesh_createbox(
    swk_mesh_t* const mesh,
    float size
);

void mesh_destroy(
    swk_mesh_t* const mesh
);

void mesh_create_sphere(
    swk_mesh_t* const mesh,
    float radius,
    uint8_t num_subdivision
);

typedef struct swk_meshinfo
{
    struct swk_mesh* pmesh;
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

// forward decl
struct swk_gppcu;


// generate matrix
void app_calc_object_constant(
    struct swk_object_constant* const pp,
    struct swk_cam const* const cam,
    swk_meshinfo_t const* const mesh
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

void app_run_vertex_shader_async(
    struct swk_gppcu* const gpccu
);

// @warning. dst array size should be at least vertex size!
void app_download_points(
    struct swk_gppcu* const gppcu,
    struct vec3i* const dst, // x, y coord.
    uint32_t capacity
);

// execute
void app_render_on_screen(
    struct vec3i const* const points,
    uint16_t const* const indices,
    uint16_t num_idx 
);


///////////////////////////////////////
// OUTPUT
///////////////////////////////////////
typedef struct swk_scr_desc_rgb16 {
    uint16_t* lpbuffer;
    uint16_t width;
    uint16_t height;
} swk_scr_desc_rgb16_t;

void rgb16_drawline( swk_scr_desc_rgb16_t* const pp, int16_t x0, int16_t y0, const int16_t x1, const int16_t y1 );
