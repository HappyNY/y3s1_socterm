#pragma once
#include <stdint.h>
#include <system.h>
#include <altera_avalon_pio_regs.h>
#include "utils.h"

////////////////////////////////////////////////////////
// CONSTANTS
////////////////////////////////////////////////////////
typedef enum GPPCU_OPERATION
{
	OPR_NOP     =  0, 
	OPR_S_MOV     =  1,
	OPR_S_MVN     =  2,
	OPR_A_ADC     =  3,
	OPR_A_SBC     =  4,
	OPR_A_AND     =  5,
	OPR_A_ORR     =  6,
	OPR_A_XOR     =  7,
	OPR_B_ADI     =  8,
	OPR_B_SBI     =  9,
	OPR_C_MVI     = 10,
	OPR_0_LSL     = 11,
	OPR_0_LSR     = 12,
	OPR_0_ASR     = 13,
	OPR_0_ITOF    = 14,
	OPR_0_FTOI    = 15,
	OPR_A_FMUL    = 16,
	OPR_A_FDIV    = 17,
	OPR_A_FADD    = 18,
	OPR_A_FSUB    = 19,
	OPR_X_FNEG    = 20,
	OPR_0_FSQRT   = 21, 
	OPR_LDL     = 22,
	OPR_LDCI    = 23,
	OPR_STL     = 24
} GPPCU_OPERATION;

typedef enum GPPCU_CONDTION
{ 
	COND_ALWAYS = 0,
	COND_NEVER  = 1,
	COND_C      = 2,
	COND_NC     = 3,
	COND_Z      = 4,
	COND_NZ     = 5,
	COND_V      = 6,
	COND_NV     = 7,
	COND_N      = 8,
	COND_NN     = 9,
	COND_NEG    = 10,
	COND_POS    = 11,
	COND_EQ     = 12,
	COND_GR     = 13,
	COND_LT     = 14,
} GPPCU_CONDTION;

typedef enum GPPCU_REGISTER {
    REG00, REG01, REG02, REG03, REG04, REG05, REG06, REG07, 
    REG08, REG09, REG10, REG11, REG12, REG13, REG14, REG15, 
    REG16, REG17, REG18, REG19, REG20, REG21, REG22, REG23, 
    REG24, REG25, REG26, REG27, REG28, REG29, REG30, REG31
} GPPCU_REGISTER;

enum {
    GPPCU_MEMORY_OFST_GMEMADDR = 0
};

////////////////////////////////////////////////////////
// TYPES
////////////////////////////////////////////////////////

typedef float swk_gppcu_data_t;
typedef uint32_t swk_gppcu_instr_t; 
struct tagGPPCU
{
    swk_gppcu_instr_t*   marr; 
    int32_t     mcap; 
    int32_t     mnum; 
    int32_t     mnumthr;
    int32_t     mtaskcycle;
    int32_t     mtaskmem;
	int32_t     ro_taskmaxcycle;
    int32_t     ro_numtask;
	int32_t 	ro_max_word_per_thread;

    uint32_t    MMAP_DATOUT;
    uint32_t    MMAP_DATIN;
    uint32_t    MMAP_CMDOUT; 
};
typedef struct tagGPPCU swk_gppcu;

////////////////////////////////////////////////////////
// INTERFACE
////////////////////////////////////////////////////////
void gppcu_init(swk_gppcu* const pp, int32_t num_threads, int32_t Capacity, int32_t MaxWordPerThread); 
void gppcu_destroy(swk_gppcu* const pp);
void gppcu_program_autofeed_device(swk_gppcu const* const pp);
void gppcu_run_autofeed_device();

void gppcu_clear_instr(swk_gppcu* const pp); 

void gppcu_init_task(swk_gppcu *const pp, uint8_t WordsPerTask, uint16_t NumTasks);

void gppcu_stat(
    BOOL* poIsRunning, 
    BOOL* poIsDone, 
    uint8_t* poSzPerTask, 
    uint8_t* poPmemEnd,
    uint8_t* poPmemHead,
    uint8_t* poNumCycles,
    uint8_t* poCurCycleIdx
);

// Data read/write process is capsulized
// Maximum memory at once = 512 * 24 * 4 bytes ...  = 48kBytes
// NumElements should be equal to NumTasks
void gppcu_write(
    swk_gppcu* pp, 
    swk_gppcu_data_t const* const data, 
    uint8_t ElementSizeInWords,  
    uint32_t ofst // Means local space offset on task domain. Units in word
);
void gppcu_read(
    swk_gppcu* pp, 
    swk_gppcu_data_t * const dst, 
    uint32_t Capacity, 
    uint8_t ElementSizeInWords, 
    uint32_t ofst
);

// queue instruction

#define GPPCU_ASSEMBLE_OPR(COND, OPR, S_EN) (((uint32_t)(COND) << 28) | ((uint32_t)(OPR) << 23) | ((uint32_t)(S_EN) << 22))
#define GPPCU_ASSEMBLE_INSTRUCTION_A(COND, OPR, S_EN, RD, RA, IMM7, RB)\
    (uint32_t)(GPPCU_ASSEMBLE_OPR(COND, OPR, S_EN) | ((RD) << 17) | ((RA) << 12) | (((IMM7) & 0x7f) << 5) | (RB))

#define GPPCU_ASSEMBLE_INSTRUCTION_B(COND, OPR, S_EN, RD, RA, IMM12) \
    (uint32_t)(GPPCU_ASSEMBLE_OPR(COND, OPR, S_EN) | ((RD) << 17) | ((RA) << 12) | ((IMM12) & 0x0fff))

