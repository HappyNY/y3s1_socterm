#pragma once
#include <stdint.h>

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
#define GPPCU_ASSEMBLE_INSTRUCTION_A(COND, OPR, S_EN, RD, RA, IMM7, RB) (uint32_t)(GPPCU_ASSEMBLE_OPR(COND, OPR, S_EN) | ((RD) << 17) | ((RA) << 12) | (((IMM7) & 0x7f) << 5) | (RB))
#define GPPCU_ASSEMBLE_INSTRUCTION_B(COND, OPR, S_EN, RD, RA, IMM12) (uint32_t)(GPPCU_ASSEMBLE_OPR(COND, OPR, S_EN) | ((RD) << 17) | ((RA) << 12) | ((IMM12) & 0x0fff))
#define GPPCU_ASSEMBLE_INSTRUCTION_C(COND, OPR, S_EN, RD, IMM17) (uint32_t)(GPPCU_ASSEMBLE_OPR(COND, OPR, S_EN) | ((RD) << 17) | ((IMM17) & 0x1ffff))
