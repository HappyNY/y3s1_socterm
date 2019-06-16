module GPPCU_ALU #(
    parameter
    BW = 32
)
(
    iA,
    iB,
    iC,
    iOP,
    
    oVCNZ,
    oQ
);
    `include "GPPCU_PARAMETERS.vh"

    // -- ports
    input   [BW-1:0]    iA;
    input   [BW-1:0]    iB;
    input               iC;
    input   [3:0]       iOP;
    
    output  [3:0]       oVCNZ;
    output  [BW-1:0]    oQ;
    
    // -- logics
endmodule