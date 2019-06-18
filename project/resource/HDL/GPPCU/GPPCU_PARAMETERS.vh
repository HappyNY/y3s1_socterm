    // control word definition
    localparam
        DBW = 32,
        ABW = 16, // Address bitwidth
        TBW = 12  // Thread selection bitwidth
    ; 
        
    localparam 
        INSTR_IMM0_7    =  5,
        INSTR_IMM1_12   =  0,
        INSTR_IMM2_17   =  0,
        INSTR_REGB_5    =  0,
        INSTR_REGA_5    = 12,
        INSTR_REGD_5    = 17,
        INSTR_S         = 22,
        INSTR_OPR_5     = 23,
        INSTR_COND_4    = 28
    ;
    
    localparam
        CW_VALID        =  0,
        CW_REGWR        =  1,
        CW_GMEMRD       =  2,
        CW_BSEL0        =  3,
        CW_BSEL1        =  4,
        CW_ALOPC0       =  5,
        CW_ALOPC1       =  6,
        CW_ALOPC2       =  7,
        CW_ALOPC3       =  8,
        CW_FPOPC0       =  9,
        CW_FPOPC1       = 10,
        CW_FPOPC2       = 11,
        CW_FPOPC3       = 12, 
        CW_LMEM_WR      = 13, // for STL
        CW_LMEM_RD      = 14, // for LDL
        CW_ALOP         = 15,
        CW_FPOP         = 16, 
        CW_USEREGA      = 17,
        CW_USEREGB      = 18, 
        CW_BITS         = 19
    ;
     
    // localparam.. instr mappings
    localparam[4:0]
        NOP     =  0, 
        MOV     =  1,
        MVN     =  2,
        ADC     =  3,
        SBC     =  4,
        AND     =  5,
        ORR     =  6,
        XOR     =  7,
        ADI     =  8,
        SBI     =  9,
        MVI     = 10,
        LSL     = 11,
        LSR     = 12,
        ASR     = 13,
        ITOF    = 14,
        FTOI    = 15,
        FMUL    = 16,
        FDIV    = 17,
        FADD    = 18,
        FSUB    = 19,
        FNEG    = 20,
        FSQRT   = 21, 
        LDL     = 22,
        LDCI    = 23,
        STL     = 24
    ; 
    
    // SREG indices
    localparam
        SREG_VCNZ = 0,
        SREG_V = 0,
        SREG_C = 1,
        SREG_N = 2,
        SREG_Z = 3,
        SREG_RESERVED = 4
    ;
    
    localparam[2:0]
        ALT_FPU_FTOI = 3'B001,
        ALT_FPU_ITOF = 3'B010,
        ALT_FPU_SQRT = 3'B011,
        ALT_FPU_MUL  = 3'B100,
        ALT_FPU_DIV  = 3'B111,
        ALT_FPU_SUB  = 3'B110,
        ALT_FPU_ADD  = 3'B101
    ;
    
    // COND VERIFIERS
    localparam[3:0]
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
        COND_LT     = 14
        COND_RESERVED = 15
    ;
    
        