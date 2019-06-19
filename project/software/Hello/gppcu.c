#include "gppcu.h"
#include "utils.h"
#include <stdlib.h>
 

#define GPPCU_PARAMTERS \
enum { GPPCU_CMD_RD =  1 };\
enum { GPPCU_CMD_WR =  2 };\
enum { GPPCU_CMD_STAT =  4 };\
enum { GPPCU_CMD_CLK =  (1 << 31) };

static inline void gppcu_push_instr( uint32_t instr )
{
    GPPCU_PARAMTERS;
    IOWR_ALTERA_AVALON_PIO_DATA( PIO_DATAOUT_BASE, instr );
    IOWR_ALTERA_AVALON_PIO_DATA( PIO_CMD_BASE, 0 );
    IOWR_ALTERA_AVALON_PIO_DATA( PIO_CMD_BASE, GPPCU_CMD_CLK );
    IOWR_ALTERA_AVALON_PIO_DATA( PIO_CMD_BASE, 0 );
}

static inline uint32_t gppcu_data_rd( int ThreadIdx, int WordIdx )
{
    GPPCU_PARAMTERS;
    uint32_t cmd = ( GPPCU_CMD_RD << 24 ) | ( ThreadIdx << 16 ) | ( WordIdx );
    IOWR_ALTERA_AVALON_PIO_DATA( PIO_CMD_BASE, cmd );
    IOWR_ALTERA_AVALON_PIO_DATA( PIO_CMD_BASE, GPPCU_CMD_CLK | cmd );
    IOWR_ALTERA_AVALON_PIO_DATA( PIO_CMD_BASE, ~GPPCU_CMD_CLK & cmd );
    return IORD_ALTERA_AVALON_PIO_DATA( PIO_DATAIN_BASE );
}

static inline void gppcu_data_wr( int ThreadIdx, int WordIdx, uint32_t Data )
{
    GPPCU_PARAMTERS;
    uint32_t cmd = ( GPPCU_CMD_WR << 24 ) | ( ThreadIdx << 16 ) | ( WordIdx );
    IOWR_ALTERA_AVALON_PIO_DATA( PIO_DATAOUT_BASE, Data );
    IOWR_ALTERA_AVALON_PIO_DATA( PIO_CMD_BASE, cmd );
    IOWR_ALTERA_AVALON_PIO_DATA( PIO_CMD_BASE, GPPCU_CMD_CLK | cmd );
    IOWR_ALTERA_AVALON_PIO_DATA( PIO_CMD_BASE, ~GPPCU_CMD_CLK & cmd );
} 

void gppcu_device_command(uint8_t lparam, uint16_t command)
{
    GPPCU_PARAMTERS;
    
	uint32_t cmd = (GPPCU_CMD_STAT << 24) | (lparam << 16) | command;
	IOWR_ALTERA_AVALON_PIO_DATA(PIO_CMD_BASE, cmd); 
	IOWR_ALTERA_AVALON_PIO_DATA(PIO_CMD_BASE, GPPCU_CMD_CLK | cmd); 
	IOWR_ALTERA_AVALON_PIO_DATA(PIO_CMD_BASE, ~GPPCU_CMD_CLK | cmd); 
} 

void gppcu_stat( BOOL* poIsRunning, BOOL* poIsDone, uint8_t* poSzPerTask, uint8_t* poPmemEnd, uint8_t* poPmemHead, uint8_t* poNumCycles, uint8_t* poCurCycleIdx )
{
    GPPCU_PARAMTERS;
    uint32_t cmd = ( GPPCU_CMD_STAT << 24 ); 
    IOWR_ALTERA_AVALON_PIO_DATA( PIO_CMD_BASE, cmd ); 
    uint32_t stat = IORD_ALTERA_AVALON_PIO_DATA( PIO_DATAIN_BASE );
    *poIsRunning = ( stat & ( 1 << 31 ) ) != 0;
    *poIsDone = ( stat & ( 1 << 30 ) ) != 0;
    *poSzPerTask = ( stat >> 24 ) & 0x3f;
    *poPmemEnd = ( stat >> 16 ) & 0xff;
    *poPmemHead = ( stat >> 8 ) & 0xff;
    *poNumCycles = ( stat >> 4 ) & 0xf;
    *poCurCycleIdx = ( stat ) & 0xf;
}

 
void gppcu_init(swk_gppcu* const pp, int32_t num_threads, int32_t Capacity, int32_t MaxWordPerThread)
{
    pp->mcap = Capacity;
    pp->marr = malloc(Capacity * sizeof(swk_gppcu_data_t));
    passert( pp->marr != NULL, "Malloc returned nullptr" );
    pp->mnum = 0;
    pp->mnumthr = num_threads;
    pp->mtaskcycle = 0;
    pp->mtaskmem = 0;
	pp->ro_numtask = 0;
	pp->ro_max_word_per_thread = MaxWordPerThread;
}

