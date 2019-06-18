module GPPCU_COND_VERIFIER
(
    iCOND,
    iSREG,
    
    oVALID
);
    `include "GPPCU_PARAMETERS.vh"
    
    // -- ports
    input   [3:0]   iCOND;
    input   [4:0]   iSREG;
    
    output          oVALID;
    
    // details
    reg             oVALID;
    
    // -- logics
    wire sv = iSREG[SREG_V];
    wire sc = iSREG[SREG_C];
    wire sn = iSREG[SREG_N];
    wire sz = iSREG[SREG_Z];

    always @(*) case (iCOND)
        COND_ALWAYS : oVALID <= 1;
        COND_NEVER  : oVALID <= 0;
        COND_C      : oVALID <= sc;
        COND_NC     : oVALID <= ~sc;
        COND_Z      : oVALID <= sz;
        COND_NZ     : oVALID <= ~sz;
        COND_V      : oVALID <= sv;
        COND_NV     : oVALID <= ~sv;
        COND_N      : oVALID <= sn;
        COND_NN     : oVALID <= ~sn;
        COND_NEG    : oVALID <= (~sv &  sn) | (sv &  sc);
        COND_POS    : oVALID <= (~sv & ~sn) | (sv & ~sc);
        COND_EQ     : oVALID <= (~sc &  sz);
        // Same as POS ...
        COND_GR     : oVALID <= (~sv & ~sn) | (sv & ~sc);
        // Same as NEG ..
        COND_LT     : oVALID <= (~sv &  sn) | (sv &  sc);
    endcase
        
endmodule