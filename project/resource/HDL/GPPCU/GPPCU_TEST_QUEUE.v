// Test module which contains instruction queue
module GPPCU_TEST_QUEUE #(
    parameter
    QBW                 = 12,
    GBW                 = 10,
    NUM_THREAD          = 24
)
(
    iACLK               ,
    inRST               ,
    iCMD                ,
    iDATA               ,    
    oDATA               , 
    oDONE
);
    /*
    ABOUT CMD
        1       7       8       16 bits
        CLK     WPARAM  LPARAM  COMMAND
        
        WPARAM  NAME                LPARAM  COMMAND         iDATA       oDATA
        0       PUSH INSTRUCTION    X       X               INSTR
        1       READ LOCAL MEMORY   THREAD  ADDRESS         X           OUTPUT
        2       WRITE LOCAL MEMORY  THREAD  ADDRESS         INPUT       X
        3       SET GLOBAL MEMORY   X       ADDRESS         INPUT
        4       OPERATION COMMAND                           X           STATUS
            LPARAM  COMMAND             NAME
            0       1 = RUN, 0 = STOP   RUN OR STOP
            1       NUM CYCLES          SET CYCLES
            2       OFST PER CYCLE      SET OFST PER CYCLES
            3       X                   RESET PROGRAM
            
    INITIALIZE SEQUENCE
        WP  LP  CMD IDAT
        4   3   
        4   2   o   
        4   
            
    */

    // -- ports 
    input               iACLK;
    input               inRST;
    input   [31:0]      iCMD;
    input   [31:0]      iDATA;
    output 	[31:0]      oDATA; 
    output              oDONE;
    
    // details    
    reg     [31:0]      oDATA               ;
    
    // -- nets
    wire    [16:0]      oGMEM_ADDR          ;
    wire    [31:0]      iGMEM_WDATA         ;
    wire                oGMEM_CLK           ;
    
    // -- parameter
    localparam 
        OPR_INSTR = 0,
        OPR_RDL = 1,
        OPR_WRL = 2,
        OPR_WRG = 3,
        OPR_COMMAND = 4
    ;
    
    localparam 
        LPM_RUNSTOP = 0,
        LPM_NUMCYCLE = 1,
        LPM_SZPERCYCLE = 2,
        LPM_RESETPRG = 3
    ;
    // -- regs
    reg     [31:0]      cur_task_ofst;
    reg     [31:0]      sz_per_task;
    reg     [11:0]      num_cycles;
    reg     [11:0]      cur_cycle_idx;
    
    // -- routing
    wire                opclk   = iCMD[31];
    wire    [ 6:0]      wparam  = iCMD[30:24];
    wire    [ 7:0]      lparam  = iCMD[23:16];
    wire    [15:0]      command = iCMD[15:0];
    wire    [31:0]      data_out;
    
    wire                instr_ready;
    wire                instr_valid;
    wire                gppcu_idle; 
    
    // -- instruction queue  
    reg     [QBW-1:0]       pmem_end;
    reg     [QBW-1:0]       pmem_head;
    reg                     program_done;
    wire    [31:0]          program;
    wire    [31:0]          status;
    reg                     pmem_running; 
    
    assign oDONE            = program_done;
    assign status[31]       = pmem_running;
    assign status[30]       = program_done;
    assign status[29:24]    = sz_per_task   [0+:6];
    assign status[23:16]    = pmem_end      [0+:8];
    assign status[15: 8]    = pmem_head     [0+:8];
    assign status[ 7: 4]    = num_cycles    [0+:4];
    assign status[ 3: 0]    = cur_cycle_idx [0+:4];
    
    DPRAM_PARAM #( 
        .DBW         (32), 
        .DEPTH       (1 << QBW)
    ) DPRAM_PARAM_pmem
    (
        .iPA_CLK     (opclk),
        .iPA_ADDR    (pmem_end),
        .iPA_WR      (wparam == OPR_INSTR), 
        .iPA_WDATA   (iDATA),
        .oPA_RDATA   (),
        .iPB_CLK     (iACLK),
        .iPB_ADDR    (pmem_head),
        .iPB_WR      (0), 
        .iPB_WDATA   (0),
        .oPB_RDATA   (program)
    ); 
    
    // -- program logic
    // On program
    always @(posedge opclk or negedge inRST) begin
        if(~inRST) begin
            pmem_end <= 0;
            pmem_running <= 0;
        end
        else if(wparam == OPR_INSTR) begin
            if(~pmem_running) begin
                pmem_end <= pmem_end + 1;
            end
        end
        else if(wparam == OPR_COMMAND) begin
            case (lparam)
                LPM_RUNSTOP: begin
                    pmem_running <= command[0];
                end
                LPM_RESETPRG: begin
                    pmem_end <= 0;
                    pmem_running <= 0;  
                end
                LPM_NUMCYCLE: begin
                    num_cycles <= command;
                end
                LPM_SZPERCYCLE: begin
                    sz_per_task <= command;
                end
            endcase
        end
    end
    
    // -- instruction supply logic
    always @(posedge iACLK) begin
        if(pmem_running & inRST) begin
            if(cur_cycle_idx == num_cycles) begin
                program_done <= pmem_running;
                // to generate instr_invalid signal.
                pmem_head <= pmem_end;
            end
            else if(pmem_head < pmem_end) begin
                if(instr_ready) begin
                    pmem_head <= pmem_head + 1;
                end 
            end
            else begin
                pmem_head <= 0; 
                cur_task_ofst <= cur_task_ofst + sz_per_task;
                cur_cycle_idx <= cur_cycle_idx + 1;
            end
        end
        else begin
            program_done    <= 0;
            pmem_head       <= 0;
            cur_task_ofst   <= 0;
            cur_cycle_idx   <= 0;
        end
    end
    
    wire    [31:0]      gmem_dat;
    DPRAM_PARAM #( 
        .DBW         (32), 
        .DEPTH       (1 << GBW)
    ) DPRAM_PARAM_cmem
    (
        .iPA_CLK     (opclk),
        .iPA_ADDR    (command),
        .iPA_WR      (wparam == OPR_WRG), 
        .iPA_WDATA   (iDATA),
        .oPA_RDATA   (),
        .iPB_CLK     (oGMEM_CLK),
        .iPB_ADDR    (oGMEM_ADDR),
        .iPB_WR      (0), 
        .iPB_WDATA   (0),
        .oPB_RDATA   (gmem_dat)
    ); 
    
    assign iGMEM_WDATA = oGMEM_ADDR == 0 ? cur_task_ofst : gmem_dat;
    assign instr_valid = pmem_running && !program_done && pmem_head < pmem_end;
    always oDATA <= wparam == OPR_COMMAND ? status : data_out;
    
    // -- core
    GPPCU_CORE # (
        .NUM_THREAD (NUM_THREAD),
        .WORD_BW    (9)
    ) GPPCU_CORE_inst
    (
        .iACLK               (iACLK),          // Instruction should be valid on ACLK's rising edge.
        .inRST               (inRST),
        .oIDLING             (gppcu_idle),          // Doing nothing.
        .iINSTR              (program),
        .iINSTR_VALID        (instr_valid),
        .oINSTR_READY        (instr_ready),   // Replaces instruction address. 
        .iLMEM_CLK           (opclk),
        .iLMEM_THREAD_SEL    (lparam), // Thread selection input
        .iLMEM_ADDR          (command),
        .iLMEM_WDATA         (iDATA),
        .oLMEM_RDATA         (data_out),
        .iLMEM_RD            (wparam == OPR_RDL),
        .iLMEM_WR            (wparam == OPR_WRL),
        .oGMEM_ADDR          (oGMEM_ADDR),
        .iGMEM_WDATA         (iGMEM_WDATA),
        .oGMEM_CLK           (oGMEM_CLK)
    );
