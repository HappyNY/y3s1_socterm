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
    /*NOP*/ 4'b0000: {oC, oQ} <= {1'b0,    {BW{1'b0}}};
    /*MOV*/ 4'b0001: {oC, oQ} <= {1'b0,     iB};
    /*MVN*/ 4'b0010: {oC, oQ} <= {1'b1,     ~iB};
    /*ADC*/ 4'b0011: {oC, oQ} <= iA + iB + iC;
    /*SBC*/ 4'b0100: {oC, oQ} <= iA - iB - iC;
    /*AND*/ 4'b0101: {oC, oQ} <= {1'b0,     iA & iB};
    /*ORR*/ 4'b0110: {oC, oQ} <= {1'b0,     iA | iB};
    /*XOR*/ 4'b0111: {oC, oQ} <= {1'b0,     iA ^ iB};
    /*ADI*/ 4'b1000: {oC, oQ} <= iA + iB;
    /*SBI*/ 4'b1001: {oC, oQ} <= iA - iB;
    /*MVI*/ 4'b1010: {oC, oQ} <= iB;
    /*LSL*/ 4'b1011: {oC, oQ} <= {iA[BW-2:0],   1'b0};
    /*LSR*/ 4'b1100: {oC, oQ} <= {1'b0,         iA[BW-1:1]};
    /*ASR*/ 4'b1101: {oC, oQ} <= {iA[BW-1],     iA[BW-1:1]};
    endcase
    
endmodule