#include <system.h>
#include "app.h"
#include <stdlib.h>
#include <string.h>

mfloat_t* mat4_rotate_zyx( mfloat_t* result, mfloat_t* vrot )
{ 
    mfloat_t mtrx[MAT4_SIZE], tmp[MAT4_SIZE];
    mat4_identity( mtrx );
    mat4_multiply( mtrx, mtrx, mat4_rotation_z( tmp, vrot[2] ) );
    mat4_multiply( mtrx, mtrx, mat4_rotation_y( tmp, vrot[1] ) );
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
    memcpy( v_camloc, &cam->location, sizeof( float ) * 3 ); 

    vec4( v_dir, 0.f, 0.f, -10.0f, 0.f );
    vec3( v_up, 0.f, 1.0f, 0.f );

    mat4_rotate_zyx( tmp, &cam->rotation );
    vec4_multiply_mat4( v_dir, v_dir, tmp );
    vec3_add( v_target, v_dir, v_camloc );
    mat4_look_at( lookat, v_camloc, v_target, v_up );
    
    // create perspective
    mfloat_t perspective[MAT4_SIZE];
    mat4_perspective_fov( perspective, cam->fov, LCD_WIDTH, LCD_HEIGHT, cam->near, cam->far );

    // result
    mat4_multiply( pp->worldmat, mat4_multiply( pp->worldmat, world, lookat ), perspective );
    pp->width = LCD_WIDTH;
    pp->height = LCD_HEIGHT;
}
