#include <system.h>
#include <stdio.h>
#include "gppcu.h"
#include <altera_avalon_pio_regs.h>
#include <nios2.h>
#include <string.h>
#include "app.h"
#include <stdlib.h> 
#include <stdarg.h>
#include "gpccu_assembly_macros.h"

#define BOOL int
#define TRUE 1
#define FALSE 0

void wait(int val)
{
    volatile int i;
    for ( i = 0; i < val; ++i )
	{
		// nop;
	}
}

BOOL display_stat_is_done()
{
    bool prun, pdone;
    uint8_t szpertask, popmemend, popmemhead, ponumcycles, pocurcycleidx;
    gppcu_stat( &prun, &pdone, &szpertask, &popmemend, &popmemhead, &ponumcycles, &pocurcycleidx );
    printf(
        "is running ? %d\n"
        "is done ? %d\n"
        "cur mem ofst: %d \n"
        "mem: %d/%d \n"
        "cycle: %d/%d \n",
        prun, pdone, szpertask, popmemhead, popmemend, pocurcycleidx, ponumcycles
    );
    return pdone;
}

void mem_clr( int max_rot, int max_iter )
{
    for ( int i = 0; i < max_rot; ++i )
    {
        for ( int j = 0; j < max_iter; ++j )
        {
            gppcu_data_wr_slow( i, j, 0 );
        }
    }
}

void monitor( int num_thr, int begin, int num_mem )
{ 
    for ( int rot = 0; rot < num_thr; ++rot )
    {
        printf( "for thread %d\n", rot );
        for ( int i = begin; i < begin + num_mem; ++i )
        {
            printf( "%9x", i );
        }
        printf( "\n" );
        for ( int i = begin; i < begin + num_mem; ++i )
        {
            gppcu_data_rd_slow( rot, i );
            printf( "%9x", gppcu_data_rd_slow( rot, i ) );
        }
        printf( "\n" );
    }
}

float reinterpret_float( uint32_t val )
{
    return *(float*) & val;
}

char* spf( char* fmt, ... )
{
    va_list va;
    va_start( va, fmt );
    static char buff[1024];
    vsprintf( buff, fmt, va );
    va_end( va );
    return buff;
}

