	component SYSTEM is
		port (
			bufferram_out_address    : in    std_logic_vector(16 downto 0) := (others => 'X'); -- address
			bufferram_out_chipselect : in    std_logic                     := 'X';             -- chipselect
			bufferram_out_clken      : in    std_logic                     := 'X';             -- clken
			bufferram_out_write      : in    std_logic                     := 'X';             -- write
			bufferram_out_readdata   : out   std_logic_vector(15 downto 0);                    -- readdata
			bufferram_out_writedata  : in    std_logic_vector(15 downto 0) := (others => 'X'); -- writedata
			bufferram_out_byteenable : in    std_logic_vector(1 downto 0)  := (others => 'X'); -- byteenable
			bufferram_rst_reset      : in    std_logic                     := 'X';             -- reset
			buffram_clk_clk          : in    std_logic                     := 'X';             -- clk
			clk_clk                  : in    std_logic                     := 'X';             -- clk
			pio_cmd_out_export       : out   std_logic_vector(31 downto 0);                    -- export
			pio_data_in_export       : in    std_logic_vector(31 downto 0) := (others => 'X'); -- export
			pio_data_out_export      : out   std_logic_vector(31 downto 0);                    -- export
			sdram_wire_addr          : out   std_logic_vector(12 downto 0);                    -- addr
			sdram_wire_ba            : out   std_logic_vector(1 downto 0);                     -- ba
			sdram_wire_cas_n         : out   std_logic;                                        -- cas_n
			sdram_wire_cke           : out   std_logic;                                        -- cke
			sdram_wire_cs_n          : out   std_logic;                                        -- cs_n
			sdram_wire_dq            : inout std_logic_vector(31 downto 0) := (others => 'X'); -- dq
			sdram_wire_dqm           : out   std_logic_vector(3 downto 0);                     -- dqm
			sdram_wire_ras_n         : out   std_logic;                                        -- ras_n
			sdram_wire_we_n          : out   std_logic                                         -- we_n
		);
	end component SYSTEM;

	u0 : component SYSTEM
		port map (
			bufferram_out_address    => CONNECTED_TO_bufferram_out_address,    -- bufferram_out.address
			bufferram_out_chipselect => CONNECTED_TO_bufferram_out_chipselect, --              .chipselect
			bufferram_out_clken      => CONNECTED_TO_bufferram_out_clken,      --              .clken
			bufferram_out_write      => CONNECTED_TO_bufferram_out_write,      --              .write
			bufferram_out_readdata   => CONNECTED_TO_bufferram_out_readdata,   --              .readdata
			bufferram_out_writedata  => CONNECTED_TO_bufferram_out_writedata,  --              .writedata
			bufferram_out_byteenable => CONNECTED_TO_bufferram_out_byteenable, --              .byteenable
			bufferram_rst_reset      => CONNECTED_TO_bufferram_rst_reset,      -- bufferram_rst.reset
			buffram_clk_clk          => CONNECTED_TO_buffram_clk_clk,          --   buffram_clk.clk
			clk_clk                  => CONNECTED_TO_clk_clk,                  --           clk.clk
			pio_cmd_out_export       => CONNECTED_TO_pio_cmd_out_export,       --   pio_cmd_out.export
			pio_data_in_export       => CONNECTED_TO_pio_data_in_export,       --   pio_data_in.export
			pio_data_out_export      => CONNECTED_TO_pio_data_out_export,      --  pio_data_out.export
			sdram_wire_addr          => CONNECTED_TO_sdram_wire_addr,          --    sdram_wire.addr
			sdram_wire_ba            => CONNECTED_TO_sdram_wire_ba,            --              .ba
			sdram_wire_cas_n         => CONNECTED_TO_sdram_wire_cas_n,         --              .cas_n
			sdram_wire_cke           => CONNECTED_TO_sdram_wire_cke,           --              .cke
			sdram_wire_cs_n          => CONNECTED_TO_sdram_wire_cs_n,          --              .cs_n
			sdram_wire_dq            => CONNECTED_TO_sdram_wire_dq,            --              .dq
			sdram_wire_dqm           => CONNECTED_TO_sdram_wire_dqm,           --              .dqm
			sdram_wire_ras_n         => CONNECTED_TO_sdram_wire_ras_n,         --              .ras_n
			sdram_wire_we_n          => CONNECTED_TO_sdram_wire_we_n           --              .we_n
		);

