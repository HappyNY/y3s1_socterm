module GPPCU_THREAD ( 
    iACLK,
     
    iCW_DEC,
    iCW_EXEC,
    iCW_WB,
    
    iINSTR_FCH,
    iINSTR_DEC,
    iINSTR_EXEC,
    iINSTR_WB,
    
    iLMEMCLK,
    iLMEMSEL,
    iLMEMWREN,
    iLMEMADDR,
    oLMEMRDATA,
    iLMEMWDATA,
    iGMEMDATA,
    
    oBUSY // For multi-cycle fpu
);
    // 
    `include "GPPCU_PARAMETERS.vh"

    // ports
    input                   iACLK;
     
    input   [CW_BITS-1:0]   iCW_DEC;
    input   [CW_BITS-1:0]   iCW_EXEC;
    input   [CW_BITS-1:0]   iCW_WB;
    
    input   [DBW-1:0]       iINSTR_FCH;
    input   [DBW-1:0]       iINSTR_DEC;
    input   [DBW-1:0]       iINSTR_EXEC;
    input   [DBW-1:0]       iINSTR_WB;
    
    input                   iLMEMCLK;
    input                   iLMEMSEL;
    input                   iLMEMWREN;
    input   [ABW-1:0]       iLMEMADDR;
    output  [DBW-1:0]       oLMEMRDATA;
    input   [DBW-1:0]       iLMEMWDATA;
    input   [DBW-1:0]       iGMEMDATA;
    
    output                  oBUSY;
    
    // Register bank
    GPPCU_THREAD_REGBANK GPPCU_THREAD_REGBANK_inst(
        .iACLK(iACLK),
        .iREGASEL(),
        .iREGBSEL(),
        .iREGDSEL(), // FROM PIPE_WRBK
        .oREGA(),
        .oREGB(),
        .iREGD(),
        .iWR()
    );
    
endmodule