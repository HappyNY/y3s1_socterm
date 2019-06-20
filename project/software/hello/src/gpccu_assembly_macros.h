#include "gppcu.h"

enum gp_regs{
    gp_reg0, gp_reg1, gp_reg2, gp_reg3, gp_reg4, gp_reg5, gp_reg6, gp_reg7, 
    gp_reg8, gp_reg9, gp_rega, gp_regb, gp_regc, gp_regd, gp_rege, gp_regf,
};

// @WARNING ... Should not include this file on header !
// only valid on local scope, cannot be redefined in same scope !
#define gp_set_gppcu_ptr(ptr) swk_gppcu_t* const GPPCU_INSTANCE____ = ptr
#define gp_fmul(rd, ra, rb) gppcu_fp_arith(GPPCU_INSTANCE____, COND_ALWAYS, OPR_A_FMUL, rd, ra, rb)
#define gp_fadd(rd, ra, rb) gppcu_fp_arith(GPPCU_INSTANCE____, COND_ALWAYS, OPR_A_FADD, rd, ra, rb)
#define gp_fsub(rd, ra, rb) gppcu_fp_arith(GPPCU_INSTANCE____, COND_ALWAYS, OPR_A_FSUB, rd, ra, rb)
#define gp_fdiv(rd, ra, rb) gppcu_fp_arith(GPPCU_INSTANCE____, COND_ALWAYS, OPR_A_FDIV, rd, ra, rb)
#define gp_fsqrt(rd, ra)    gppcu_fp_0(GPPCU_INSTANCE____, COND_ALWAYS, OPR_0_FSQRT, rd, ra)
#define gp_ftoi(rd, ra)     gppcu_fp_0(GPPCU_INSTANCE____, COND_ALWAYS, OPR_0_FTOI, rd, ra)
#define gp_itof(rd, ra)     gppcu_fp_0(GPPCU_INSTANCE____, COND_ALWAYS, OPR_0_ITOF, rd, ra)

#define gp_ldl(dest, addr, ofst)      gppcu_ldl(GPPCU_INSTANCE____, COND_ALWAYS, dest,addr,ofst)
#define gp_stl(data, addr, ofst)      gppcu_stl(GPPCU_INSTANCE____, COND_ALWAYS, data,addr,ofst)
#define gp_ldci(rd, addr)             gppcu_ldci(GPPCU_INSTANCE____, COND_ALWAYS, rd, addr)

#define gp_nop()                      gppcu_nop(GPPCU_INSTANCE____)
#define gp_mov(to, from, add)         gppcu_arith_s(GPPCU_INSTANCE____, COND_ALWAYS, OPR_S_MOV, false, to, from, add)
#define gp_mvi(to, imm17)             gppcu_mvi(GPPCU_INSTANCE____,     COND_ALWAYS, false, to, imm17)
#define gp_adc(rd, ra, rb, imm7)      gppcu_arith_a(GPPCU_INSTANCE____, COND_ALWAYS, OPR_A_ADC, false, rd, ra, rb, imm7)
#define gp_sbc(rd, ra, rb, imm7)      gppcu_arith_a(GPPCU_INSTANCE____, COND_ALWAYS, OPR_A_SBC, false, rd, ra, rb, imm7)
#define gp_and(rd, ra, rb, imm7)      gppcu_arith_a(GPPCU_INSTANCE____, COND_ALWAYS, OPR_A_AND, false, rd, ra, rb, imm7)
#define gp_orr(rd, ra, rb, imm7)      gppcu_arith_a(GPPCU_INSTANCE____, COND_ALWAYS, OPR_A_ORR, false, rd, ra, rb, imm7)
#define gp_xor(rd, ra, rb, imm7)      gppcu_arith_a(GPPCU_INSTANCE____, COND_ALWAYS, OPR_A_XOR, false, rd, ra, rb, imm7)
#define gp_adi(rd, ra, imm12)         gppcu_arith_b(GPPCU_INSTANCE____, COND_ALWAYS, OPR_B_ADI, false, rd, ra, imm12 )
#define gp_sbi(rd, ra, imm12)         gppcu_arith_b(GPPCU_INSTANCE____, COND_ALWAYS, OPR_B_SBI, false, rd, ra, imm12 )
#define gp_lsl(rd, ra)                gppcu_arith_0(GPPCU_INSTANCE____, COND_ALWAYS, OPR_0_LSL, false, rd, ra)
#define gp_lsr(rd, ra)                gppcu_arith_0(GPPCU_INSTANCE____, COND_ALWAYS, OPR_0_LSR, false, rd, ra)
#define gp_asr(rd, ra)                gppcu_arith_0(GPPCU_INSTANCE____, COND_ALWAYS, OPR_0_ASR, false, rd, ra)
