#include "gppcu.h"
#include "utils.h"
#include <stdlib.h>
#include <string.h>

#define GPPCU_PARAMTERS \
enum { GPPCU_CMD_RD =  1 };\
enum { GPPCU_CMD_WR =  2 };\
enum { GPPCU_CMD_CONST = 3 };\
enum { GPPCU_CMD_STAT =  4 };\
enum { GPPCU_CMD_CLK =  (1 << 31) };

static inline void gppcu_push_instr( uint32_t CMD, uint32_t DATOUT, uint32_t instr )
{
    GPPCU_PARAMTERS;
    IOWR_ALTERA_AVALON_PIO_DATA( DATOUT, instr );
    IOWR_ALTERA_AVALON_PIO_DATA( CMD, 0 );
    IOWR_ALTERA_AVALON_PIO_DATA( CMD, GPPCU_CMD_CLK );
    IOWR_ALTERA_AVALON_PIO_DATA( CMD, 0 );
}

static inline uint32_t gppcu_data_rd( uint32_t CMD, uint32_t DATIN, int ThreadIdx, int WordIdx )
{
    GPPCU_PARAMTERS;
    uint32_t cmd = ( GPPCU_CMD_RD << 24 ) | ( ThreadIdx << 16 ) | ( WordIdx );
    IOWR_ALTERA_AVALON_PIO_DATA( CMD, cmd );
    IOWR_ALTERA_AVALON_PIO_DATA( CMD, GPPCU_CMD_CLK | cmd );
    IOWR_ALTERA_AVALON_PIO_DATA( CMD, ~GPPCU_CMD_CLK & cmd );
    return IORD_ALTERA_AVALON_PIO_DATA( DATIN );
}

static inline void gppcu_data_wr( uint32_t CMD, uint32_t DATOUT, int ThreadIdx, int WordIdx, uint32_t Data )
{
    GPPCU_PARAMTERS;
    uint32_t cmd = ( GPPCU_CMD_WR << 24 ) | ( ThreadIdx << 16 ) | ( WordIdx );
    IOWR_ALTERA_AVALON_PIO_DATA( DATOUT, Data );
    IOWR_ALTERA_AVALON_PIO_DATA( CMD, cmd );
    IOWR_ALTERA_AVALON_PIO_DATA( CMD, GPPCU_CMD_CLK | cmd );
    IOWR_ALTERA_AVALON_PIO_DATA( CMD, ~GPPCU_CMD_CLK & cmd );
}

static inline void gppcu_constant_wr( uint32_t CMD, uint32_t DATOUT, uint16_t Addr, uint32_t Data )
{
    GPPCU_PARAMTERS;
    uint32_t cmd = ( GPPCU_CMD_CONST << 24 ) | ( 0 << 16 ) | ( Addr );
    IOWR_ALTERA_AVALON_PIO_DATA( DATOUT, Data );
    IOWR_ALTERA_AVALON_PIO_DATA( CMD, cmd );
    IOWR_ALTERA_AVALON_PIO_DATA( CMD, GPPCU_CMD_CLK | cmd );
    IOWR_ALTERA_AVALON_PIO_DATA( CMD, ~GPPCU_CMD_CLK & cmd ); 
}
void gppcu_device_command( uint32_t CMD, uint8_t lparam, uint16_t command )
{
    GPPCU_PARAMTERS;

    uint32_t cmd = ( GPPCU_CMD_STAT << 24 ) | ( lparam << 16 ) | command;
    IOWR_ALTERA_AVALON_PIO_DATA( CMD, cmd );
    IOWR_ALTERA_AVALON_PIO_DATA( CMD, GPPCU_CMD_CLK | cmd );
    IOWR_ALTERA_AVALON_PIO_DATA( CMD, ~GPPCU_CMD_CLK | cmd );
}

void gppcu_stat( bool * poIsRunning, bool * poIsDone, uint8_t * poSzPerTask, uint8_t * poPmemEnd, uint8_t * poPmemHead, uint8_t * poNumCycles, uint8_t * poCurCycleIdx )
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


