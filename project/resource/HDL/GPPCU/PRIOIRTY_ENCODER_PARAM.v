`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 2019/05/16 13:22:55
// Design Name: 
// Module Name: PRIOIRTY_ENCODER_PARAM
// Project Name: 
// Target Devices: 
// Tool Versions: 
// Description: 
// 
// Dependencies: 
// 
// Revision:
// Revision 0.01 - File Created
// Additional Comments:
// 
////////////////////////////////////////////////////////////////////////////////// 
module PRIORITY_ENCODER_PARAM# (
    parameter 
    EBW = 4 // Encoder bitwidth
)
(
    INPUT,
    OUTPUT
);
    //////////////////////////////////////////////////
    // Constants
    //////////////////////////////////////////////////
    localparam DECBW =  1 << EBW;
    
    //////////////////////////////////////////////////
    // Ports
    //////////////////////////////////////////////////
    input  [DECBW-1:0] INPUT;
    output [EBW  -1:0] OUTPUT;
        
    //////////////////////////////////////////////////
    // Locals
    //////////////////////////////////////////////////
    
    //////////////////////////////////////////////////
    // Logic
    //////////////////////////////////////////////////
    wire [DECBW-1:0] actual_input;
    tri  [EBW  -1:0] result;
    
    generate 
        genvar c;
        
        for(c = 0; c < DECBW-2; c = c + 1) begin : MASK_GEN
            assign actual_input[c] = INPUT[c] & ~|INPUT[DECBW-1:c+1];
            assign result = actual_input[c] ? c : {EBW{1'bz}};
        end
        assign actual_input[DECBW-1] = INPUT[DECBW-1];
        assign OUTPUT = result;
    endgenerate
    //////////////////////////////////////////////////
endmodule

//////////////////////////////////////////////////////
// Testbench
//////////////////////////////////////////////////////
`timescale 1ns/1ns
module PRIORITY_ENCODER_PARAM_testbench;
    
    localparam EBW = 4;
    localparam DECBW = 1<<EBW;
    reg[DECBW-1:0] INPUT = 3;
    wire[EBW-1:0] OUTPUT;
    PRIORITY_ENCODER_PARAM# (
        EBW  // Encoder bitwidth
    ) PRIOIRTY_ENCODER_PARAM_testbench
    (
        .INPUT(INPUT),
        .OUTPUT(OUTPUT)
    );
    
    always @(*) #30 INPUT = INPUT + 1;
endmodule
