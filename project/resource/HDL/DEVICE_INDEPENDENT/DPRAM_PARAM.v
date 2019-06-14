module DPRAM_PARAM #(
    parameter
    DBW = 32, 
    DEPTH = 1023
)
(
    iPA_CLK,
    iPA_ADDR,
    iPA_WR, 
    iPA_WDATA,
    oPA_RDATA,    
    
    iPB_CLK,
    iPB_ADDR,
    iPB_WR, 
    iPB_WDATA,
    oPB_RDATA
); 
    `include "bit_fit.vh"
    
    // params
    localparam ABW = bit_fit(DEPTH); 
    
    // ports
    input                           iPA_CLK;
    input   [ABW-1:0]               iPA_ADDR;
    input                           iPA_WR; 
    input   [DBW-1:0]               iPA_WDATA;
    output  [DBW-1:0]               oPA_RDATA;    
    
    input                           iPB_CLK;
    input   [ABW-1:0]  iPB_ADDR;
    input                           iPB_WR; 
    input   [DBW-1:0]               iPB_WDATA;
    output  [DBW-1:0]               oPB_RDATA;    
    
    // port details
    reg     [DBW-1:0]               oPA_RDATA;    
    reg     [DBW-1:0]               oPB_RDATA;    
    
    // logic
	reg     [DBW-1:0]               ram[DEPTH-1:0];
	
	// Port A
	always @ (posedge iPA_CLK)
	begin
		if (iPA_WR) 
		begin
			ram[iPA_ADDR] <= iPA_WDATA;
			oPA_RDATA <= iPA_WDATA;
		end
		else 
		begin
			oPA_RDATA  <= ram[iPA_ADDR];
		end
	end
	
	// Port B
	always @ (posedge iPB_CLK)
	begin
		if (iPB_WR) 
		begin
			ram[iPB_ADDR] <= iPB_WDATA;
			oPB_RDATA <= iPB_WDATA;
		end
		else 
		begin
			oPB_RDATA  <= ram[iPB_ADDR];
		end
	end 
endmodule