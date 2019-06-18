module TOP
(
	//////////// BASIC //////////
    CLK,
    PB,
	//////////// DISPLAY //////////
    LCDRGB,
    LCDCON,

	//////////// SDRAM //////////
	DRAM_ADDR,
	DRAM_BA,
	DRAM_CAS_N,
	DRAM_CKE,
	DRAM_CLK,
	DRAM_CS_N,
	DRAM_DQ,
	DRAM_DQM,
	DRAM_RAS_N,
	DRAM_WE_N, 
);
    // ------ PORTS
    input               CLK;
    input  [3:0]        PB;
    
    output [23:0]       LCDRGB;
    output [13:0]       LCDCON;
    
    output [12:0]	    DRAM_ADDR;  
    output [1:0]	    DRAM_BA;    
    output              DRAM_CAS_N; 
    output              DRAM_CKE;   
    output              DRAM_CLK;   
    output              DRAM_CS_N;  
    inout  [31:0]		DRAM_DQ;    
    output [3:0]		DRAM_DQM;   
    output              DRAM_RAS_N; 
    output              DRAM_WE_N;   
    
    // ------ local
    wire iCLK50MHz = CLK;
    wire inRST = PB[0];
    
    wire [16:0] bufferram_out_address   ;
    wire        bufferram_out_chipselect = 1;
    wire        bufferram_out_clken     ;
    wire        bufferram_out_write     ;
    wire [15:0] bufferram_out_readdata  ; 
    wire [15:0] bufferram_out_writedata ;
    wire [1:0]  bufferram_out_byteenable;
    wire        clk_clk;
    wire [23:0] buffer_to_tft_color;
    wire        bram_clk;
    
    wire [9:0]  tft_haddr;
    wire [8:0]  tft_vaddr;
    wire        tft_hsync;
    wire        tft_vsync;
    wire        tft_de;
    wire        bufferram_rst_reset = 0;
    wire        buffram_clk_clk = bram_clk; 
    
    wire [12:0] sdram_wire_addr;
    wire [1:0]  sdram_wire_ba;
    wire        sdram_wire_cas_n;
    wire        sdram_wire_cke;
    wire        sdram_wire_cs_n;
    wire [31:0] sdram_wire_dq;
    wire [3:0]  sdram_wire_dqm;
    wire        sdram_wire_ras_n;
    wire        sdram_wire_we_n;
    
    wire [16:0] buff_addr = ((tft_vaddr>>1) * 400) + (tft_haddr>>1); 
    
    wire [31:0] gppcu_cmd, gppcu_datain, gppcu_dataout;
    
    // ------ regs 
    reg[16:0] buff_pvt;
    
    // ------ logic
    SYSTEM SYSTEM_inst(
        .bufferram_out_address(buff_addr),
        .bufferram_out_chipselect(bufferram_out_chipselect), 
        .bufferram_out_clken(1),       
        .bufferram_out_write(/*MUST SET 0 LATER*/0),       
        .bufferram_out_readdata(bufferram_out_readdata),   
        .bufferram_out_writedata(/*MUST SET 0 LATER*/0),//buff_addr),    
        .bufferram_out_byteenable(2'b11), 
		.bufferram_rst_reset(bufferram_rst_reset),      
		.buffram_clk_clk(buffram_clk_clk),          
        .clk_clk(iCLK50MHz),         
		.pio_cmd_out_export(gppcu_cmd),       //   pio_cmd_out.export
		.pio_data_in_export(gppcu_dataout),       //   pio_data_in.export
		.pio_data_out_export(gppcu_datain),      //  pio_data_out.export
		.sdram_wire_addr(DRAM_ADDR),       
		.sdram_wire_ba(DRAM_BA),           
		.sdram_wire_cas_n(DRAM_CAS_N),     
		.sdram_wire_cke(DRAM_CKE),         
		.sdram_wire_cs_n(DRAM_CS_N),       
		.sdram_wire_dq(DRAM_DQ),           
		.sdram_wire_dqm(DRAM_DQM),         
		.sdram_wire_ras_n(DRAM_RAS_N),     
		.sdram_wire_we_n(DRAM_WE_N)        
    );
    
    // Phase shift by 3 ns
    SDRAM_PLL (
	    iCLK50MHz,
	    DRAM_CLK
    );
    
    // Assemble color
    wire [15:0] rgb16 = bufferram_out_readdata;
    assign buffer_to_tft_color = {
        rgb16[15:12], {4{rgb16[11]}}, 
        rgb16[10: 6], {3{rgb16[ 5]}}, 
        rgb16[ 4: 1], {4{rgb16[ 0]}}
    }; 
    
    LCD LCD_inst
    (
        .clk(iCLK50MHz),                
        .rst_(inRST),
        .bBL(1),                // Backlight en
        .bDTH(0),               // Dithering en
        .oBRAM_CLK(bram_clk),           // to VRAM
        .iCOLOR(buffer_to_tft_color),              // from VRAM 
        .oHADDR(tft_haddr),
        .oVADDR(tft_vaddr),
        .oADDR(bufferram_out_address),
        .oLCDRGB(LCDRGB),            // to LCD device output
        .oLCDCON(LCDCON[13:3]),
        .oDE(tft_de),
        .oHSYNC(tft_hsync),
        .oVSYNC(tft_vsync)
    ); 
    
    // GPPCU CORE
    GPPCU_TEST_QUEUE GPPCU_TEST_QUEUE_inst(
        .iACLK               (iCLK50MHz),
        .inRST               (inRST),
        .iCMD                (gppcu_cmd),
        .iDATA               (gppcu_datain),    
        .oDATA               (gppcu_dataout)
    );

endmodule