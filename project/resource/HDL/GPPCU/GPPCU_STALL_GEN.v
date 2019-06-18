module GPPCU_STALL_GEN #
(
    parameter
    NUMREG = 32
)
(
    iACLK,
    inRST,
    iREGD,
    iREGA,
    iREGB,
    iVALID_REGD,
    iVALID_REGA,
    iVALID_REGB,
    oENABLED,
    
    iWRREG,
    iWRREG_VALID
);
    `include "bit_fit.vh"
    localparam RBW = bit_fit(NUMREG-1);
    
    // -- ports
    input                   iACLK;
    input                   inRST;
    input   [RBW-1:0]       iREGD;
    input   [RBW-1:0]       iREGA;
    input   [RBW-1:0]       iREGB;
    input                   iVALID_REGD;
    input                   iVALID_REGA;
    input                   iVALID_REGB;
    output                  oENABLED;
    
    input   [RBW-1:0]       iWRREG;
    input                   iWRREG_VALID;
    
    // -- regs
    reg     [NUMREG-1:0]    occupied;
    wire    [NUMREG-1:0]    pending_occupy;
    wire    [NUMREG-1:0]    rq_mask_a, rq_mask_b; 
    wire    [NUMREG-1:0]    wr_mask;

    // -- logics
    // wr_mask enables synchronous access to written reg on it. ... @defered.
    assign oENABLED = !(|(occupied & ((rq_mask_a | rq_mask_b)/* & ~wr_mask*/)));
    
    // -- occupy logic
    generate 
        genvar i;
        for(i = 0; i < NUMREG; i = i + 1) begin : SELECT
            wire outsig = iWRREG_VALID & (i == iWRREG);
            wire insig  = oENABLED && iVALID_REGD && i == iREGD; 
            assign pending_occupy[i] = (occupied[i] | insig) & ~outsig;
            
            always @(posedge iACLK) begin
                if(~inRST)
                    occupied[i] = 0;
                else
                    occupied[i] = pending_occupy[i]; 
            end
            
            assign rq_mask_a[i] = (i == iREGA) & iVALID_REGA;
            assign rq_mask_b[i] = (i == iREGB) & iVALID_REGB;
            assign wr_mask[i]   = (i ==iWRREG) & iWRREG_VALID;
        end
    endgenerate
endmodule

//
// Testbench
module GPPCU_STALL_GEN_testbench;
    
    
endmodule