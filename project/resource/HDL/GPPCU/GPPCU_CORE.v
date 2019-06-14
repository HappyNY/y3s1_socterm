// Same interface for both pipelined architecture and multicycled architecture.
module GPPCU_CORE #
(
    parameter
    NUM_THREAD = 1 /*32, Debug option for temporary*/
)
(
    iACLK,          // Instruction should be valid on ACLK's rising edge.
    inRST,
    iINSTR,
    iINSTR_VALID,
    oINSTR_READY,   // Replaces instruction address. 
    
    iLMEM_CLK,
    iLMEM_THREAD_SEL, // Thread selection input
    iLMEM_ADDR,
    iLMEM_WDATA,
    oLMEM_RDATA,
    iLMEM_RD,
    iLMEM_WR,
    
    oGMEM_ADDR,
    iGMEM_WDATA,
    oGMEM_CLK
);
    /// params
    `include "GPPCU_PARAMETERS.vh"
    
    /// ports
    input                   iACLK;
    input                   inRST;
    input   [DBW-1:0]       iINSTR;
    input                   iINSTR_VALID;
    output                  oINSTR_READY; // Replaces instruction address. 
    
	input                   iLMEM_CLK;
    input   [TBW-1:0]       iLMEM_THREAD_SEL; // Thread selection input
    input   [ABW-1:0]       iLMEM_ADDR;
    input   [DBW-1:0]       iLMEM_WDATA;
    output  [DBW-1:0]       oLMEM_RDATA;
    input                   iLMEM_RD;
    input                   iLMEM_WR;
    
    output  [ABW-1:0]       oGMEM_ADDR;
    input   [DBW-1:0]       iGMEM_WDATA;
    output                  oGMEM_CLK;  
        
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
    
    // Proceed pipeline
    // @example
    //      F D E W
    //        F D E W
    //          F D . . E W
    //            F . . D E W
    always @(posedge iACLK) begin
        cw_valid_writeback  <= ~inRST ? 0 : cw_valid_exec;
        cw_valid_exec       <= ~inRST ? 0 : data_stall ? 0               : cw_valid_decode;
        cw_valid_decode     <= ~inRST ? 0 : data_stall ? cw_valid_decode : cw_valid_fetch;
        cw_valid_fetch      <= ~inRST ? 0 : data_stall ? cw_valid_fetch  : can_fetch & iINSTR_VALID;
        
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
    wire on_calculation_delay;
    assign data_stall = ~reg_use_en | on_calculation_delay;
    assign can_fetch  =  reg_use_en;
    GPPCU_STALL_GEN #(
        .NUMREG(32)
    ) GPPCU_STALL_GEN_inst
    (
        .iACLK(iACLK),
        .inRST(inRST),
        .iREGD(instr_fetch[INSTR_REGD_5+:5]),
        .iREGA(instr_fetch[INSTR_REGA_5+:5]),
        .iREGB(instr_fetch[INSTR_REGB_5+:5]),
        .iVALID_REGD(cw_valid_fetch & decoding_cw[CW_REGWR]),
        .iVALID_REGA(cw_valid_fetch & decoding_cw[CW_USEREGA]),
        .iVALID_REGB(cw_valid_fetch & decoding_cw[CW_USEREGB]),
        .oENABLED(reg_use_en),
        .iWRREG(instr_writeback[INSTR_REGD_5+:5]),
        .iWRREG_VALID(cw_valid_writeback & cw_writeback[CW_REGWR])
    ); 
    
    // Decoder
    GPPCU_INSTR_DEC GPPCU_INSTR_DEC_inst(
        .iOPC(instr_fetch[INSTR_OPR_5+:5]),
        .oCW(decoding_cw)
    );
     
    // Global memory interface
    assign oGMEM_CLK  = iACLK & cw_valid_fetch & decoding_cw[CW_GMEMRD];
    assign oGMEM_ADDR = instr_fetch[INSTR_IMM2_17+:17];
    
    // Thread instances
    wire[NUM_THREAD-1:0] thread_0_calc_delay; // Only first wire in vector will be used.
    wire[DBW-1:0] lmem_out_data_mux[NUM_THREAD-1:0];
    assign on_calculation_delay = thread_0_calc_delay[0];
    assign oLMEM_RDATA = lmem_out_data_mux[iLMEM_THREAD_SEL];
    
    generate
        genvar idx_thread;
        
        for(idx_thread = 0
            ; idx_thread < NUM_THREAD
            ; idx_thread = idx_thread + 1) 
        begin : THREADS
            // Chip selection        
            wire lmem_thread_sel = iLMEM_THREAD_SEL == idx_thread;
            
            // Thread instance.
            GPPCU_THREAD GPPCU_THREAD_inst( 
                .iACLK(iACLK),
                
                .iCW_DEC({cw_decode, cw_valid_decode}),
                .iCW_EXEC({cw_exec, cw_valid_exec}),
                .iCW_WB({cw_writeback, cw_valid_writeback}),
                
                .iINSTR_FCH(instr_fetch),
                .iINSTR_DEC(instr_decode),
                .iINSTR_EXEC(instr_exec),
                .iINSTR_WB(instr_writeback),
                
                .iLMEMCLK(iLMEM_CLK),
                .iLMEMSEL(lmem_thread_sel),
                .iLMEMWREN(iLMEM_WR),
                .iLMEMADDR(iLMEM_ADDR),
                .oLMEMRDATA(lmem_out_data_mux[idx_thread]),
                .iLMEMWDATA(iLMEMWDATA),
                
                .iGMEMDATA(iGMEM_WDATA),
                
                .oBUSY(thread_0_calc_delay[idx_thread]) // For multi-cycle fpu
            );
        end
    endgenerate
    
endmodule