// Test module which contains instruction queue
module GPPCU_TEST_QUEUE #(
    parameter
    QBW                 = 10
)
(
    iACLK               ,
    inRST               ,
    iCMD                ,
    iDATA               ,    
    oDATA               ,
    oFULL               ,
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
        4       READ STATUS                                 X           [0] IS DONE?
    */

    // -- ports 
    input               iACLK;
    input               inRST;
    input   [31:0]      iCMD;
    input   [31:0]      iDATA;
    output 	[31:0]      oDATA;
    output              oFULL;
    output              oDONE;
    
    // -- regs    
    wire    [16:0]      oGMEM_ADDR          ;
    reg     [31:0]      iGMEM_WDATA         ;
    wire                oGMEM_CLK           ;
    reg     [31:0]      oDATA               ;
    
    // -- parameter
    localparam 
        OPR_INSTR = 0,
        OPR_RDL = 1,
        OPR_WRL = 2,
        OPR_WRG = 3,
        OPR_STAT = 4
    ;
    
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
    reg     [QBW-1:0]      queue_head;
    reg     [QBW-1:0]      queue_tail;
    wire    [31:0]      queue_dat;
    DPRAM_PARAM #( 
        .DBW         (32), 
        .DEPTH       (1 << QBW)
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
    
    // -- output data logic
    wire [15:0] upper = queue_head;
    wire [15:0] lower = queue_tail;
    
    always @(posedge opclk) case(wparam) 
        OPR_STAT: oDATA <= {upper, lower};
        default : oDATA <= data_out;
    endcase
    
    assign  instr_valid = queue_head != queue_tail;
    assign  oFULL = queue_head == queue_tail - 1;
    assign  oDONE = queue_head == queue_tail && gppcu_idle;
    
    // adjust head
    always @(posedge opclk or negedge inRST) begin
        if(~inRST) begin
            queue_head <= 0;
        end
        else if(wparam == OPR_INSTR) begin 
            queue_head <= queue_head + 1;
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
    
    // @todo. Resolve invalid data read/write operation for thread 0 and 1.
    // -- core
    GPPCU_CORE # (
        .NUM_THREAD (24),
        .WORD_BW    (9)
    )
    (
        .iACLK               (iACLK),          // Instruction should be valid on ACLK's rising edge.
        .inRST               (inRST),
        .oIDLING             (gppcu_idle),          // Doing nothing.
        .iINSTR              (queue_dat),
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


module GPPCU_TEST_QUEUE_testbench;
    `include "GPPCU_PARAMETERS.vh"
    
    // -- ports 
    reg               iACLK;
    reg               inRST;
    reg   [31:0]      iCMD;
    reg   [31:0]      iDATA;
    wire  [31:0]      oDATA;
    wire              oFULL;
    wire              oDONE; 
    
    GPPCU_TEST_QUEUE #( 
        QBW                 ( 10 )
    ) GPPCU_TEST_QUEUE_testinst
    (
        .iACLK               (iACLK),
        .inRST               (inRST),
        .iCMD                (iCMD),
        .iDATA               (iDATA),    
        .oDATA               (oDATA),
        .oFULL               (oFULL),
        .oDONE               (oDONE)
    );
    
    
endmodule