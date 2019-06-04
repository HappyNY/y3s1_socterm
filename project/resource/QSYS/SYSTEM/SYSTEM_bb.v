
module SYSTEM (
	clk_clk,
	bufferram_out_address,
	bufferram_out_chipselect,
	bufferram_out_clken,
	bufferram_out_write,
	bufferram_out_readdata,
	bufferram_out_writedata,
	bufferram_out_byteenable,
	buffram_clk_clk,
	bufferram_rst_reset,
	sdram_wire_addr,
	sdram_wire_ba,
	sdram_wire_cas_n,
	sdram_wire_cke,
	sdram_wire_cs_n,
	sdram_wire_dq,
	sdram_wire_dqm,
	sdram_wire_ras_n,
	sdram_wire_we_n);	

	input		clk_clk;
	input	[16:0]	bufferram_out_address;
	input		bufferram_out_chipselect;
	input		bufferram_out_clken;
	input		bufferram_out_write;
	output	[15:0]	bufferram_out_readdata;
	input	[15:0]	bufferram_out_writedata;
	input	[1:0]	bufferram_out_byteenable;
	input		buffram_clk_clk;
	input		bufferram_rst_reset;
	output	[12:0]	sdram_wire_addr;
	output	[1:0]	sdram_wire_ba;
	output		sdram_wire_cas_n;
	output		sdram_wire_cke;
	output		sdram_wire_cs_n;
	inout	[31:0]	sdram_wire_dq;
	output	[3:0]	sdram_wire_dqm;
	output		sdram_wire_ras_n;
	output		sdram_wire_we_n;
endmodule
