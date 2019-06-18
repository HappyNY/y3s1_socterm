// Test module which contains instruction queue
module GPPCU_TEST_QUEUE(
    iACLK               ,
    iINSTR_CLK          ,
    iINSTR              , 
    
    iLMEM_CLK           ,
    iLMEM_THREAD_SEL    , // Thread selection input
    iLMEM_ADDR          ,
    iLMEM_WDATA         ,
    oLMEM_RDATA         ,
    iLMEM_RD            ,
    iLMEM_WR            ,
    
    oGMEM_ADDR          ,
    iGMEM_WDATA         ,
    oGMEM_CLK
);
    // -- ports 
    input               iACLK               ;
    input               iINSTR_CLK          ;
    input   [31:0]      iINSTR              ; 
    
    input               iLMEM_CLK           ;
    input   [ 9:0]      iLMEM_THREAD_SEL    ; // Thread selection input
    input   [15:0]      iLMEM_ADDR          ;
    input   [31:0]      iLMEM_WDATA         ;
    output  [31:0]      oLMEM_RDATA         ;
    input               iLMEM_RD            ;
    input               iLMEM_WR            ;
    
    output  [16:0]      oGMEM_ADDR          ;
    input   [31:0]      iGMEM_WDATA         ;
    output              oGMEM_CLK           ;

    // -- regs
    
    // -- instruction queue    
endmodule