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
    localparam ABW = bit_fit(NUMREG);
    
    // ports
    input                   iACLK;
    input                   inRST;
    input                   iREGD;
    input   [ABW-1:0]       iREGA;
    input   [ABW-1:0]       iREGB;
    input                   iVALID_REGD;
    input                   iVALID_REGA;
    input                   iVALID_REGB;
    output                  oENABLED;
    
    input   [ABW-1:0]       iWRREG;
    input                   iWRREG_VALID;
    
    // regs
    reg     [NUMREG-1:0]    occupied;
    wire    [NUMREG-1:0]    rq_mask_a, rq_mask_b; 
    
    // logics
    assign oENABLED = !(|(occupied & (rq_mask_a | rq_mask_b)));
    generate 
        genvar i;
        for(i = 0; i < NUMREG; i = i + 1) begin : SELECT
            wire outsig = ~iWRREG_VALID | (i == iWRREG);
            wire insig  = oENABLED && iVALID_REGD && i == iREGD; 
            always @(posedge iACLK) begin
                if(~inRST)
                    occupied[i] = 0;
                else
                    occupied[i] = (occupied[i] & outsig) | insig; 
            end
            
            assign rq_mask_a[i] = (i == iREGA) & iVALID_REGA;
            assign rq_mask_b[i] = (i == iREGB) & iVALID_REGB;
        end
    endgenerate
endmodule

//
// Testbench
module GPPCU_STALL_GEN_testbench;
    
    
endmodule