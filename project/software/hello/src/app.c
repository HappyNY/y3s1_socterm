#include <system.h>
#include "app.h"
#include <stdlib.h>
#include <string.h>
#include "gppcu.h"
#include "gpccu_assembly_macros.h"

mfloat_t* mat4_rotate_zyx( mfloat_t* result, mfloat_t* vrot )
{ 
    mfloat_t mtrx[MAT4_SIZE], tmp[MAT4_SIZE];
    mat4_identity( mtrx );
    mat4_identity( tmp );
    mat4_multiply( mtrx, mtrx, mat4_rotation_z( tmp, vrot[2] ) );
    mat4_identity( tmp );
    mat4_multiply( mtrx, mtrx, mat4_rotation_y( tmp, vrot[1] ) );
    mat4_identity( tmp );
    mat4_multiply( mtrx, mtrx, mat4_rotation_x( tmp, vrot[0] ) );
    mat4_assign( result, mtrx );
    return result;
}

void app_calc_object_constant( struct swk_object_constant* const pp, struct swk_cam const* const cam, struct swk_meshinfo const* const mesh )
{
    mfloat_t world[MAT4_SIZE], tmp[MAT4_SIZE];

    // create world matrix
    mat4_identity( world );
    mat4_scale( world, world, &mesh->scale );
    mat4_multiply( world, world, mat4_rotate_zyx( tmp, &mesh->rotation ) );
    mat4_translate( world, world, &mesh->location );

    // create view matrix
    mfloat_t lookat[MAT4_SIZE];
    mfloat_t v_dir[VEC4_SIZE];
    mfloat_t v_target[VEC3_SIZE];
    mfloat_t v_up[VEC3_SIZE];

    mfloat_t v_camloc[VEC3_SIZE];
    vec3_assign( v_camloc, &cam->location );

    vec4( v_dir, 0.f, 0.f, -10.0f, 0.f );
    vec3( v_up, 0.f, 1.0f, 0.f );

    mat4_rotate_zyx( tmp, &cam->rotation );
    vec4_multiply_mat4( v_dir, v_dir, tmp );
    vec3_add( v_target, v_dir, v_camloc );
    mat4_look_at( lookat, v_camloc, v_target, v_up );
    
    // create perspective
    mfloat_t perspective[MAT4_SIZE];
    mat4_perspective_fov( perspective, to_radians( cam->fov ), LCD_WIDTH, LCD_HEIGHT, cam->near, cam->far );

    // result
    mat4_multiply( pp->world_view_proj, world, lookat );
    mat4_multiply( pp->world_view_proj, pp->world_view_proj, perspective );
    pp->width = LCD_WIDTH;
    pp->height = LCD_HEIGHT;
}

enum
{
    // constant ofst
    COFST_FLOAT_ONE = 1,
    COFST_MAT4_WORLD_VIEW_PROJ = 16,
    COFST_WIDTH,
    COFST_HEIGHT,

    // local memory offset ... task space
    SIZE_VEC3 = 3,
    OFST_VEC3_VTEX_INPUT = 0,
    OFST_VECI3_VTEX_OUTPUT = OFST_VEC3_VTEX_INPUT + SIZE_VEC3, 
    OFST_TABLE = OFST_VECI3_VTEX_OUTPUT + SIZE_VEC3,

    TASK_SPACE = 12
};

