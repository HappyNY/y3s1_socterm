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