void gppcu_init_task(swk_gppcu *const pp, uint8_t WordsPerTask, uint16_t NumTasks)
{
	pp->ro_numtask = NumTasks;
	pp->mtaskcycle = NumTasks / pp->mnumthr + 1;
	pp->mtaskmem = WordsPerTask;
	pp->ro_taskmaxcycle = pp->ro_max_word_per_thread / WordsPerTask;
	passert(NumTasks < pp->ro_taskmaxcycle * pp->mnumthr, "Task number has set too many.");
}

void gppcu_destroy( swk_gppcu* const pp )
{
	free(pp->marr);
}

void gppcu_program_autofeed_device( swk_gppcu const* const pp )
{
    swk_gppcu_instr_t const* lphead = pp->marr;
    swk_gppcu_instr_t const* const lpend = pp->marr + pp->mnum;

    enum {
        LPM_RUNSTOP = 0,
        LPM_NUMCYCLE = 1,
        LPM_SZPERCYCLE = 2,
        LPM_RESETPRG = 3
    };

    // Reset device
    gppcu_device_command( LPM_RESETPRG, 0 );
    gppcu_device_command( LPM_SZPERCYCLE, pp->mtaskmem );
    gppcu_device_command( LPM_NUMCYCLE, pp->mtaskcycle );

    // Push program
    while ( lphead < lpend ) { 
        gppcu_push_instr( *lphead++ );
    }

    // Run
    gppcu_device_command( LPM_RUNSTOP, 1 );
}

void gppcu_clear_instr( swk_gppcu* const pp )
{
    pp->mnum = 0;
}

void gppcu_write(
    swk_gppcu* pp, 
    swk_gppcu_data_t const* const data, 
    uint8_t ElementSizeInWords,  
    uint32_t ofst // Means local space offset on task domain. Units in word
)
{
	int idx_cycle = 0;
    int cycle_based_offset = 0;
    int idx_task, idx_thread;
    swk_gppcu_data_t const* head = data;

    passert( ofst + ElementSizeInWords < pp->mtaskmem, "Task memory overflow" );
    // Write data per thread
    for ( idx_task = 0, idx_thread = 0; idx_task < pp->ro_numtask; ++idx_task )
    {
        // @todo. 
        // write task_data 
        int elem_idx;
        for( elem_idx = 0; elem_idx < ElementSizeInWords; ++elem_idx ) 
        {
            gppcu_data_wr(idx_thread, cycle_based_offset + ofst + elem_idx, *head++);
        }

		++idx_thread;
        if ( idx_thread == 24 )
		{
			idx_thread = 0;
			++idx_cycle;

            cycle_based_offset += pp->mtaskmem;
		}
    }
}

void gppcu_read(
    swk_gppcu* pp,
    swk_gppcu_data_t* const dst,
    uint32_t Capacity,
    uint8_t ElementSizeInWords,
    uint32_t ofst
)
{
    int idx_cycle = 0;
    int cycle_based_offset = 0;
    int idx_task = 0, idx_thread = 0;
    int elem_idx;
    swk_gppcu_data_t* head = dst;
    swk_gppcu_data_t const* const lpend = head + Capacity;

    passert( ofst + ElementSizeInWords < pp->mtaskmem, "Task memory overflow" );
    // Write data per thread
    for ( ; idx_task < pp->ro_numtask; ++idx_task )
    {
        // validation check
        if ( head + ElementSizeInWords >= lpend )
        {
            break;
        }

        // @todo. 
        // write task_data 
        for ( elem_idx = 0; elem_idx < ElementSizeInWords; ++elem_idx )
        {
            *head++ = gppcu_data_rd( idx_thread, cycle_based_offset + ofst + elem_idx );
        }

        ++idx_thread;
        if ( idx_thread == 24 )
        {
            idx_thread = 0;
            ++idx_cycle;

            cycle_based_offset += pp->mtaskmem;
        }
    }
}

void gppcu_data_wr_slow( int ThreadIdx, int WordIdx, uint32_t Data )
{
    gppcu_data_wr( ThreadIdx, WordIdx, Data );
}

uint32_t gppcu_data_rd_slow( int ThreadIdx, int WordIdx )
{
    return gppcu_data_rd( ThreadIdx, WordIdx );
}

