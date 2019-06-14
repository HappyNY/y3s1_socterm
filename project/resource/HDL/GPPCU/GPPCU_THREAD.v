module GPPCU_THREAD ( 
    iACLK,
     
    iCW_DEC,
    iCW_EXEC,
    iCW_WB,
    
    iINSTR_FCH,
    iINSTR_DEC,
    iINSTR_EXEC,
    iINSTR_WB,
    
    iLMEMCLK,
    iLMEMSEL,
    iLMEMWREN,
    iLMEMADDR,
    oLMEMRDATA,
    iLMEMWDATA,
    iGMEMDATA,
    
    oBUSY // For multi-cycle fpu
);
    // 
    `include "GPPCU_PARAMETERS.vh"

    // -- ports
    input                   iACLK;
     
    input   [CW_BITS-1:0]   iCW_DEC;
    input   [CW_BITS-1:0]   iCW_EXEC;
    input   [CW_BITS-1:0]   iCW_WB;
    
    input   [DBW-1:0]       iINSTR_FCH;
    input   [DBW-1:0]       iINSTR_DEC;
    input   [DBW-1:0]       iINSTR_EXEC;
    input   [DBW-1:0]       iINSTR_WB;
    
    input                   iLMEMCLK;
    input                   iLMEMSEL;
    input                   iLMEMWREN;
    input   [ABW-1:0]       iLMEMADDR;
    output  [DBW-1:0]       oLMEMRDATA;
    input   [DBW-1:0]       iLMEMWDATA;
    input   [DBW-1:0]       iGMEMDATA;
    
    output                  oBUSY;
    
    // -- global
    wire cw_valid_dec   = iCW_DEC [0];
    wire cw_valid_exec  = iCW_EXEC[0];
    wire cw_valid_wb    = iCW_WB  [0];
    
    // uses pending sreg 
    wire exec_opr_valid;
    
    // -- Register bank
    wire [31:0] dec_reg_a, dec_reg_b;
    wire [31:0] wrbk_reg_d;
    
    GPPCU_THREAD_REGBANK GPPCU_THREAD_REGBANK_inst(
        .iACLK(iACLK),
        .iREGASEL(iINSTR_DEC[INSTR_REGA_5+:5]),
        .iREGBSEL(iINSTR_DEC[INSTR_REGB_5+:5]),
        .iREGDSEL(iINSTR_WB [INSTR_REGD_5+:5]), // FROM PIPE_WRBK
        .oREGA(dec_reg_a),
        .oREGB(dec_reg_b),
        .iREGD(wrbk_reg_d),
        .iWR(cw_valid_wb & iCW_WB[CW_REGWR])
    );
    
    // -- Operand B Mux
    reg  [31:0] dec_opr_b_mux;
    always @(*) begin
        case(iCW_DEC[CW_BSEL1:CW_BSEL0])
        2'd0: dec_opr_b_mux <= dec_reg_b;
        2'd1: dec_opr_b_mux <= dec_reg_b + iINSTR_DEC[INSTR_IMM0_7+:7];
        2'd2: dec_opr_b_mux <= iINSTR_DEC[INSTR_IMM1_12+:12];
        2'd3: dec_opr_b_mux <= iGMEMDATA;
        endcase
    end
    
    // -- Execute platform reg
    reg [31:0]  exec_oprand_a, exec_oprand_b;
    always @(posedge iACLK) exec_oprand_a   <= dec_reg_a;
    always @(posedge iACLK) exec_oprand_b   <= dec_opr_b_mux; 
    
    // This signal will be internally registered.
    wire [31:0] wrbk_locmem_dat;
    
    // -- Local memory
    DPRAM_PARAM #( 
        .DBW(32), 
        .DEPTH(2048)
    ) DPRAM_PARAM_local_memory
    (
        // For external interface
        .iPA_CLK(iLMEMCLK),
        .iPA_ADDR(iLMEMADDR),
        .iPA_WR(iLMEMWREN & iLMEMSEL), 
        .iPA_WDATA(iLMEMWDATA),
        .oPA_RDATA(oLMEMRDATA),    
        
        // For internal use
        .iPB_CLK(iACLK),
        .iPB_ADDR(exec_oprand_b),
        .iPB_WR(cw_valid_exec & iCW_EXEC[CW_LMEM_WR] & exec_opr_valid), 
        .iPB_WDATA(exec_oprand_a),
        .oPB_RDATA(wrbk_locmem_dat)
    ); 
    
    // -- Execution stage
    wire [4:0]  exec_s;
    // +exec_opr_valid
    // @todo. Add ALU, FPU, Condition verifier
    
    // -- Writeback platform reg
    reg [31:0]  wrbk_q; 
    // wire [31:0] wrbk_alu_q, wrbk_fpu_q ... delay mux to next pipeline, to reduce thruput...
    reg [ 4:0]  wrbk_sreg;
    reg         wrbk_opr_valid;
    
    // logics
    always @(posedge iACLK) begin
        wrbk_opr_valid  <= exec_opr_valid;
        wrbk_sreg       <= iINSTR_WB[INSTR_S] ? exec_s : wrbk_sreg;
    end
    assign wrbk_reg_d    = iCW_WB[CW_LMEM_RD] ? wrbk_locmem_dat : wrbk_q;
endmodule