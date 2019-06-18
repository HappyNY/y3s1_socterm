/**
 * LCD device controller module.
 * The VRAM should be provided.
 *
 * * * Documentation
     Since there was not any specific guide or reference for driving LCD display, 
    I assumed several theories to drive this LCD module.
    
    @todo 19.01.03. implement this!
    Model A) .... Failed!
        - When the DE signal is active, on every DCLK edge will put the pixel data into device. 
        - On every falling edge of the DE signal, the LCD automatically performs line feed.
        - On every 480th falling edge of DE singal, (480 means vertical line count of LCD display) the cursor returns to home.
        ==> Failed to drive LCD device via DE signal only. 
        
    Model B)
        - Use hsync and vsync to drive LCD device.
        
 * * *
 *
 *      LCDCON[13] DCLK
 *      LCDCON[12] DE
 *      LCDCON[11] DIM
 *      LCDCON[10] DITH
 *      LCDCON[ 9] HSD
 *      LCDCON[ 8] MODE
 *      LCDCON[ 7] POWER_CTL
 *      LCDCON[ 6] RSTB
 *      LCDCON[ 5] SHLR
 *      LCDCON[ 4] UPDN
 *      LCDCON[ 3] VSD
 *
 *      LCDCON[ 2] I2C_SCL
 *      LCDCON[ 1] I2C_SDA
 *      LCDCON[ 0] INT_n
 */
module LCD
(
    clk,                
    rst_,
    bBL,                // Backlight en
    bDTH,               // Dithering en

    oBRAM_CLK,           // to VRAM
    iCOLOR,              // from VRAM 

    oHADDR,
    oVADDR,
    oADDR,

    oLCDRGB,            // to LCD device output
    oLCDCON,
    
    oDE,
    oHSYNC,
    oVSYNC
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
    
    // ------ params 
    localparam HPXL = 800, VPXL = 480;
    localparam MAXP = HPXL * VPXL;
    
    localparam 
        ABW  = bit_fit(MAXP - 1),
        HBW  = bit_fit(HPXL - 1),
        VBW  = bit_fit(VPXL - 1);
        
    // ------ ports;
    input   clk;                
    input   rst_;
    input   bBL;               // Backlight en
    input   bDTH;               // Dithering en
    
    output  oBRAM_CLK;           // to VRAM
    input   [23:0]      iCOLOR;              // from VRAM 
    
    output  [HBW-1:0]   oHADDR;
    output  [VBW-1:0]   oVADDR;
    output  [ABW-1:0]   oADDR;
    
    output  [23:0]      oLCDRGB;            // to LCD device output
    output  [13:3]      oLCDCON;
    
    output              oVSYNC;
    output              oHSYNC;
    output              oDE   ;
    
    // ------ locals
    wire                oVSYNC  = oLCDCON[ 3];
    wire                oHSYNC  = oLCDCON[ 9];
    wire                oDE     = oLCDCON[12]; 
    wire reset;
    assign oLCDCON[6] = reset;
             
    LCD_CON LCD_CON_inst(
        .iCLK(clk),
        .inRST(rst_),
        .oDCLK(oLCDCON[13]),
        .oDE(oLCDCON[12]),
        .oMODE(oLCDCON[8]),
        .onRST(reset),
        .oHSYNC(oLCDCON[9]),
        .oVSYNC(oLCDCON[3]),
        .oPOW(oLCDCON[7]),
        .oBRAM_CLK(oBRAM_CLK),
        .oHADDR(oHADDR),
        .oVADDR(oVADDR),
        .oADDR(oADDR)
    ); 
    
    // reg[23:0] col;
    // wire[7:0] VADR = {oVADDR[8:5], 4'd0};
    // always @(oBRAM_CLK) begin
    //     col <= { VADR, VADR, VADR };
    // end
    assign oLCDRGB = iCOLOR;
    assign oLCDCON[11] = bBL;
    assign oLCDCON[10] = bDTH; 
    assign oLCDCON[ 5] = 1'b1;
    assign oLCDCON[ 4] = 1'b0; 
endmodule