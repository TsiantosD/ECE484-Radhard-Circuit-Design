`timescale 1ns/1ps

module tb_s298;
    reg GND, VDD, CK;

    reg [2:0] vec; 

    wire G2 = vec[2];
    wire G1 = vec[1];
    wire G0 = vec[0];

    // Primary Outputs
    wire G117, G118, G132, G133, G66, G67;

    s298 UUT (
        .GND(GND), .VDD(VDD), .CK(CK),
        .G0(G0), .G1(G1), .G2(G2),
        .G117(G117), .G118(G118), .G132(G132), .G133(G133), .G66(G66), .G67(G67)
    );

    integer file_handle;
    integer i;

    initial begin
        file_handle = $fopen("s298_golden.csv", "w");

        // 105-column header
        $fdisplay(file_handle, "VEC,G0,G1,G2,G117,G118,G132,G133,G66,G67,G29,G30,G34,G39,G44,G56,G86,G92,G98,G102,G107,G113,G119,G125,n1,n2,n3,n4,n6,n7,n8,n9,n10,n11,n12,n13,n16,n17,n18,n19,n20,n21,n22,n23,n24,n25,n26,n27,n28,n29,n30,n31,n32,n33,n34,n35,n36,n37,n38,n39,n40,n41,n42,n43,n44,n45,n46,n47,n48,n49,n50,n51,n52,n53,n54,n55,n56,n57,n58,n59,n60,n61,n62,n63,n64,n65,n66,n67,n68,n69,n70,n71,n72,n73,n76,n77,n78,n79,n80,n81,n82,n83,n84");

        GND = 0;
        VDD = 1;
        CK  = 0;

        for (i = 0; i < 8; i = i + 1) begin
            vec = i;
            #10; 

            $fdisplay(file_handle, "%0d,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b",
                i, G0, G1, G2, G117, G118, G132, G133, G66, G67,
                UUT.G29, UUT.G30, UUT.G34, UUT.G39, UUT.G44, UUT.G56, UUT.G86, UUT.G92, UUT.G98, UUT.G102, UUT.G107, UUT.G113, UUT.G119, UUT.G125, UUT.n1, UUT.n2, UUT.n3, UUT.n4, UUT.n6, UUT.n7, UUT.n8, UUT.n9, UUT.n10, UUT.n11, UUT.n12, UUT.n13, UUT.n16, UUT.n17, UUT.n18, UUT.n19, UUT.n20, UUT.n21, UUT.n22, UUT.n23, UUT.n24, UUT.n25, UUT.n26, UUT.n27, UUT.n28, UUT.n29, UUT.n30, UUT.n31, UUT.n32, UUT.n33, UUT.n34, UUT.n35, UUT.n36, UUT.n37, UUT.n38, UUT.n39, UUT.n40, UUT.n41, UUT.n42, UUT.n43, UUT.n44, UUT.n45, UUT.n46, UUT.n47, UUT.n48, UUT.n49, UUT.n50, UUT.n51, UUT.n52, UUT.n53, UUT.n54, UUT.n55, UUT.n56, UUT.n57, UUT.n58, UUT.n59, UUT.n60, UUT.n61, UUT.n62, UUT.n63, UUT.n64, UUT.n65, UUT.n66, UUT.n67, UUT.n68, UUT.n69, UUT.n70, UUT.n71, UUT.n72, UUT.n73, UUT.n76, UUT.n77, UUT.n78, UUT.n79, UUT.n80, UUT.n81, UUT.n82, UUT.n83, UUT.n84
            );
        end

        $fclose(file_handle);
        $finish; 
    end
endmodule
