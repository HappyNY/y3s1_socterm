`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 2019/05/11 17:12:35
// Design Name: 
// Module Name: ENCODER_PARAM
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


module ENCODER_PARAM#
(
    parameter 
    EBW = 4
)
(
    INPUT,
    OUTPUT
); 
    localparam IBW = 1 << EBW;

    input  [IBW-1:0] INPUT;
    output [EBW-1:0] OUTPUT;
    
    reg[EBW-1:0] msb;
    assign OUTPUT = msb;
    always @(*) begin : ENCODE
        integer i;
        msb = 0;
        for(i = 0; i < IBW; i = i+1) begin
            if(!msb && INPUT[i])begin
                msb = i;
            end
        end
    end
     
endmodule