int main()
{
    printf( "Hell, nios ! \n" );
    mem_clr( 24, 120 );

    swk_gppcu_t gppcu;

    printf( "init ..\n" );
    gppcu_init( &gppcu, 24, 2048, 512, PIO_CMD_BASE, PIO_DATAOUT_BASE, PIO_DATAIN_BASE );

    printf( "upload ..\n" );

    swk_mesh_t mesh;
    mesh_init( &mesh, 2048, 8096 );

    printf( "box create ..\n" );
    mesh_createbox( &mesh, 65 );
    mesh_subdevide( &mesh ); 
    // mesh_subdevide( &mesh ); 
    // mesh_subdevide( &mesh );  
    // mesh.num_indices = 180;

    swk_meshinfo_t mesh_inst;
    mesh_inst.pmesh = &mesh;

    vec3_zero( &mesh_inst.location );
    vec3_zero( &mesh_inst.rotation );
    vec3_one( &mesh_inst.scale );

    mesh_inst.rotation.y = 45.0f;
    mesh_inst.rotation.x = 45.0f;

    swk_cam_t cam;
    cam.far = 250.f;
    cam.near = 1.f;
    cam.fov = 85.0f;
    vec3( &cam.location, 0.f, 0.f, 35.f );
    vec3_zero( &cam.rotation );

    swk_object_constant_t result;

    struct vec3i* output = malloc( sizeof( struct vec3i ) * 2048 );
    app_upload_vertices( &gppcu, &mesh );
    app_upload_program( &gppcu );

    while ( true )
    {
        app_calc_object_constant( &result, &cam, &mesh_inst );

        // printf( "writing constant ..\n" );
        // printf( "uploading vertices..\n" );  

        app_upload_object_constant( &gppcu, &result );

        app_run_vertex_shader_async( &gppcu ); 
        
        // wait( 10 );
        while ( !gppcu_is_done( &gppcu, 0 ) ) { 
            //display_stat_is_done();  
        };

        app_download_points( &gppcu, output, 0xffff );
        // monitor( 8, 0, 24 );

        mesh_inst.rotation.y += 0.016f;
        mesh_inst.rotation.x += 0.005f;
        mesh_inst.rotation.z += 0.003f;

        // verify whether vertex has uploaded successfully
        int i, thr = 0, ofst = 0; 
        int rowcnt = 0;
        char lines[10][512];
        char* ptrs[10];
        if ( 0 ) for ( i = 0; i < mesh.num_vertices; ++i )
        {
            if ( rowcnt++ % 8 == 0 )
            {
                int c;
                if ( rowcnt )
                {
                    for ( c = 0; c < countof( ptrs ); ++c )
                    {
                        printf( lines[c] );
                        printf( "\n" );

                    }
                }

                for ( c = 0; c < countof( ptrs ); ++c )
                {
                    ptrs[c] = lines[c];
                }

            }
            uint32_t v[12];
            int k;
            for ( k = 0; k < 12; ++k )
            {
                v[k] = gppcu_data_rd_slow( thr, ofst + k );
            }

            ptrs[0] += sprintf( ptrs[0], "%-33s", spf( " ---- vertex %d ---- ", i ) );
            ptrs[1] += sprintf( ptrs[1], "%-33s", spf( "rx: %f", reinterpret_float( v[1] ) ) );
            ptrs[2] += sprintf( ptrs[2], "%-33s", spf( "ry: %f", reinterpret_float( v[2] ) ) );
            ptrs[3] += sprintf( ptrs[3], "%-33s", spf( "rz: %f", reinterpret_float( v[3] ) ) );
            ptrs[4] += sprintf( ptrs[4], "%-33s", spf( "xx: %d", v[4] ) );
            ptrs[5] += sprintf( ptrs[5], "%-33s", spf( "yy: %d", v[5] ) );
            ptrs[6] += sprintf( ptrs[6], "%-33s", spf( "mult_x: %f", reinterpret_float( v[8] ) ) );
            ptrs[7] += sprintf( ptrs[7], "%-33s", spf( "mult_y: %f", reinterpret_float( v[9] ) ) );
            ptrs[8] += sprintf( ptrs[8], "%-33s", spf( "mult_z: %f", reinterpret_float( v[10] ) ) );
            ptrs[9] += sprintf( ptrs[9], "%-33s", spf( "mult_w: %f", reinterpret_float( v[11] ) ) );

            if ( ++thr == 24 )
            {
                thr = 0;
                ofst += 12;
            }
        }

        // getchar();

        // What it should be ...
        struct vec3i smples[1024];
        /**//*
        for ( i = 0; i < mesh.num_vertices; ++i )
        {
            mfloat_t smplvtex[4];
            vec3_assign( smplvtex, &mesh.vertices[i] );
            smplvtex[3] = 1.f;
            // printf( "Vector at %f, %f, %f, %f\n", smplvtex[0], smplvtex[1], smplvtex[2], smplvtex[3] );
            vec4_multiply_mat4( smplvtex, smplvtex, result.world_view_proj );
            // printf( "Translate %f, %f, %f, %f\n", smplvtex[0], smplvtex[1], smplvtex[2], smplvtex[3] );
            // vec4_divide_f( smplvtex, smplvtex, smplvtex[3] );
            // printf( "Division  %f, %f, %f, %f\n", smplvtex[0], smplvtex[1], smplvtex[2], smplvtex[3] );
            // smplvtex[0] += 0.5f;
            // smplvtex[1] += 0.5f;
            // smplvtex[0] *= result.width;
            // smplvtex[1] *= result.height; 
            smplvtex[0] += result.width /2;
            smplvtex[1] += result.height/2;
        
            // printf( "Result    %f, %f, %f, %f\n\n", smplvtex[0], smplvtex[1], smplvtex[2], smplvtex[3] ); 
        
            vec3i_assign_vec3( &smples[i], smplvtex ); 
        } //*/

        // app_render_on_screen( smples, mesh.indices, mesh.num_indices );
        app_render_on_screen( output, mesh.indices, mesh.num_indices );
    }
    
    monitor( 4, 0, 8 );
    free( output );
}

