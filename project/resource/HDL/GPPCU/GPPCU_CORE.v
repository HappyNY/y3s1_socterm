// Same interface for both pipelined architecture and multicycled architecture.
module GPPCU_CORE #
(
    parameter
    NUM_THREAD = 32
)
(
    iACLK,          // Instruction should be valid on ACLK's rising edge.
    inRST,
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
    input                   inRST;
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
        CW_USEREGA      = 18,
        CW_USEREGB      = 19, 
        CW_BITS         = 20; 
        
    // regs
    reg cw_valid_fetch, cw_valid_decode, cw_valid_exec, cw_valid_writeback;
    reg[CW_BITS-1:1] cw_decode, cw_exec, cw_writeback;
    reg[DBW-1:0] instr_fetch, instr_decode, instr_exec, instr_writeback;
    
    // wires
    wire can_fetch;
    wire data_stall; // prevents data hazard
    wire[CW_BITS-1:1] decoding_cw;
    
    // logics
    assign oINSTR_READY = can_fetch;
    
    //      F D E W
    //        F D E W
    //          F D . E W
    //            F . D E W ...
    // Proceed pipeline
    always @(posedge iACLK) begin
        cw_valid_writeback  <= cw_valid_exec;
        cw_valid_exec       <= data_stall ? 0               : cw_valid_decode;
        cw_valid_decode     <= data_stall ? cw_valid_decode : cw_valid_fetch;
        cw_valid_fetch      <= data_stall ? cw_valid_fetch  : can_fetch & iINSTR_VALID;
        
        cw_writeback        <= cw_exec;
        cw_exec             <= data_stall ? cw_exec         : cw_decode;
        cw_decode           <= data_stall ? cw_decode       : decoding_cw;
        // There is no cw_instr 
        
        instr_writeback     <= instr_exec;
        instr_exec          <= data_stall ? instr_exec      : instr_decode;
        instr_decode        <= data_stall ? instr_decode    : instr_fetch;
        instr_fetch         <= data_stall ? instr_fetch     : iINSTR;
    end
    // DECODE STAGE
    // Stall generator
    wire reg_use_en;
    assign data_stall = ~reg_use_en;
    GPPCU_STALL_GEN #(
        // .NUMREG(32)
    ) GPPCU_STALL_GEN_inst
    (
        .iACLK(iACLK),
        .inRST(inRST),
        .iREGD(instr_decode[INSTR_REGD_5+:5]),
        .iREGA(instr_decode[INSTR_REGA_5+:5]),
        .iREGB(instr_decode[INSTR_REGB_5+:5]),
        .iVALID_REGD(cw_valid_decode & cw_decode[CW_REGWR]),
        .iVALID_REGA(cw_valid_decode & cw_decode[CW_USEREGA]),
        .iVALID_REGB(cw_valid_decode & cw_decode[CW_USEREGB]),
        .oENABLED(reg_use_en),
        .iWRREG(instr_writeback[INSTR_REGD_5+:5]),
        .iWRREG_VALID(cw_valid_writeback & cw_writeback[CW_REGWR])
    );
    // EXECUTE STAGE
    
    // WRITEBACK STAGE 
    // thread instances
    generate
        genvar idx_thread;
    endgenerate
    
endmodule