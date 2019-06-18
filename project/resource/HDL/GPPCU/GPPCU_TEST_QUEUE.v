// Test module which contains instruction queue
module GPPCU_TEST_QUEUE(
    iACLK               ,
    inRST               ,
    iCMD                ,
    iDATA               ,    
    oDATA
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
    */

    // -- ports 
    input               iACLK;
    input               inRST;
    input   [31:0]      iCMD;
    input   [31:0]      iDATA;
    output 	[31:0]      oDATA;
    
    // -- regs    
    wire    [16:0]      oGMEM_ADDR          ;
    reg     [31:0]      iGMEM_WDATA         ;
    wire                oGMEM_CLK           ;
    
    // -- parameter
    localparam 
        OPR_INSTR = 0,
        OPR_RDL = 1,
        OPR_WRL = 2,
        OPR_WRG = 3
    ;
    // -- routing
    wire                opclk = iCMD[31];
    wire    [ 6:0]      wparam = iCMD[30:24];
    wire    [ 7:0]      lparam = iCMD[23:16];
    wire    [15:0]      command = iCMD[15:0];
    
    wire                instr_ready;
    wire                instr_valid;
    
    // -- instruction queue    
    reg     [ 7:0]      queue_head;
    reg     [ 7:0]      queue_tail;
    wire    [31:0]      queue_dat;
    DPRAM_PARAM #( 
        .DBW         (32), 
        .DEPTH       (128)
    ) DPRAM_PARAM_queue
    (
        .iPA_CLK     (opclk),
        .iPA_ADDR    (queue_head),
        .iPA_WR      (wparam == OPR_INSTR), 
        .iPA_WDATA   (iDATA),
        .oPA_RDATA   (),
        .iPB_CLK     (iACLK),
        .iPB_ADDR    (queue_tail),
        .iPB_WR      (0), 
        .iPB_WDATA   (0),
        .oPB_RDATA   (queue_dat)
    ); 
    
    assign  instr_valid = queue_head != queue_tail;
    
    // adjust head
    always @(posedge opclk or negedge inRST) begin
        if(~inRST) begin
            queue_head <= 0;
        end
        else if(wparam == OPR_INSTR) begin 
            queue_head          <= queue_head + 1;
        end
    end
    
    // adjust tail
    always @(posedge iACLK) begin
        if(~inRST) begin
            queue_tail <= 0;
        end
        else begin
            if(instr_ready && instr_valid) begin
                queue_tail <= queue_tail + 1;
            end
        end
    end
    
    // -- core
    GPPCU_CORE # (
        .NUM_THREAD (16)
    )
    (
        .iACLK               (iACLK),          // Instruction should be valid on ACLK's rising edge.
        .inRST               (inRST),
        .iINSTR              (queue_dat),
        .iINSTR_VALID        (instr_valid),
        .oINSTR_READY        (instr_ready),   // Replaces instruction address. 
        .iLMEM_CLK           (opclk),
        .iLMEM_THREAD_SEL    (lparam), // Thread selection input
        .iLMEM_ADDR          (command),
        .iLMEM_WDATA         (iDATA),
        .oLMEM_RDATA         (oDATA),
        .iLMEM_RD            (wparam == OPR_WRL),
        .iLMEM_WR            (wparam == OPR_RDL),
        .oGMEM_ADDR          (oGMEM_ADDR),
        .iGMEM_WDATA         (iGMEM_WDATA),
        .oGMEM_CLK           (oGMEM_CLK)
    );
endmodule