#define GPPCU_ASSEMBLE_INSTRUCTION_C(COND, OPR, S_EN, RD, IMM17) \
    (uint32_t)(GPPCU_ASSEMBLE_OPR(COND, OPR, S_EN) | ((RD) << 17) | ((IMM17) & 0x1ffff))



////////////////////////////////////////////////////////
// INTERFACE - INSTRUCTION BUILD
////////////////////////////////////////////////////////
void gppcu_data_wr_slow( int ThreadIdx, int WordIdx, uint32_t Data );
uint32_t gppcu_data_rd_slow( int ThreadIdx, int WordIdx );

static inline void gppcu_put_instr( swk_gppcu* const pp, swk_gppcu_instr_t instr )
{
    pp->marr[pp->mnum++] = instr;
    passert( pp->mnum < pp->mcap, "Instruction memory overflow!" );
}

static inline void gppcu_nop(
    swk_gppcu* const pp )
{
    gppcu_put_instr( pp, 0 );
}

static inline void gppcu_arith_s(
    swk_gppcu* const pp,
    GPPCU_CONDTION cond,
    GPPCU_OPERATION opr,
    BOOL s,
    GPPCU_REGISTER regd,
    GPPCU_REGISTER regb,
    int8_t imm7 )
{
    gppcu_put_instr(
        pp,
        GPPCU_ASSEMBLE_INSTRUCTION_A( cond, opr, s != 0, regd, 0, imm7, regb )
    );
}
static inline void gppcu_arith_0(
    swk_gppcu* const pp,
    GPPCU_CONDTION cond,
    GPPCU_OPERATION opr,
    BOOL s,
    GPPCU_REGISTER regd,
    GPPCU_REGISTER rega )
{
    gppcu_put_instr(
        pp,
        GPPCU_ASSEMBLE_INSTRUCTION_B( cond, opr, s != 0, regd, rega, 0 )
    );
}

static inline void gppcu_mvi(
    swk_gppcu* const pp,
    GPPCU_CONDTION cond,
    BOOL s,
    GPPCU_REGISTER regd,
    int32_t imm17 )
{
    gppcu_put_instr(
        pp,
        GPPCU_ASSEMBLE_INSTRUCTION_C( cond, OPR_C_MVI, s != 0, regd, imm17 )
    );
}

static inline void gppcu_arith_a(
    swk_gppcu* const pp,
    GPPCU_CONDTION cond,
    GPPCU_OPERATION opr,
    BOOL s,
    GPPCU_REGISTER regd,
    GPPCU_REGISTER rega,
    GPPCU_REGISTER regb,
    int8_t imm7 )
{
    gppcu_put_instr(
        pp,
        GPPCU_ASSEMBLE_INSTRUCTION_A( cond, opr, s != 0, regd, rega, imm7, regb )
    );
}

static inline void gppcu_arith_b(
    swk_gppcu* const pp,
    GPPCU_CONDTION cond,
    GPPCU_OPERATION opr,
    BOOL s,
    GPPCU_REGISTER regd,
    GPPCU_REGISTER rega,
    int16_t imm12 )
{
    gppcu_put_instr(
        pp,
        GPPCU_ASSEMBLE_INSTRUCTION_B( cond, opr, s != 0, regd, rega, imm12 )
    );
}
static inline void gppcu_fp_arith(
    swk_gppcu* const pp,
    GPPCU_CONDTION cond,
    GPPCU_OPERATION opr,
    GPPCU_REGISTER regd,
    GPPCU_REGISTER rega,
    GPPCU_REGISTER regb )
{
    gppcu_put_instr(
        pp,
        GPPCU_ASSEMBLE_INSTRUCTION_A( cond, opr, 0, regd, rega, 0, regb )
    );
}
static inline void gppcu_fp_0(
    swk_gppcu* const pp,
    GPPCU_CONDTION cond,
    GPPCU_OPERATION opr,
    GPPCU_REGISTER regd,
    GPPCU_REGISTER rega )
{
    gppcu_put_instr(
        pp,
        GPPCU_ASSEMBLE_INSTRUCTION_A( cond, opr, 0, regd, rega, 0, 0 )
    );
} 
static inline void gppcu_ldl(
    swk_gppcu* const pp,
    GPPCU_CONDTION cond,
    GPPCU_REGISTER dest,
    GPPCU_REGISTER addr,
    uint8_t ofst )
{
    gppcu_put_instr(
        pp,
        GPPCU_ASSEMBLE_INSTRUCTION_A( cond, OPR_LDL, 0, dest, 0, ofst, addr )
    );
}
static inline void gppcu_stl(
    swk_gppcu* const pp,
    GPPCU_CONDTION cond,
    GPPCU_REGISTER data,
    GPPCU_REGISTER addr,
    uint8_t ofst )
{
    gppcu_put_instr(
        pp,
        GPPCU_ASSEMBLE_INSTRUCTION_A( cond, OPR_STL, 0, 0, data, ofst, addr )
    );
}
static inline void gppcu_ldci(
    swk_gppcu* const pp,
    GPPCU_CONDTION cond,
    GPPCU_REGISTER dest,
    uint32_t addr )
{
    gppcu_put_instr(
        pp,
        GPPCU_ASSEMBLE_INSTRUCTION_C( cond, OPR_LDCI, 0, dest, addr )
    );
}

