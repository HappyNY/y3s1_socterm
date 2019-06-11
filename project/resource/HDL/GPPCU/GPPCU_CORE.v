// Same interface for both pipelined architecture and multicycled architecture.
module GPPCU_CORE #
(
    parameter
    NUM_THREAD = 32
)
(
    iACLK,          // Instruction should be valid on ACLK's rising edge.
    iINSTR,
    iINSTR_VALID,
    oINSTR_READY,   // Replaces instruction address. 
    
    iLMEM_THREAD_SEL, // Thread selection input
    iLMEM_ADDR,
    iLMEM_WDATA,
    oLMEM_RDATA,
    iLMEM_RD,
    iLMEM_WR,
    
    oGMEM_ADDR,
    iGMEM_WDATA,
    oGMEM_REQUEST
);
    /// params
    localparam
        DBW = 32,
        ABW = 16,
        TBW = 12;  
    
    /// ports
    input                   iACLK;
    input   [DBW-1:0]       iINSTR;
    input                   iINSTR_VALID;
    output                  oINSTR_READY; // Replaces instruction address. 
    
    input   [TBW-1:0]       iLMEM_THREAD_SEL; // Thread selection input
    input   [ABW-1:0]       iLMEM_ADDR;
    input   [DBW-1:0]       iLMEM_WDATA;
    output  [DBW-1:0]       oLMEM_RDATA;
    input                   iLMEM_RD;
    input                   iLMEM_WR;
    
    output  [ABW-1:0]       oGMEM_ADDR;
    input   [DBW-1:0]       iGMEM_WDATA;
    output                  oGMEM_REQUEST; 
        
    // port details
    
    // control word definition
    localparam 
        INSTR_IMM0_7    = 5,
        INSTR_IMM1_12   = 0,
        INSTR_IMM2_17   = 0,
        INSTR_REGB_5    = 0,
        INSTR_REGA_5    = 12,
        INSTR_REGD_5    = 17,
        INSTR_OPR_5     = 23,
        INSTR_COND_4    = 28;
    
    localparam
        CW_VALID        = 0,
        CW_REGWR        = 1,
        CW_GMEMRD       = 2,
        CW_BSEL0        = 3,
        CW_BSEL1        = 4,
        CW_ALOPC0       = 5,
        CW_ALOPC1       = 6,
        CW_ALOPC2       = 7,
        CW_ALOPC3       = 8,
        CW_ALOPC4       = 9,
        CW_FPOPC0       = 10,
        CW_FPOPC1       = 11,
        CW_FPOPC2       = 12,
        CW_FPOPC3       = 13, 
        CW_LMEM_WR      = 14, // for STL
        CW_LMEM_RD      = 15, // for LDL
        CW_ARITHMETIC   = 16,
        CW_FLOAT        = 17, 
        CW_BITS         = 18; 
        
    // pipeline stages
    reg[CW_BITS-1:0] CW_FET, CW_DEC, CW_EXEC, CW_WB;
    
    // thread instances
    generate
        genvar idx_thread;
    endgenerate
    
endmodule