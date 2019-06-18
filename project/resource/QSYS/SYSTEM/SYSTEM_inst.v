	SYSTEM u0 (
		.bufferram_out_address    (<connected-to-bufferram_out_address>),    // bufferram_out.address
		.bufferram_out_chipselect (<connected-to-bufferram_out_chipselect>), //              .chipselect
		.bufferram_out_clken      (<connected-to-bufferram_out_clken>),      //              .clken
		.bufferram_out_write      (<connected-to-bufferram_out_write>),      //              .write
		.bufferram_out_readdata   (<connected-to-bufferram_out_readdata>),   //              .readdata
		.bufferram_out_writedata  (<connected-to-bufferram_out_writedata>),  //              .writedata
		.bufferram_out_byteenable (<connected-to-bufferram_out_byteenable>), //              .byteenable
		.bufferram_rst_reset      (<connected-to-bufferram_rst_reset>),      // bufferram_rst.reset
		.buffram_clk_clk          (<connected-to-buffram_clk_clk>),          //   buffram_clk.clk
		.clk_clk                  (<connected-to-clk_clk>),                  //           clk.clk
		.pio_cmd_out_export       (<connected-to-pio_cmd_out_export>),       //   pio_cmd_out.export
		.pio_data_in_export       (<connected-to-pio_data_in_export>),       //   pio_data_in.export
		.pio_data_out_export      (<connected-to-pio_data_out_export>),      //  pio_data_out.export
		.sdram_wire_addr          (<connected-to-sdram_wire_addr>),          //    sdram_wire.addr
		.sdram_wire_ba            (<connected-to-sdram_wire_ba>),            //              .ba
		.sdram_wire_cas_n         (<connected-to-sdram_wire_cas_n>),         //              .cas_n
		.sdram_wire_cke           (<connected-to-sdram_wire_cke>),           //              .cke
		.sdram_wire_cs_n          (<connected-to-sdram_wire_cs_n>),          //              .cs_n
		.sdram_wire_dq            (<connected-to-sdram_wire_dq>),            //              .dq
		.sdram_wire_dqm           (<connected-to-sdram_wire_dqm>),           //              .dqm
		.sdram_wire_ras_n         (<connected-to-sdram_wire_ras_n>),         //              .ras_n
		.sdram_wire_we_n          (<connected-to-sdram_wire_we_n>)           //              .we_n
	);

