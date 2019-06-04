`define RAM_NAME REPLACE_THIS_TO_YOUR_OWN_DPRAM_NAME
`define ALTERA
`define ITF_NAME REPLACE_THIS_TO_YOUR_OWN_INTERFACE_NAME

module `ITF_NAME #
(
    parameter 
    PT1_RW_WID = 8, 
    PT1_RW_DEP = 1024,
    // RD DEPTH WILL BE AUTOMATICALLY GENERATED  
    PT2_RW_WID = 16
)
(
    iPT1CLK,
    iPT1RDEN,
    iPT1WREN,
    iPT1ADDR,
    iPT1WDATA,
    oPT1RDATA,

    iPT2CLK,
    iPT2RDEN,
    iPT2WREN,
    iPT2ADDR,
    iPT2WDATA,
    oPT2RDATA,   
);
    function[5:0] bit_fit;
        input reg[31:0] val;
        begin
            bit_fit = 0;
            while(val > 0) begin
                val = val >> 1;
                bit_fit = bit_fit + 1;
            end
        end
    endfunction
    
    localparam BITSZ = PT1_RW_WID * PT1_RW_DEP;
    localparam PT2_RW_DEP = BITSZ / PT2_RW_WID
    localparam PT1_ADDRBW = `bit_fit(PT1_RW_DEP);
    localparam PT2_ADDRBW = `bit_fit(PT2_RW_DEP);
    
    // ------ ports
    input  iPT1CLK;
    input  iPT1RDEN;
    input  iPT1WREN;
    input  [PT1_ADDRBW-1:0] iPT1ADDR;
    input  [PT1_RW_WID-1:0] iPT1WDATA;
    output [PT1_RW_WID-1:0] oPT1RDATA;
    
    input  iPT2CLK;
    input  iPT2RDEN;
    input  iPT2WREN;
    input  [PT2_ADDRBW-1:0] iPT2ADDR;
    input  [PT2_RW_WID-1:0] iPT2WDATA;
    output [PT2_RW_WID-1:0] oPT2RDATA; 
    
    // ------ logics
`ifdef ALTERA
    `RAM_NAME __RAM_INSTANCE__ (
	.address_a(iPT1ADDR),
	.address_b(iPT2ADDR),
	.clock_a(iPT1CLK),
	.clock_b(iPT2CLK),
	.data_a(iPT1WDATA),
	.data_b(iPT2WDATA),
	.wren_a(iPT1WREN),
	.wren_b(iPT2WREN),
	.q_a(oPT1RDATA),
	.q_b(oPT2RDATA));
`else
    
`endif         
endmodule