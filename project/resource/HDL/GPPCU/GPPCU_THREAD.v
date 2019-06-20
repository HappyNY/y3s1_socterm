module GPPCU_THREAD # (
    parameter 
    WORD_BITS   = 10
)
( 
    iACLK,
    inRST,
     
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
    input                   inRST;
    
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
    wire exec_cond_verified;
    reg  wrbk_cond_verified; 
    
    // -- Register bank
    wire [31:0] dec_reg_a, dec_reg_b;
    wire [31:0] wrbk_reg_d;
    
    GPPCU_THREAD_REGBANK GPPCU_THREAD_REGBANK_inst(
        .iACLK      (iACLK),
        .iREGASEL   (iINSTR_DEC[INSTR_REGA_5+:5]),
        .iREGBSEL   (iINSTR_DEC[INSTR_REGB_5+:5]),
        .iREGDSEL   (iINSTR_WB [INSTR_REGD_5+:5]), // FROM PIPE_WRBK
        .oREGA      (dec_reg_a),
        .oREGB      (dec_reg_b),
        .iREGD      (wrbk_reg_d),
        .iWR        (cw_valid_wb & iCW_WB[CW_REGWR] & wrbk_cond_verified)
    );
    
    // -- Operand B Mux
    reg  [31:0] dec_opr_b_mux;
    always @(*) begin
        case(iCW_DEC[CW_BSEL1:CW_BSEL0])
        2'd0: dec_opr_b_mux <= dec_reg_b + iINSTR_DEC[INSTR_IMM0_7+:7];
        2'd1: dec_opr_b_mux <= iINSTR_DEC[INSTR_IMM1_12+:12];
        2'd2: dec_opr_b_mux <= iINSTR_DEC[INSTR_IMM2_17+:17];
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
        .DEPTH(1 << WORD_BITS)
    ) DPRAM_PARAM_local_memory
    (
        // For external interface
        .iPA_CLK    (iLMEMCLK),
        .iPA_ADDR   (iLMEMADDR),
        .iPA_WR     (iLMEMWREN & iLMEMSEL), 
        .iPA_WDATA  (iLMEMWDATA),
        .oPA_RDATA  (oLMEMRDATA),    
        
        // For internal use
        .iPB_CLK    (iACLK),
        .iPB_ADDR   (exec_oprand_b),
        .iPB_WR     (cw_valid_exec & iCW_EXEC[CW_LMEM_WR] & exec_cond_verified), 
        .iPB_WDATA  (exec_oprand_a),
        .oPB_RDATA  (wrbk_locmem_dat) // Data read performs unconditionally.
    ); 
    
    // -- Execution stage 
    reg  [ 4:0] exec_sreg = 0;
    reg  [31:0] wrbk_alu_q = 0, wrbk_fpu_q = 0;  
    wire [31:0] alu_q, fpu_q;
    wire [ 4:0] alu_sreg, fpu_sreg/*No valid status register.*/;
    assign fpu_sreg = 0;
    wire [ 4:0] pending_sreg = iCW_EXEC[CW_FPOP] ? fpu_sreg : alu_sreg; 
    
    // Unconditionally load to platform.
    // Processings related to pipeline stall only depends on 'valid' bit,
    //which is managed from outside of this module.
    always @(posedge iACLK) begin
        wrbk_alu_q <= alu_q; 
        // wrbk_fpu_q is assigned from below.
        exec_sreg  <= ~inRST ? 0 : cw_valid_exec & exec_cond_verified & iINSTR_EXEC[INSTR_S] ? pending_sreg : exec_sreg;
    end
    
    // Single-cycle operation.
    GPPCU_ALU #( 
        .BW     (32)
    ) GPPCU_ALU_inst
    (
        .iA     (exec_oprand_a),
        .iB     (exec_oprand_b),
        .iC     (exec_sreg[SREG_C]),
        .iOP    (iCW_EXEC[CW_ALOPC0+:4]),
        
        .oV     (alu_sreg[SREG_V]),
        .oC     (alu_sreg[SREG_C]),
        .oN     (alu_sreg[SREG_N]),
        .oZ     (alu_sreg[SREG_Z]),
        .oQ     (alu_q)
    );
    
    // -- FPU Drive logic
    wire        fp_busy;
    reg         fp_start;
    wire        fp_done;
    reg[1:0]    fp_stage = 0;
    reg[31:0]   fpu_da, fpu_db;
    localparam [1:0]
        FP_IDLE     = 0, 
        FP_RUN      = 1,
        FP_BUSY     = 2,
        FP_DONE     = 3
    ;
    
    // @todo. Condtiion verifier logic
    // @ Generates enable/disable signal 
    GPPCU_COND_VERIFIER GPPCU_COND_VERIFIER_inst(
        .iCOND      (iINSTR_EXEC[INSTR_COND_4+:4]),
        .iSREG      (exec_sreg),
        .oVALID     (exec_cond_verified)
    );
    
    // Stall signal resolves automatically when the stage arrives DONE.
    assign fp_busy = cw_valid_exec && fp_stage != FP_DONE && iCW_EXEC[CW_FPOP] && fp_stage != FP_DILE; 
    // No reset logic. Always wait for calculation done
    always @(posedge iACLK) 
    if(~inRST)
        fp_stage <= 0;
    else begin
        case(fp_stage)
            FP_IDLE: begin
                fp_stage    <= cw_valid_exec & iCW_EXEC[CW_FPOP] ? FP_RUN : FP_IDLE;
                fpu_da      <= exec_oprand_a;
                fpu_db      <= exec_oprand_b;
            end 
            FP_RUN: begin
                fp_stage    <= FP_BUSY; 
                fp_start    <= 1'b1;
            end
            FP_BUSY: begin
                fp_stage    <= fp_done ? FP_DONE : FP_BUSY;
                wrbk_fpu_q  <= fp_done ? fpu_q   : wrbk_fpu_q ;
                fp_start    <= 1'b0;
            end
            FP_DONE: begin
                fp_stage    <= FP_IDLE;
                fp_start    <= 1'b0;
            end
        endcase
    end
    // Platform dependent module.
    // Uses altera's multicycled floating point unit.
    // To adopt multistage FPU on it, core should also be modified.
    // @Composite state machine which drives this fpu module
    assign oBUSY = fp_busy;
    //* ##NOTICE## DISABLE UNTIL THE PIPELINE LOGIC VERIFIED!!!! TO MUCH TIME COST ...
    GPPCU_MC_FPU GPPCU_MC_FPU_inst(
        .clk        (iACLK),
        .clk_en     (1'b1),
        .dataa      (fpu_da),
        .datab      (fpu_db),
        .n          (iCW_EXEC[CW_FPOPC0+:3]),
        .reset      (0),
        .reset_req  (0),
        .start      (fp_start),
        .done       (fp_done),
        .result     (fpu_q)
    ); /*/
    assign fp_done = 1;
    // */
    
    // -- Writeback platform reg
    wire [31:0]  wrbk_q; 
    assign wrbk_q               = iCW_WB[CW_FPOP] ? wrbk_fpu_q : wrbk_alu_q; 
    assign wrbk_reg_d           = iCW_WB[CW_LMEM_RD] ? wrbk_locmem_dat : wrbk_q;
    
    // logics
    always @(posedge iACLK) wrbk_cond_verified      <= exec_cond_verified;
    
endmodule
