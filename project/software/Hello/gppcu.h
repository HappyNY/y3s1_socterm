#pragma once
#include <stdint.h>
#include <system.h>
#include <altera_avalon_pio_regs.h>

#ifndef BOOL 
#define BOOL uint8_t
#endif

enum Opr
{
	OPR_NOP     =  0, 
	OPR_MOV     =  1,
	OPR_MVN     =  2,
	OPR_ADC     =  3,
	OPR_SBC     =  4,
	OPR_AND     =  5,
	OPR_ORR     =  6,
	OPR_XOR     =  7,
	OPR_ADI     =  8,
	OPR_SBI     =  9,
	OPR_MVI     = 10,
	OPR_LSL     = 11,
	OPR_LSR     = 12,
	OPR_ASR     = 13,
	OPR_ITOF    = 14,
	OPR_FTOI    = 15,
	OPR_FMUL    = 16,
	OPR_FDIV    = 17,
	OPR_FADD    = 18,
	OPR_FSUB    = 19,
	OPR_FNEG    = 20,
	OPR_FSQRT   = 21, 
	OPR_LDL     = 22,
	OPR_LDCI    = 23,
	OPR_STL     = 24
};

enum Cond
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
};

#define GPPCU_ASSEMBLE_OPR(COND, OPR, S_EN) (((uint32_t)(COND) << 28) | ((uint32_t)(OPR) << 23) | ((uint32_t)(S_EN) << 22))
#define GPPCU_ASSEMBLE_INSTRUCTION_A(COND, OPR, S_EN, RD, RA, IMM7, RB)\
    (uint32_t)(GPPCU_ASSEMBLE_OPR(COND, OPR, S_EN) | ((RD) << 17) | ((RA) << 12) | (((IMM7) & 0x7f) << 5) | (RB))

#define GPPCU_ASSEMBLE_INSTRUCTION_B(COND, OPR, S_EN, RD, RA, IMM12) \
    (uint32_t)(GPPCU_ASSEMBLE_OPR(COND, OPR, S_EN) | ((RD) << 17) | ((RA) << 12) | ((IMM12) & 0x0fff))

#define GPPCU_ASSEMBLE_INSTRUCTION_C(COND, OPR, S_EN, RD, IMM17) \
    (uint32_t)(GPPCU_ASSEMBLE_OPR(COND, OPR, S_EN) | ((RD) << 17) | ((IMM17) & 0x1ffff))


typedef float swk_gppcu_data_t;

struct tagGPPCU
{
    uint32_t*   marr; 
    int32_t     mcap; 
    int32_t     mnum; 
    int32_t     mnumthr;
    int32_t     mtaskcycle;
    int32_t     mtaskmem;
	int32_t     ro_taskmaxcycle;
    int32_t     ro_numtask;
	int32_t 	ro_max_word_per_thread;
};
typedef struct tagGPPCU swk_gppcu_stat;

void gppcu_init(swk_gppcu_stat* const pp, int32_t num_threads, int32_t Capacity, int32_t MaxWordPerThread);
void gppcu_destroy(swk_gppcu_stat* const pp);
void gppcu_program_queue_device(swk_gppcu_stat const* const pp);
void gppcu_program_autofeed_device(swk_gppcu_stat const* const pp);

void gppcu_clear_instr(swk_gppcu_stat* const pp);
BOOL gppcu_is_running(swk_gppcu_data_t const* const pp);

void gppcu_init_task(swk_gppcu_stat *const pp, uint8_t WordsPerTask, uint16_t NumTasks);

// Data read/write process is capsulized
// Maximum memory at once = 512 * 24 * 4 bytes ...  = 48kBytes
// NumElements should be equal to NumTasks
void gppcu_write(
    swk_gppcu_stat* pp, 
    swk_gppcu_data_t const* const data, 
    uint8_t ElementSizeInWords,  
    uint32_t ofst // Means local space offset on task domain. Units in word
);
void gppcu_read(
    swk_gppcu_stat* pp, 
    swk_gppcu_data_t * const dst, 
    uint32_t Capacity, 
    uint8_t ElementSizeInWords, 
    uint32_t ofst
);

// Queueing instruction

#define GPPCU_PARAMTERS \
    enum { GPPCU_CMD_RD =  1 };\
    enum { GPPCU_CMD_WR =  2 };\
    enum { GPPCU_CM_STAT =  4 };\
    enum { GPPCU_CMD_CLK =  (1 << 31) };
    
static inline void gppcu_queue_instr(uint32_t instr)
{   
    GPPCU_PARAMTERS;
	IOWR_ALTERA_AVALON_PIO_DATA(PIO_DATAOUT_BASE, instr);
	IOWR_ALTERA_AVALON_PIO_DATA(PIO_CMD_BASE, 0); 
	IOWR_ALTERA_AVALON_PIO_DATA(PIO_CMD_BASE, GPPCU_CMD_CLK); 
	IOWR_ALTERA_AVALON_PIO_DATA(PIO_CMD_BASE, 0);  
} 

static inline uint32_t gppcu_data_rd(int ThreadIdx, int WordIdx)
{ 
    GPPCU_PARAMTERS;
	uint32_t cmd = (GPPCU_CMD_RD << 24) | (ThreadIdx << 16) | (WordIdx);
	IOWR_ALTERA_AVALON_PIO_DATA(PIO_CMD_BASE, cmd); 
	IOWR_ALTERA_AVALON_PIO_DATA(PIO_CMD_BASE, GPPCU_CMD_CLK|cmd); 
	IOWR_ALTERA_AVALON_PIO_DATA(PIO_CMD_BASE, ~GPPCU_CMD_CLK&cmd); 
	return IORD_ALTERA_AVALON_PIO_DATA(PIO_DATAIN_BASE);
}

static inline void gppcu_data_wr(int ThreadIdx, int WordIdx, uint32_t Data)
{ 
    GPPCU_PARAMTERS;
	uint32_t cmd = (GPPCU_CMD_WR << 24) | (ThreadIdx << 16) | (WordIdx);
	IOWR_ALTERA_AVALON_PIO_DATA(PIO_DATAOUT_BASE, Data); 
	IOWR_ALTERA_AVALON_PIO_DATA(PIO_CMD_BASE, cmd); 
	IOWR_ALTERA_AVALON_PIO_DATA(PIO_CMD_BASE, GPPCU_CMD_CLK|cmd); 
	IOWR_ALTERA_AVALON_PIO_DATA(PIO_CMD_BASE, ~GPPCU_CMD_CLK&cmd);  
}
#undef GPPCU_PARAMETERS

void gppcu_stat_queue(uint16_t* phead, uint16_t* ptail);