void gppcu_init( swk_gppcu * const pp, int32_t num_threads, int32_t Capacity, int32_t MaxWordPerThread )
{
    pp->mcap = Capacity;
    pp->marr = malloc( Capacity * sizeof( swk_gppcu_data_t ) );
    passert( pp->marr != NULL, "Malloc returned nullptr" );
    pp->mnum = 0;
    pp->mnumthr = num_threads;
    pp->mtaskcycle = 0;
    pp->mtaskmem = 0;
    pp->ro_numtask = 0;
    pp->ro_max_word_per_thread = MaxWordPerThread;
}

void gppcu_init_task( swk_gppcu * const pp, uint8_t WordsPerTask, uint16_t NumTasks )
{
    pp->ro_numtask = NumTasks;
    pp->mtaskcycle = NumTasks / pp->mnumthr + 1;
    pp->mtaskmem = WordsPerTask;
    pp->ro_taskmaxcycle = pp->ro_max_word_per_thread / WordsPerTask;
    passert( NumTasks < pp->ro_taskmaxcycle * pp->mnumthr, "Task number has set too many." );
}

void gppcu_destroy( swk_gppcu * const pp )
{
    free( pp->marr );
}

enum {
    LPM_RUNSTOP = 0,
    LPM_NUMCYCLE = 1,
    LPM_SZPERCYCLE = 2,
    LPM_RESETPRG = 3
};

static inline bool instr_useregd( uint8_t opc )
{
    switch ( opc )
    { 
    case OPR_NOP:      return false;
    case OPR_S_MOV:    return true;
    case OPR_S_MVN:    return true;
    case OPR_A_ADC:    return true;
    case OPR_A_SBC:    return true;
    case OPR_A_AND:    return true;
    case OPR_A_ORR:    return true;
    case OPR_A_XOR:    return true;
    case OPR_B_ADI:    return true;
    case OPR_B_SBI:    return true;
    case OPR_C_MVI:    return true;
    case OPR_0_LSL:    return true;
    case OPR_0_LSR:    return true;
    case OPR_0_ASR:    return true;
    case OPR_0_ITOF:   return true;
    case OPR_0_FTOI:   return true;
    case OPR_A_FMUL:   return true;
    case OPR_A_FDIV:   return true;
    case OPR_A_FADD:   return true;
    case OPR_A_FSUB:   return true;
    case OPR_X_FNEG:   return false;
    case OPR_0_FSQRT:  return true;
    case OPR_LDL:      return true;
    case OPR_LDCI:     return true;
    case OPR_STL:      return false;
    default: return false;
    }
}

static inline bool instr_userega( uint8_t opc )
{
    switch ( opc )
    {
    case OPR_NOP:      return false;
    case OPR_S_MOV:    return false;
    case OPR_S_MVN:    return false;
    case OPR_A_ADC:    return true;
    case OPR_A_SBC:    return true;
    case OPR_A_AND:    return true;
    case OPR_A_ORR:    return true;
    case OPR_A_XOR:    return true;
    case OPR_B_ADI:    return true;
    case OPR_B_SBI:    return true;
    case OPR_C_MVI:    return false;
    case OPR_0_LSL:    return true;
    case OPR_0_LSR:    return true;
    case OPR_0_ASR:    return true;
    case OPR_0_ITOF:   return true;
    case OPR_0_FTOI:   return true;
    case OPR_A_FMUL:   return true;
    case OPR_A_FDIV:   return true;
    case OPR_A_FADD:   return true;
    case OPR_A_FSUB:   return true;
    case OPR_X_FNEG:   return true;
    case OPR_0_FSQRT:  return true;
    case OPR_LDL:      return true;
    case OPR_LDCI:     return false;
    case OPR_STL:      return true;
    default: return false;
    }
}

