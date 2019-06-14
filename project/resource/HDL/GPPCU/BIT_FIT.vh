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
    