int main__cxt()
{
    swk_gppcu_t gppcu;

    gppcu_init( &gppcu, 24, 1024, 512, PIO_CMD_BASE, PIO_DATAOUT_BASE, PIO_DATAIN_BASE );
    gppcu_init_task( &gppcu, 5, 240 );
     
    mem_clr( 12, 24 );

    gppcu_clear_instr( &gppcu );

    gp_set_gppcu_ptr( &gppcu );  

    gp_mvi( 0x5, 3 );
    gp_mvi( 0x6, 4 );
     
    gp_itof( 0x0, 0x5 );
    gp_itof( 0x1, 0x6 );
    
    gp_fmul( 0x2, 0x1, 0x0 );
    gp_fdiv( 0x3, 0x1, 0x0 );
    gp_fsqrt( 0x4, 0x2 );

    gp_stl( 0x0, REGPIVOT, 0 );
    gp_stl( 0x1, REGPIVOT, 1 );
    gp_stl( 0x2, REGPIVOT, 2 );
    gp_stl( 0x3, REGPIVOT, 3 );
    gp_stl( 0x4, REGPIVOT, 4 ); 

    gppcu_program_autofeed_device_parallel( &gppcu );
    gppcu_run_autofeed_device( &gppcu );

    while ( display_stat_is_done() == false )
    {
        // gppcu_program_autofeed_device_parallel( &gppcu );
        // gppcu_run_autofeed_device( &gppcu );

        wait( 1000000 );
    }
    monitor( 12, 0, 20 );

    gppcu_destroy( &gppcu );
    while ( 1 );
}

int maincc()
{
    printf( "Hello from Nios II! ... Launching ... \n" );
     
    const int max_iter = 16;
    const int max_rot = 4;

    swk_gppcu_t gppcu;

    gppcu.MMAP_CMDOUT = PIO_CMD_BASE;
    gppcu.MMAP_DATIN = PIO_DATAIN_BASE;
    gppcu.MMAP_DATOUT = PIO_DATAOUT_BASE;

    DEPRECATED__gppcu_init( &gppcu, 24, 1024, 512 );
    gppcu_init_task( &gppcu, 4, 240 );

    gppcu_clear_instr( &gppcu ); 
    swk_gppcu_data_t initdat[240];
    int i;
    for ( i = 0; i < countof( initdat ); ++i )
    {
        initdat[i] = i;
    }
      
    gp_set_gppcu_ptr( &gppcu ); 
    gp_mov( REG0, REGPIVOT, 0 ); 
    gp_adi( REG1, REG0, 1 ); 
    gp_stl( REG0, REG0, 1 ); 
    gp_stl( REG1, REG0, 2 ); 
    gp_ldci( REG4, 1 ); 
    gp_stl( REG4, REG0, 0 );
    gp_mvi( REG4, 0x3323 );
    gp_stl( REG4, REG0, 3 );

    int v = 0x1234;
    gppcu_write_const( &gppcu, &v, 1, 1 ); 

    while ( true )
    {
        mem_clr( max_rot, max_iter );
        gppcu_write( &gppcu, initdat, 1, 0 );

        // Program & run 
        gppcu_program_autofeed_device_parallel( &gppcu );
        gppcu_run_autofeed_device( &gppcu );

        // Display dat
        wait( 5000000 );
        display_stat_is_done();

        monitor( 4, 0, 21 );
    }
}