static inline bool instr_useregb( uint8_t opc )
{
    switch ( opc )
    {
    case OPR_NOP:      return false;
    case OPR_S_MOV:    return true;
    case OPR_S_MVN:    return true;
    case OPR_A_ADC:    return true;
    case OPR_A_SBC:    return true;
    case OPR_A_AND:    return true;
    case OPR_A_ORR:    return true;
    case OPR_A_XOR:    return true;
    case OPR_B_ADI:    return false;
    case OPR_B_SBI:    return false;
    case OPR_C_MVI:    return false;
    case OPR_0_LSL:    return false;
    case OPR_0_LSR:    return false;
    case OPR_0_ASR:    return false;
    case OPR_0_ITOF:   return false;
    case OPR_0_FTOI:   return false;
    case OPR_A_FMUL:   return true;
    case OPR_A_FDIV:   return true;
    case OPR_A_FADD:   return true;
    case OPR_A_FSUB:   return true;
    case OPR_X_FNEG:   return false;
    case OPR_0_FSQRT:  return false;
    case OPR_LDL:      return true;
    case OPR_LDCI:     return false;
    case OPR_STL:      return true;
    default: return false;
    }
}

void gppcu_program_autofeed_device( swk_gppcu const* const pp )
{
    swk_gppcu_instr_t const* lphead = pp->marr;
    swk_gppcu_instr_t const* const lpend = pp->marr + pp->mnum;

    // Reset device
    gppcu_device_command( pp->MMAP_CMDOUT, LPM_RESETPRG, 0 );
    gppcu_device_command( pp->MMAP_CMDOUT, LPM_SZPERCYCLE, pp->mtaskmem );
    gppcu_device_command( pp->MMAP_CMDOUT, LPM_NUMCYCLE, pp->mtaskcycle );

    // Push program
    // Generate stall in software level ...
    // Since hardware stall signal generator is not stable ! 
    while ( lphead < lpend ) {
        // @todo. verify hardware stall generator and remove this
        const swk_gppcu_instr_t instr = *lphead++;
#if GPPCU_ENABLE_UNCONDITIONAL_BUBBLE_GENERATION  
#endif
        gppcu_push_instr( pp->MMAP_CMDOUT, pp->MMAP_DATOUT, 0 );
        gppcu_push_instr( pp->MMAP_CMDOUT, pp->MMAP_DATOUT, 0 );
        gppcu_push_instr( pp->MMAP_CMDOUT, pp->MMAP_DATOUT, instr );
    }
}

char* instr_to_string( char* buff64, swk_gppcu_instr_t instr )
{ 
    char* head = buff64;
    int space[] = { 4, 5, 1, 5, 5, 7, 5 };
    memset( buff64, '0', 64 ); 
    int* c = space;
    int cnt = 32;
    while ( cnt-- )
    {
        *head++ = instr & 0x80000000u ? '1' : '0';
        instr <<= 1;
        if ( --c[0] == 0 )
        {
            ++c;
            *head++ = ' ';
        }
    }
    *head++ = 0;
    return head;
}

