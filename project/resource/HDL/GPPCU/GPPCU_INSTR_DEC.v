// Run as combination circuit
module GPPCU_INSTR_DEC (
    iOPC,
    oCW
);
    // params
    `include "GPPCU_PARAMETERS.vh"
    
    // ports
    input   [4:0]           iOPC;
    output  [CW_BITS-1:1]   oCW;

    // port details
    reg     [CW_BITS-1:1]   oCW;
    
    // localparam.. instr mappings
    localparam[4:0]
        NOP     =  0, 
        MOV     =  1,
        MVN     =  2,
        ADC     =  3,
        SBC     =  4,
        AND     =  5,
        ORR     =  6,
        XOR     =  7,
        ADI     =  8,
        SBI     =  9,
        MVI     = 10,
        
        ITOF    = 11,
        FTOI    = 12,
        FMUL    = 13,
        FDIV    = 14,
        FADD    = 15,
        FSUB    = 16,
        FNEG    = 17,
        FSQRT   = 18,
        
        LDL     = 19,
        LDCI    = 20,
        STL     = 21
    ; 
        
    
    // logic
    always @(iOPC) begin /*
                           U U     L L                                      
                           S S     M M F    A    B  G                       
                           E E     E E P    L    S  M R                       
                           R R F A M M O    O    E  E E                         
                           E E P L | | P    P    L  M G                        
                           G G O O R W C    C       R W                        
                           A B P P D R 3..0 3..0 1. D R  */ case (iOPC)
        NOP   : oCW <= 'b__0_0_0_0_0_0_0000_0000_00_0_0;
        MOV   : oCW <= 'b__0_0_0_0_0_0_0000_0000_00_0_0;
        MVN   : oCW <= 'b__0_0_0_0_0_0_0000_0000_00_0_0;
        ADC   : oCW <= 'b__0_0_0_0_0_0_0000_0000_00_0_0;
        SBC   : oCW <= 'b__0_0_0_0_0_0_0000_0000_00_0_0;
        AND   : oCW <= 'b__0_0_0_0_0_0_0000_0000_00_0_0;
        ORR   : oCW <= 'b__0_0_0_0_0_0_0000_0000_00_0_0;
        XOR   : oCW <= 'b__0_0_0_0_0_0_0000_0000_00_0_0;
        ADI   : oCW <= 'b__0_0_0_0_0_0_0000_0000_00_0_0;
        SBI   : oCW <= 'b__0_0_0_0_0_0_0000_0000_00_0_0;
        MVI   : oCW <= 'b__0_0_0_0_0_0_0000_0000_00_0_0;
        ITOF  : oCW <= 'b__0_0_0_0_0_0_0000_0000_00_0_0;
        FTOI  : oCW <= 'b__0_0_0_0_0_0_0000_0000_00_0_0;
        FMUL  : oCW <= 'b__0_0_0_0_0_0_0000_0000_00_0_0;
        FDIV  : oCW <= 'b__0_0_0_0_0_0_0000_0000_00_0_0;
        FADD  : oCW <= 'b__0_0_0_0_0_0_0000_0000_00_0_0;
        FSUB  : oCW <= 'b__0_0_0_0_0_0_0000_0000_00_0_0;
        FNEG  : oCW <= 'b__0_0_0_0_0_0_0000_0000_00_0_0;
        FSQRT : oCW <= 'b__0_0_0_0_0_0_0000_0000_00_0_0;
        LDL   : oCW <= 'b__0_0_0_0_0_0_0000_0000_00_0_0;
        LDCI  : oCW <= 'b__0_0_0_0_0_0_0000_0000_00_0_0;
        STL   : oCW <= 'b__0_0_0_0_0_0_0000_0000_00_0_0;
        default: oCW <= 0;
        endcase 
    end
endmodule