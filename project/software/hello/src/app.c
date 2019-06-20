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

void mesh_init( swk_mesh_t* const mesh, uint16_t cap_vtx, uint16_t cap_idx )
{
    mesh->cap_vtx = cap_vtx;
    mesh->cap_idx = cap_idx;

    mesh->num_indices = 0;
    mesh->num_vertices = 0;

    mesh->vertices = malloc( sizeof( struct vec3 ) * cap_vtx );
    mesh->indices = malloc( sizeof( uint16_t ) * cap_idx );
}

void mesh_subdevide( swk_mesh_t* const mesh )
{
    // for each polygons...
    int idx, initial_max = mesh->num_indices;
    uint16_t* pidx_head = &mesh->num_indices;
    uint16_t* vtex_idx = &mesh->num_vertices;

    for ( idx = 0; idx < initial_max - 2; idx += 3 )
    {
        //      2
        //   4     6
        // 1    5    3 
        uint16_t* pi = &mesh->indices[idx];
        uint16_t* pn = &mesh->indices[*pidx_head];

        uint16_t
            i1 = pi[0],
            i2 = pi[1],
            i3 = pi[2];

        struct vec3
            v1 = mesh->vertices[i1],
            v2 = mesh->vertices[i2],
            v3 = mesh->vertices[i3];

        struct vec3 v4, v5, v6;
        vec3_lerp( &v4, &v1, &v2, 0.5f );
        vec3_lerp( &v6, &v2, &v3, 0.5f );
        vec3_lerp( &v5, &v3, &v1, 0.5f );

        uint16_t i4, i5, i6;
        i4 = vtex_idx[0]++;
        i5 = vtex_idx[0]++;
        i6 = vtex_idx[0]++;

        mesh->vertices[i4] = v4;
        mesh->vertices[i5] = v5;
        mesh->vertices[i6] = v6;
        
        pi[0] = i5;
        pi[1] = i4;
        pi[2] = i6;

        pn[0] = i1;
        pn[1] = i4;
        pn[2] = i5;

        pn[3] = i4;
        pn[4] = i2;
        pn[5] = i6;
        
        pn[6] = i6;
        pn[7] = i3;
        pn[8] = i5;

        // Three polygons added
        *pidx_head += 9;
    }
}

void mesh_createbox( swk_mesh_t* const mesh, float size )
{ 
    float vertices[][3] = {
        {-size, -size, -size},
        {-size, size, -size},
        {size, size, -size},
        {size, -size, -size},
        {-size, -size, size},
        {-size, size, size},
        {size, size, size},
        {size, -size, size}
    };

    uint16_t indices[] = {
        0, 1, 2,
        0, 2, 3,

        4, 6, 5,
        4, 7, 6,

        4, 5, 1,
        4, 1, 0,

        3, 2, 6,
        3, 6, 7,

        1, 5, 6,
        1, 6, 2,

        4, 0, 3,
        4, 3, 7,
    }; 

    passert( mesh->cap_vtx > countof( vertices ), "Memory overflow" );
    passert( mesh->cap_idx > countof( indices ), "Memory overflow" );

    memcpy( mesh->vertices, vertices, sizeof( vertices ) );
    memcpy( mesh->indices, indices, sizeof( indices ) );

    mesh->num_vertices = countof( vertices );
    mesh->num_indices = countof( indices );
}

void mesh_destroy( swk_mesh_t* const mesh )
{
    free( mesh->vertices );
    free( mesh->indices );
}

void mesh_create_sphere( swk_mesh_t* const mesh, float radius, uint8_t num_subdivision )
{
    
}

void app_calc_object_constant( struct swk_object_constant* const pp, struct swk_cam const* const cam, swk_meshinfo_t const* const info )
{
    mfloat_t world[MAT4_SIZE], tmp[MAT4_SIZE];

    // create world matrix
    mat4_identity( world );
    mat4_scale( world, world, &info->scale );
    mat4_multiply( world, world, mat4_rotate_zyx( tmp, &info->rotation ) );
    mat4_translate( world, world, &info->location );

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
    mat4_perspective( perspective, to_radians( cam->fov ), LCD_WIDTH/(float)LCD_HEIGHT, cam->near, cam->far );

    // result
    mat4_multiply( pp->world_view_proj, world, lookat );
    // mat4_multiply( pp->world_view_proj, pp->world_view_proj, perspective );
    pp->width = LCD_WIDTH;
    pp->height = LCD_HEIGHT;
}

enum
{
    // constant ofst
    COFST_FLOAT_ONE = 1,
    COFST_WIDTH = 14,
    COFST_HEIGHT = 15,
    COFST_MAT4_WORLD_VIEW_PROJ = 16,

    // local memory offset ... task space 
    OFST_VEC3_VTEX_INPUT = 0,
    OFST_VECI4_VTEX_OUTPUT = OFST_VEC3_VTEX_INPUT + VEC3_SIZE, 
    OFST_TABLE = OFST_VECI4_VTEX_OUTPUT + VEC4_SIZE,

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

        ra = acc, rb, rc, rd, re,
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
    for ( col = 0; col < 4; ++col ) {
        gp_mvi( acc, 0 );
        for ( row = 0; row < 4; row++ ) {
            gp_ldci( loader, COFST_MAT4_WORLD_VIEW_PROJ + col + row * 4 );
            gp_fmul( calc, rx + row, loader );
            gp_adi( loader, acc, 0 );
            gp_fadd( acc, loader, calc );
        }
        gp_stl( acc, REGPIVOT, OFST_TABLE + col );
    }
     
