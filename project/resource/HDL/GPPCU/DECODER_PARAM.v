`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 2019/05/11 17:12:35
// Design Name: 
// Module Name: DECODER_PARAM
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


module DECODER_PARAM#
(
    parameter 
    EBW = 4
)
(
    INPUT,
    OUTPUT
);
    localparam IBW = 1 << EBW;
    
    input  [EBW-1:0] INPUT;
    output [IBW-1:0] OUTPUT;
    
    generate
        genvar i;
        for (i = 0; i < IBW; i = i + 1) begin : DECODE
            assign OUTPUT[i] = i == INPUT;
        end
    endgenerate    
endmodule