void gppcu_program_autofeed_device_parallel( swk_gppcu const* const pp )
{
    swk_gppcu_instr_t const* lphead = pp->marr;
    swk_gppcu_instr_t const* const lpend = pp->marr + pp->mnum;

    // Reset device
    gppcu_device_command( pp->MMAP_CMDOUT, LPM_RESETPRG, 0 );

    // IMPORTANT !!! TASK MEMORY AND CYCLE ARE ADJUSTED ...
    gppcu_device_command( pp->MMAP_CMDOUT, LPM_SZPERCYCLE, pp->mtaskmem * 2);
    gppcu_device_command( pp->MMAP_CMDOUT, LPM_NUMCYCLE, ( pp->mtaskcycle + 1 ) / 2 );

    // REGF will be automatically set as pivot register 
    gppcu_push_instr( pp->MMAP_CMDOUT, pp->MMAP_DATOUT, 0 );
    gppcu_push_instr( pp->MMAP_CMDOUT, pp->MMAP_DATOUT, 0 ); 
    gppcu_push_instr(
        pp->MMAP_CMDOUT,
        pp->MMAP_DATOUT,
        GPPCU_ASSEMBLE_INSTRUCTION_C( COND_ALWAYS, OPR_LDCI, 0, REG15, 0 )
    ); 
    gppcu_push_instr( pp->MMAP_CMDOUT, pp->MMAP_DATOUT, 0 );
    gppcu_push_instr( pp->MMAP_CMDOUT, pp->MMAP_DATOUT, 0 );
    gppcu_push_instr(
        pp->MMAP_CMDOUT,
        pp->MMAP_DATOUT,
        GPPCU_ASSEMBLE_INSTRUCTION_B( COND_ALWAYS, OPR_B_ADI, 0, REG31, REG15, pp->mtaskmem )
    ); 
    gppcu_push_instr( pp->MMAP_CMDOUT, pp->MMAP_DATOUT, 0 );
    gppcu_push_instr( pp->MMAP_CMDOUT, pp->MMAP_DATOUT, 0 );

    // Push program
    // Generate stall in software level ...
    // Since hardware stall signal generator is not stable !

    // int8_t prev_d = -1;
    while ( lphead < lpend ) {
        // @todo. verify hardware stall generator and remove this
        swk_gppcu_instr_t const instr = *lphead++;  

        // @parallel instruction
        swk_gppcu_instr_t parallel_instr[1] = { instr };
        const uint8_t opc = ( instr >> 23 ) & 0x1f;

        // don't repeat nop
        if ( opc == 0 )
        {
            gppcu_push_instr( pp->MMAP_CMDOUT, pp->MMAP_DATOUT, 0 );
            continue;
        }

        // Parallelize
        if ( instr_userega( opc ) ) {
            parallel_instr[0] += 16 << 12; 
        }
        if ( instr_useregb( opc ) ) {
            parallel_instr[0] += 16 << 0; 
        }
        if ( instr_useregd( opc ) ) { 
            parallel_instr[0] += 16 << 17; 
        }

        gppcu_push_instr( pp->MMAP_CMDOUT, pp->MMAP_DATOUT, instr );  
        gppcu_push_instr( pp->MMAP_CMDOUT, pp->MMAP_DATOUT, 0 );
        gppcu_push_instr( pp->MMAP_CMDOUT, pp->MMAP_DATOUT, parallel_instr[0] );   
        gppcu_push_instr( pp->MMAP_CMDOUT, pp->MMAP_DATOUT, 0 );
    }
}

void gppcu_run_autofeed_device( swk_gppcu const* const pp )
{ 
    // Run
    gppcu_device_command( pp->MMAP_CMDOUT, LPM_RUNSTOP, 0 );
    // wait for few cycles
    volatile int i = 32;
    while ( i-- );
    gppcu_device_command( pp->MMAP_CMDOUT, LPM_RUNSTOP, 1 );
}

void gppcu_clear_instr( swk_gppcu * const pp )
{
    pp->mnum = 0;
}

void gppcu_write(
    swk_gppcu * pp,
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
        for ( elem_idx = 0; elem_idx < ElementSizeInWords; ++elem_idx )
        {
            gppcu_data_wr( pp->MMAP_CMDOUT, pp->MMAP_DATOUT, idx_thread, cycle_based_offset + ofst + elem_idx, *head++ );
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
    swk_gppcu * pp,
    swk_gppcu_data_t * const dst,
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
            *head++ = gppcu_data_rd( pp->MMAP_CMDOUT, pp->MMAP_DATIN, idx_thread, cycle_based_offset + ofst + elem_idx );
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

void gppcu_write_const( swk_gppcu* pp, swk_gppcu_data_t const* const data, uint32_t ofst, uint32_t size )
{
    swk_gppcu_data_t const* lphead = data, *lpend = data + size;

    while ( lphead != lpend )
    {
        gppcu_constant_wr( pp->MMAP_CMDOUT, pp->MMAP_DATOUT, ofst++, lphead[0] );
        ++lphead;
    }
}

void gppcu_data_wr_slow( int ThreadIdx, int WordIdx, uint32_t Data )
{
    gppcu_data_wr( PIO_CMD_BASE, PIO_DATAOUT_BASE, ThreadIdx, WordIdx, Data );
}

uint32_t gppcu_data_rd_slow( int ThreadIdx, int WordIdx )
{
    return gppcu_data_rd( PIO_CMD_BASE, PIO_DATAIN_BASE, ThreadIdx, WordIdx );
}

