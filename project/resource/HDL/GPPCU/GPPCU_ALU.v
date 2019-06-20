module GPPCU_ALU #(
    parameter
    BW = 32
)
(
    iA,
    iB,
    iC,
    iOP,
    
    oV,
    oC,
    oN,
    oZ,
    oQ
);
    `include "GPPCU_PARAMETERS.vh"

    // -- ports
    input   [BW-1:0]    iA;
    input   [BW-1:0]    iB;
    input               iC;
    input   [3:0]       iOP;
     
    output  [BW-1:0]    oQ;
    output              oV;
    output              oC;
    output              oN;
    output              oZ;
    
    // details
    reg     [BW-1:0]    oQ;
    reg                 oC;
    
    // -- logics
    assign oV = ~(iA[BW-1] ^ iB[BW-1]) & (iA[BW-1] ^ oQ[BW-1]);
    assign oN = oQ[BW-1];
    assign oZ = oQ == 0;
    
    // @see GPPCU_PARAMETERS.vh
    always @(*) case (iOP)
        NOP: {oC, oQ} <= {1'b0,    {BW{1'b0}}};
        MOV: {oC, oQ} <= {1'b0,     iB};
        MVN: {oC, oQ} <= {1'b1,     ~iB};
        ADC: {oC, oQ} <= iA + iB + iC;
        SBC: {oC, oQ} <= iA - iB - iC;
        AND: {oC, oQ} <= {1'b0,     iA & iB};
        ORR: {oC, oQ} <= {1'b0,     iA | iB};
        XOR: {oC, oQ} <= {1'b0,     iA ^ iB};
        ADI: {oC, oQ} <= iA + iB;
        SBI: {oC, oQ} <= iA - iB;
        MVI: {oC, oQ} <= iB;
        LSL: {oC, oQ} <= {iA[BW-2:0],   1'b0};
        LSR: {oC, oQ} <= {1'b0,         iA[BW-1:1]};
        ASR: {oC, oQ} <= {iA[BW-1],     iA[BW-1:1]};
    endcase
    
endmodule