void app_upload_program( struct swk_gppcu* const pp )
{
    gppcu_clear_instr( pp );

    // Program definition
    enum {
        rx,
        ry,
        rz,
        rw, // STORES FLOAT ONE
        acc,
        calc,
        loader,

        ra = acc, rb, rc, rd
        // REGPIVOT = 9
    };
    gp_set_gppcu_ptr( pp );

    // -- initialization
    // ldl rx, [vec + 0]
    // ldl ry, [vec + 1]
    // ldl rz, [vec + 2]
    // ldci rw, <float_one>
    gp_ldl( rx, REGPIVOT, OFST_VEC3_VTEX_INPUT + 0 );
    gp_ldl( ry, REGPIVOT, OFST_VEC3_VTEX_INPUT + 1 );
    gp_ldl( rz, REGPIVOT, OFST_VEC3_VTEX_INPUT + 2 );
    gp_ldci( rw, COFST_FLOAT_ONE );

    // -- iteration
    // mvi acc, 0
    // ldl ldr, [mtrx + 0 + 0] ; [ row  + column ]
    // fmul plf, rx, ldr
    // fadd acc, plf
    // ldl ldr, [mtrx + 4 + 0]
    // fmul plf, ry, ldr
    // fadd acc, plf
    // .... repeat for all !
    // stl acc, [vout + 0]
    // ...
    
    // calc row 0, col 0
    int row, col;
    // Multiply matrix
    for ( col = 0; col < 3; ++col ) {
        gp_mvi( acc, 0 );
        for ( row = 0; row < 4; row++ ) {
            gp_ldci( loader, COFST_MAT4_WORLD_VIEW_PROJ + row * 4 + col );
            gp_fmul( calc, rx + row, loader );
            gp_fadd( acc, acc, calc );
        }
        gp_stl( acc, REGPIVOT, OFST_TABLE + col );
    }
     
    // -- Divide by z
    gp_ldl( rx, REGPIVOT, OFST_TABLE + 0 );
    gp_ldl( ry, REGPIVOT, OFST_TABLE + 1 );
    gp_ldl( rz, REGPIVOT, OFST_TABLE + 2 );

    gp_fdiv( rx, rx, rz );
    gp_fdiv( ry, ry, rz );

    // multiply width, height
    gp_ldci( ra, COFST_WIDTH );
    gp_ldci( rb, COFST_HEIGHT );
    
    gp_fmul( rx, rx, ra );
    gp_fmul( ry, ry, rb );

    // to integer
    gp_ftoi( rx, rx );
    gp_ftoi( ry, ry );

    // -- output result
    gp_stl( rx, REGPIVOT, OFST_VECI3_VTEX_OUTPUT + 0 );
    gp_stl( ry, REGPIVOT, OFST_VECI3_VTEX_OUTPUT + 1 );
    gp_stl( rz, REGPIVOT, OFST_VECI3_VTEX_OUTPUT + 2 );

    gppcu_program_autofeed_device_parallel( pp );
}

void app_upload_vertices(
    struct swk_gppcu* const gppcu,
    struct swk_mesh const* const mesh 
)
{
    // Resize task for each vertices
    gppcu_init_task(
        gppcu,
        TASK_SPACE,
        mesh->num_vertices
    );

    // write data
    gppcu_write(
        gppcu,
        (swk_gppcu_data_t*) mesh->vertices,
        SIZE_VEC3,
        OFST_VEC3_VTEX_INPUT
    );
}

void app_upload_object_constant( struct swk_gppcu* const gppcu, struct swk_object_constant const* const obj_constant )
{
    // push world_view_proj matrix, width, height
    gppcu_write_const(
        &gppcu,
        obj_constant->world_view_proj,
        COFST_MAT4_WORLD_VIEW_PROJ,
        18
    );
}

void app_run_vertex_shader_async( struct swk_gppcu* const gppcu )
{
    gppcu_run_autofeed_device( gppcu );
}

void app_download_points( struct swk_gppcu* const gppcu, struct vec3i* const dst, uint32_t capacity )
{
    gppcu_read(
        gppcu,
        dst,
        capacity,
        3,
        OFST_VECI3_VTEX_OUTPUT
    );
}

void app_render_on_screen( struct vec3i const* const points, uint16_t const* const indices, uint16_t* const lcdbuff_addr )
{

}

void rgb16_drawline( swk_scr_desc_rgb16* const pp, int16_t x0, int16_t y0, const int16_t x1, const int16_t y1 )
{
    int16_t dx = abs( x1 - x0 );
    int16_t dy = -abs( y1 - y0 );
    int sx = x1 > x0 ? 1 : -1;
    int sy = y1 > y0 ? 1 : -1;
    int32_t err = dx + dy, e2;

    while ( 1 )
    {
        // @todo. draw dot to buffer

        if ( x0 == x1 && y0 == y1 ) { break; }
        e2 = err * 2;
        if ( e2 >= dy )
        {
            err += dy;
            x0 += sx;
        }
        if ( e2 <= dx )
        {
            err += dx;
            y0 += sy;
        }
    }
}