    // -- Divide by z
    gp_ldl( rx, REGPIVOT, OFST_TABLE + 0 );
    gp_ldl( ry, REGPIVOT, OFST_TABLE + 1 );
    gp_ldl( rz, REGPIVOT, OFST_TABLE + 2 );
    gp_ldl( rw, REGPIVOT, OFST_TABLE + 3 );

    gp_stl( rw, REGPIVOT, OFST_VECI4_VTEX_OUTPUT + 3 );

    gp_fdiv( ra, rx, rw );
    gp_stl( ra, REGPIVOT, OFST_VECI4_VTEX_OUTPUT + 0 );
    //gp_ldci( rb, COFST_WIDTH );
    //gp_fmul( rc, ra, rb );
    //gp_ftoi( ra, rc );
    //gp_stl( rx, REGPIVOT, OFST_VECI4_VTEX_OUTPUT + 0 );

    gp_fdiv( ra, ry, rw );
    gp_stl( ra, REGPIVOT, OFST_VECI4_VTEX_OUTPUT + 1 );
    //gp_ldci( rb, COFST_HEIGHT );
    //gp_fmul( rc, ra, rb );
    //gp_ftoi( ra, rc );
    //gp_stl( ry, REGPIVOT, OFST_VECI4_VTEX_OUTPUT + 1 );

    gp_fdiv( ra, rz, rw );
    gp_stl( ra, REGPIVOT, OFST_VECI4_VTEX_OUTPUT + 2 );
    gp_ldci( rb, COFST_HEIGHT );
    gp_fmul( rz, ra, rb );
    //gp_stl( rz, REGPIVOT, OFST_VECI4_VTEX_OUTPUT + 1 ); 

    gppcu_program_autofeed_device_parallel( pp );
}

void app_upload_vertices(
    struct swk_gppcu* const gppcu,
    struct swk_mesh const* const mesh 
)
{
    // Resize task for each vertices
    gppcu_init_task_parallel(
        gppcu,
        TASK_SPACE,
        mesh->num_vertices
    );

    // write data
    gppcu_write(
        gppcu,
        (swk_gppcu_data_t*) mesh->vertices,
        VEC3_SIZE,
        OFST_VEC3_VTEX_INPUT
    );
}

void app_upload_object_constant( struct swk_gppcu* const gppcu, struct swk_object_constant const* const obj_constant )
{
    // push world_view_proj matrix, width, height
    gppcu_write_const(
        gppcu,
        (swk_object_constant_t*) obj_constant->world_view_proj,
        COFST_MAT4_WORLD_VIEW_PROJ,
        16
    ); 
    gppcu_write_const(
        gppcu,
        &obj_constant->width,
        COFST_WIDTH,
        1
    );
    gppcu_write_const(
        gppcu,
        &obj_constant->height,
        COFST_HEIGHT,
        1
    );
     
    float val = 1.0f; 
    gppcu_write_const(
        gppcu,
        &val,
        COFST_FLOAT_ONE,
        1
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
        OFST_VECI4_VTEX_OUTPUT
    );
}

void app_render_on_screen( struct vec3i const* const points, uint16_t const* const indices, uint16_t num_idx )
{
    static uint16_t buff[400 * 240];
    memset( buff, 0, sizeof( buff ) );

    uint16_t const* idx_head = indices;
    uint16_t const* const idx_end = idx_head + num_idx;

    swk_scr_desc_rgb16_t scrdesc;
    scrdesc.height = 240;
    scrdesc.width = 400;
    scrdesc.lpbuffer = (uint16_t*) buff;

    while ( idx_head + 2 < idx_end )
    {
        // draw polygon.
        uint16_t idx0, idx1, idx2;
        idx0 = idx_head[0];
        idx1 = idx_head[1];
        idx2 = idx_head[2];
        rgb16_drawline( &scrdesc, points[idx0].x, points[idx0].y, points[idx1].x, points[idx1].y );
        rgb16_drawline( &scrdesc, points[idx1].x, points[idx1].y, points[idx2].x, points[idx2].y );
        rgb16_drawline( &scrdesc, points[idx2].x, points[idx2].y, points[idx0].x, points[idx0].y );

        printf( "Drawing polygons ... %d, [%d, %d, %d]\n", ( idx_end - idx_head ) / 3, idx0, idx1, idx2 );
        idx_head += 3;
    }

    memcpy( (void*) BUFFERRAM_OUT_BASE, buff, sizeof( buff ) );
}

void rgb16_drawline( swk_scr_desc_rgb16_t* const pp, int16_t x0, int16_t y0, const int16_t x1, const int16_t y1 )
{
    printf( "Rendering ptr .. %d, %d -> %d, %d\n", x0, y0, x1, y1 );

    int16_t dx = abs( x1 - x0 );
    int16_t dy = -abs( y1 - y0 );
    int sx = x1 > x0 ? 1 : -1;
    int sy = y1 > y0 ? 1 : -1;
    int32_t err = dx + dy, e2;

    while ( 1 )
    {
        // @todo. draw dot to buffer
        if ( x0 >= 0 && x0 < pp->width
             && y0 >= 0 && y0 < pp->height )
        {
            pp->lpbuffer[y0 * pp->width + x0] = 0xffff;
        }

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