endmodule


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
// TESTBENCH
///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
`timescale 1ns/1ns
module GPPCU_TEST_QUEUE_testbench;
    `include "GPPCU_PARAMETERS.vh"
    
    // -- ports 
    reg               iACLK = 0;
    reg               inRST;
    wire  [31:0]      iCMD;
    reg   [31:0]      iDATA;
    wire  [31:0]      oDATA; 
    wire              oDONE; 
    
    GPPCU_TEST_QUEUE #( 
        .QBW                 ( 10 ),
        .NUM_THREAD          ( 2  )
    ) GPPCU_TEST_QUEUE_testinst
    (
        .iACLK               (iACLK),
        .inRST               (inRST),
        .iCMD                (iCMD),
        .iDATA               (iDATA),    
        .oDATA               (oDATA), 
        .oDONE               (oDONE)
    );
    
    always #10 iACLK = ~iACLK;
    reg opclk;
    reg [6:0] wp;
    reg [7:0] lp;
    reg [15:0] cmd;
    
    assign iCMD = { opclk, wp, lp, cmd };
    integer i;
    integer j;
    initial begin
        #10 inRST = 0;
        #10 inRST = 1;
        
        opclk = 0;
        
        // set initial data 
        for(i = 0; i < 2; i = i + 1) begin 
            wp = 2;
            lp = i;
            for(j = 0; j < 16; j = j + 1) begin
                cmd = j;
                iDATA = j == 0 ? i + 'h1000 : j;
                #5 opclk = 1;
                #5 opclk = 0;
            end
        end
        
        // set instr .. ld 0, lsl 0, st 1
        #100;
        cmd = 0;
        wp = 0;
        lp = 0;
        
        iDATA = {COND_ALWAYS,   MVI,    1'b0,   5'd0,   17'd4 };                #5 opclk = 1; #5 opclk = 0;
        // iDATA = {COND_ALWAYS,   LDL,    1'b0,   5'd1,   12'h0,          5'd0 }; #5 opclk = 1; #5 opclk = 0;
        iDATA = {COND_ALWAYS,   MVI,    1'b0,   5'd1,   17'd11 };                #5 opclk = 1; #5 opclk = 0;
        
        iDATA = {COND_ALWAYS,   LSL,    1'b0,   5'd1,   5'd1,   12'd0 };        #5 opclk = 1; #5 opclk = 0;
        iDATA = {COND_ALWAYS,   STL,    1'b0,   5'h0,   5'd1,   7'd3,   5'd0 }; #5 opclk = 1; #5 opclk = 0;
        
        iDATA = {COND_ALWAYS,   LSL,    1'b0,   5'd1,   5'd1,   12'd0 };        #5 opclk = 1; #5 opclk = 0;
        iDATA = {COND_ALWAYS,   STL,    1'b0,   5'h0,   5'd1,   7'd4,   5'd0 }; #5 opclk = 1; #5 opclk = 0;
        
        iDATA = {COND_ALWAYS,   LSL,    1'b0,   5'd1,   5'd1,   12'd0 };        #5 opclk = 1; #5 opclk = 0;
        iDATA = {COND_ALWAYS,   STL,    1'b0,   5'h0,   5'd1,   7'd5,   5'd0 }; #5 opclk = 1; #5 opclk = 0;
        
        iDATA = {COND_ALWAYS,   LSL,    1'b0,   5'd1,   5'd1,   12'd0 };        #5 opclk = 1; #5 opclk = 0;
        iDATA = {COND_ALWAYS,   STL,    1'b0,   5'h0,   5'd1,   7'd6,   5'd0 }; #5 opclk = 1; #5 opclk = 0;
        
        iDATA = {COND_ALWAYS,   LDL,    1'b0,   5'd5,   5'h0,   7'd6,   5'd0 }; #5 opclk = 1; #5 opclk = 0;
    end
    
endmodule