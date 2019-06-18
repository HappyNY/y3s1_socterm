
module SYSTEM (
	bufferram_out_address,
	bufferram_out_chipselect,
	bufferram_out_clken,
	bufferram_out_write,
	bufferram_out_readdata,
	bufferram_out_writedata,
	bufferram_out_byteenable,
	bufferram_rst_reset,
	buffram_clk_clk,
	clk_clk,
	pio_cmd_out_export,
	pio_data_in_export,
	pio_data_out_export,
	sdram_wire_addr,
	sdram_wire_ba,
	sdram_wire_cas_n,
	sdram_wire_cke,
	sdram_wire_cs_n,
	sdram_wire_dq,
	sdram_wire_dqm,
	sdram_wire_ras_n,
	sdram_wire_we_n);	

	input	[16:0]	bufferram_out_address;
	input		bufferram_out_chipselect;
	input		bufferram_out_clken;
	input		bufferram_out_write;
	output	[15:0]	bufferram_out_readdata;
	input	[15:0]	bufferram_out_writedata;
	input	[1:0]	bufferram_out_byteenable;
	input		bufferram_rst_reset;
	input		buffram_clk_clk;
	input		clk_clk;
	output	[31:0]	pio_cmd_out_export;
	input	[31:0]	pio_data_in_export;
	output	[31:0]	pio_data_out_export;
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
