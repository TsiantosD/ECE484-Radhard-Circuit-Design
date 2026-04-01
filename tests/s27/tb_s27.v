`timescale 1ns/1ps

module tb_s27;
    reg GND, VDD, CK;

    reg [3:0] vec; 

    wire G3 = vec[3];
    wire G2 = vec[2];
    wire G1 = vec[1];
    wire G0 = vec[0];

    wire G17;

    s27 UUT (
        .GND(GND), .VDD(VDD), .CK(CK),
        .G0(G0), .G1(G1), .G17(G17), .G2(G2), .G3(G3)
    );

    integer file_handle;
    integer i;

    initial begin
        file_handle = $fopen("s27_golden.csv", "w");

        $fdisplay(file_handle, "VEC,G0,G1,G2,G3,G17,G10,G13,n1,n2,n3,n4,n5,n6,n7,n8,n10,n11");

        GND = 0;
        VDD = 1;
        CK  = 0; // Clock is completely irrelevant

        for (i = 0; i < 16; i = i + 1) begin
            vec = i;
            #10; 

            $fdisplay(file_handle, "%0d,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b",
                i, G0,G1,G2,G3, G17, 
                UUT.G10,UUT.G13,UUT.n1,UUT.n2,UUT.n3,UUT.n4,UUT.n5,UUT.n6,UUT.n7,UUT.n8,UUT.n10,UUT.n11);
        end

        $fclose(file_handle);
        $finish; 
    end
endmodule