int main_vv()
{
    swk_meshinfo_t meshinfo;
    vec3_zero( &meshinfo.location );
    vec3_zero( &meshinfo.rotation );
    vec3_one( &meshinfo.scale );

    swk_cam_t cam;
    cam.far = 250.f;
    cam.near = 1.f;
    cam.fov = 85.0f;
    vec3( &cam.location, 0.f, 0.f, 50.f );
    vec3_zero( &cam.rotation );

    swk_object_constant_t result;

    app_calc_object_constant( &result, &cam, &meshinfo );
    
    mfloat_t smplvtex[VEC4_SIZE];
    
    float x = 0, y = 0, z = 0;

    while ( 1 )
    {
        vec4( smplvtex, x, y, z, 1.f );
        vec4_multiply_mat4( smplvtex, smplvtex, result.world_view_proj );
        printf( "Vector at %f, %f, %f, %f\n", x, y, z, 1.0f );
        printf( "Translate %f, %f, %f, %f\n", smplvtex[0], smplvtex[1], smplvtex[2], smplvtex[3] );
        vec4_divide_f( smplvtex, smplvtex, smplvtex[3] );
        printf( "Division  %f, %f, %f, %f\n", smplvtex[0], smplvtex[1], smplvtex[2], smplvtex[3] );
        smplvtex[0] += 0.5f;
        smplvtex[1] += 0.5f;
        smplvtex[0] *= result.width;
        smplvtex[1] *= result.height;
        printf( "Result    %f, %f, %f, %f\n\n", smplvtex[0], smplvtex[1], smplvtex[2], smplvtex[3] );

        wait( 5000000 );

        x = rand() % 100 - 50;
        y = rand() % 100 - 50;
        z = rand() % 100 - 50;

    }
    return 0;
}

