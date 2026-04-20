`timescale 1ns/1ps

module tb_s344;
    reg GND, VDD, CK;

    reg [8:0] vec; 

    wire A0 = vec[0];
    wire A1 = vec[1];
    wire A2 = vec[2];
    wire A3 = vec[3];
    wire B0 = vec[4];
    wire B1 = vec[5];
    wire B2 = vec[6];
    wire B3 = vec[7];
    wire START = vec[8];

    // Primary Outputs
    wire CNTVCO2, CNTVCON2, P0, P1, P2, P3, P4, P5, P6, P7, READY;

    s344 UUT (
        .GND(GND), .VDD(VDD), .CK(CK), .A0(A0), .A1(A1),
        .A2(A2), .A3(A3), .B0(B0), .B1(B1), .B2(B2),
        .B3(B3), .START(START), .CNTVCO2(CNTVCO2), .CNTVCON2(CNTVCON2), .P0(P0),
        .P1(P1), .P2(P2), .P3(P3), .P4(P4), .P5(P5),
        .P6(P6), .P7(P7), .READY(READY)
    );

    integer file_handle;
    integer i;

    initial begin
        file_handle = $fopen("s344_golden.csv", "w");

        // Dynamically generated header
        $fdisplay(file_handle, "VEC,A0,A1,A2,A3,B0,B1,B2,B3,START,CNTVCO2,CNTVCON2,P0,P1,P2,P3,P4,P5,P6,P7,READY,CNTVG3VD,CNTVG2VD,CNTVG1VD,MRVG4VD,MRVG3VD,MRVG2VD,MRVG1VD,n1,n2,n3,n4,n6,n7,n8,n9,n10,n11,n12,n13,n14,n15,n16,n17,n18,n19,n20,n21,n22,n23,n24,n25,n26,n27,n28,n29,n30,n31,n32,n33,n34,n35,n36,n37,n38,n39,n40,n41,n42,n43,n44,n45,n46,n47,n48,n49,n50,n51,n52,n53,n54,n55,n56,n57,n58,n59,n60,n61,n62,n63,n64,n65,n66,n67,n68,n69,n70,n71,n72,n73,n74,n75,n76,n77,n78,n79,n80,n81,n82,n83,n84,n85,n86,n87,n88,n89,n90,n91,n92,n93,n94,n95,n96,n97,n98,n99,n100,n101");

        GND = 0;
        VDD = 1;
        CK  = 0;

        for (i = 0; i < 512; i = i + 1) begin
            vec = i;
            #10; 

            // Exactly 1 %0d and 127 %b format specifiers
            $fdisplay(file_handle, "%0d,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b",
                i, A0, A1, A2, A3, B0, B1, B2, B3, START, CNTVCO2, CNTVCON2, P0, P1, P2,
                P3, P4, P5, P6, P7, READY, UUT.CNTVG3VD, UUT.CNTVG2VD, UUT.CNTVG1VD, UUT.MRVG4VD, UUT.MRVG3VD, UUT.MRVG2VD, UUT.MRVG1VD, UUT.n1, UUT.n2,
                UUT.n3, UUT.n4, UUT.n6, UUT.n7, UUT.n8, UUT.n9, UUT.n10, UUT.n11, UUT.n12, UUT.n13, UUT.n14, UUT.n15, UUT.n16, UUT.n17, UUT.n18,
                UUT.n19, UUT.n20, UUT.n21, UUT.n22, UUT.n23, UUT.n24, UUT.n25, UUT.n26, UUT.n27, UUT.n28, UUT.n29, UUT.n30, UUT.n31, UUT.n32, UUT.n33,
                UUT.n34, UUT.n35, UUT.n36, UUT.n37, UUT.n38, UUT.n39, UUT.n40, UUT.n41, UUT.n42, UUT.n43, UUT.n44, UUT.n45, UUT.n46, UUT.n47, UUT.n48,
                UUT.n49, UUT.n50, UUT.n51, UUT.n52, UUT.n53, UUT.n54, UUT.n55, UUT.n56, UUT.n57, UUT.n58, UUT.n59, UUT.n60, UUT.n61, UUT.n62, UUT.n63,
                UUT.n64, UUT.n65, UUT.n66, UUT.n67, UUT.n68, UUT.n69, UUT.n70, UUT.n71, UUT.n72, UUT.n73, UUT.n74, UUT.n75, UUT.n76, UUT.n77, UUT.n78,
                UUT.n79, UUT.n80, UUT.n81, UUT.n82, UUT.n83, UUT.n84, UUT.n85, UUT.n86, UUT.n87, UUT.n88, UUT.n89, UUT.n90, UUT.n91, UUT.n92, UUT.n93,
                UUT.n94, UUT.n95, UUT.n96, UUT.n97, UUT.n98, UUT.n99, UUT.n100, UUT.n101
            );
        end

        $fclose(file_handle);
        $finish; 
    end
endmodule
