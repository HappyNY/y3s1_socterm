module GPPCU_THREAD_REGBANK(
    iACLK,
    iREGASEL,
    iREGBSEL,
    iREGDSEL, // FROM PIPE_WRBK
    oREGA,
    oREGB,
    iREGD,
    iWR
);
    // ports
    input               iACLK;
    input   [4:0]       iREGASEL;
    input   [4:0]       iREGBSEL;
    input   [4:0]       iREGDSEL; // FROM PIPE_WRBK
    output  [31:0]      oREGA;
    output  [31:0]      oREGB;
    input   [31:0]      iREGD;
    input               iWR;
    
    // logics
    reg     [31:0]      rg[31:0];
    
    // reads
    assign oREGA = rg[iREGASEL];
    assign oREGB = rg[iREGBSEL];
    
    // writes
    always @(posedge iACLK) if(iWR) rg[iREGDSEL] <= iREGD;
endmodule