int main__1()
{ 
	printf("Hello from Nios II! ... Launching ... \n");
    

    const int max_iter = 16;
    const int max_rot = 4;

    swk_gppcu_t gppcu;

    gppcu.MMAP_CMDOUT = PIO_CMD_BASE;
    gppcu.MMAP_DATIN = PIO_DATAIN_BASE;
    gppcu.MMAP_DATOUT = PIO_DATAOUT_BASE;

    DEPRECATED__gppcu_init( &gppcu, 24, 1024, 512 );
    gppcu_init_task( &gppcu, 4, 240 );

    gppcu_clear_instr( &gppcu );
	//**//*
    swk_gppcu_data_t initdat[240];
    int i;
    for ( i = 0; i < countof( initdat ); ++i ) 
    {
        initdat[i] = i;
    }

    
    // reg01 should be 2.
    // gppcu_ldci( &gppcu, COND_ALWAYS, REGF, 0 ); 
    gppcu_arith_s( &gppcu, COND_ALWAYS, OPR_S_MOV, false, REG0, REGPIVOT, 0 ); 
    gppcu_arith_b( &gppcu, COND_ALWAYS, OPR_B_ADI, false, REG1, REG0, 1 );  
    gppcu_stl( &gppcu, COND_ALWAYS, REG0, REG0, 1 );  
    gppcu_stl( &gppcu, COND_ALWAYS, REG1, REG0, 2 );  
    gppcu_ldci( &gppcu, COND_ALWAYS, REG4, 1 );
    gppcu_stl( &gppcu, COND_ALWAYS, REG4, REG0, 0 );
    int v = 0x1234;
    gppcu_write_const( &gppcu, &v, 1, 1 );
    // gppcu_arith_s( &gppcu, COND_ALWAYS, OPR_S_MOV, false, REG01, REG00, 0 );  
    // gppcu_arith_a( &gppcu, COND_ALWAYS, OPR_A_ADC, false, REG01, REG01, REG01, 0 ); 
    // gppcu_stl( &gppcu, COND_ALWAYS, REG00, REG00, 1 );
    // gppcu_stl( &gppcu, COND_ALWAYS, REG01, REG00, 2 );
    // gppcu_stl( &gppcu, COND_ALWAYS, REG01, REG00, 1 );
    // gppcu_stl( &gppcu, COND_ALWAYS, REG03, REG00, 2 );
    // gppcu_stl( &gppcu, COND_ALWAYS, REG04, REG00, 3 );
    // gppcu_stl( &gppcu, COND_ALWAYS, REG05, REG00, 4 );
    // gppcu_stl( &gppcu, COND_ALWAYS, REG06, REG00, 5 );

    while ( true )
    {
        mem_clr( max_rot, max_iter );  
        gppcu_write( &gppcu, initdat, 1, 0 ); 

        // Program & run 
        gppcu_program_autofeed_device_parallel( &gppcu ); 
        gppcu_run_autofeed_device( &gppcu );

        // Display dat
        wait( 5000000 );
        display_stat_is_done();

        monitor( 4, 0, 21 );
    }
    /*/
#define INSTR_BUNDLE \
	    GPPCU_ASSEMBLE_INSTRUCTION_C(COND_ALWAYS, OPR_C_MVI, FALSE, 0x1, 0),                 \
    GPPCU_ASSEMBLE_INSTRUCTION_C( COND_ALWAYS, OPR_C_MVI, FALSE, 0x2, 0 ),                   \
        GPPCU_ASSEMBLE_INSTRUCTION_A( COND_ALWAYS, OPR_LDL, FALSE, 0x0, 0, 0, 0x2 ),         \
        GPPCU_ASSEMBLE_INSTRUCTION_A( COND_ALWAYS, OPR_STL, FALSE, 0b0, 0x0, 1, 0x1 ),       \
        GPPCU_ASSEMBLE_INSTRUCTION_A( COND_ALWAYS, OPR_STL, FALSE, 0b0, 0x0, 2, 0x1 ),       \
        GPPCU_ASSEMBLE_INSTRUCTION_A( COND_ALWAYS, OPR_STL, FALSE, 0b0, 0x0, 3, 0x1 ),       \
        GPPCU_ASSEMBLE_INSTRUCTION_A( COND_ALWAYS, OPR_STL, FALSE, 0b0, 0x0, 4, 0x1 ),       \
        GPPCU_ASSEMBLE_INSTRUCTION_A( COND_ALWAYS, OPR_STL, FALSE, 0b0, 0x0, 5, 0x1 ),       \
        GPPCU_ASSEMBLE_INSTRUCTION_A( COND_ALWAYS, OPR_LDL, FALSE, 0x2, 0b0, 5, 0x1 ),       \
        GPPCU_ASSEMBLE_INSTRUCTION_A( COND_ALWAYS, OPR_0_LSL, FALSE, 0x2, 0x2, 0, 0x2 ),     \
        GPPCU_ASSEMBLE_INSTRUCTION_A( COND_ALWAYS, OPR_STL, FALSE, 0b0, 0x2, 6, 0x1 ),
    uint32_t instrs[] = 
    {
        INSTR_BUNDLE
    };
    int cc = 0;
    memcpy( gppcu.marr, instrs, sizeof( instrs ) );
    gppcu.mnum = countof( instrs );
	while(1)
	{ 
		++cc;
		
		printf("--- STEP %d ---\n", cc);
		
		for(int i = 0; i < max_rot; ++i)
		{
			for(int j = 0; j < max_iter; ++j)
			{
				gppcu_data_wr_slow(i, j, j == 0 ? (i << 16) + cc : 0);
			}
		}
		
        DEPRECATED__gppcu_program_autofeed_device( &gppcu );
        while ( !display_stat_is_done() ); 
        wait( 5000000 );
        
        for(int rot = 0; rot < max_rot; ++rot)
        {
            printf("for thread %d\n", rot);
            for(int i=0; i<max_iter; ++i)
            {
                printf("%9x", i);
            }
            printf("\n");
            for(int i=0; i<max_iter; ++i)
            {
                gppcu_data_rd_slow(rot, i);
                printf("%9x", gppcu_data_rd_slow(rot, i));
            }
            printf("\n");
        } 
	} 
    //*/
	return 0;
}
