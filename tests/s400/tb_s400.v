`timescale 1ns/1ps

module tb_s400;
    reg GND, VDD, CK;

    reg [2:0] vec; 

    wire CLR = vec[0];
    wire FM = vec[1];
    wire TEST = vec[2];

    // Primary Outputs
    wire GRN1, GRN2, RED1, RED2, YLW1, YLW2;

    s400 UUT (
        .GND(GND), .VDD(VDD), .CK(CK), .CLR(CLR), .FM(FM),
        .TEST(TEST), .GRN1(GRN1), .GRN2(GRN2), .RED1(RED1), .RED2(RED2),
        .YLW1(YLW1), .YLW2(YLW2)
    );

    integer file_handle;
    integer i;

    initial begin
        file_handle = $fopen("s400_golden.csv", "w");

        // Dynamically generated header
        $fdisplay(file_handle, "VEC,CLR,FM,TEST,GRN1,GRN2,RED1,RED2,YLW1,YLW2,TESTLVIINLATCHVCDAD,FMLVIINLATCHVCDAD,TCOMB_YA2,Y1C,R2C,TCOMB_GA2,TCOMB_GA1,C3_Q3VD,C3_Q2VD,C3_Q1VD,C3_Q0VD,UC_16VD,UC_17VD,UC_18VD,UC_19VD,UC_8VD,UC_9VD,UC_10VD,UC_11VD,n1,n2,n3,n5,n7,n9,n18,n19,n20,n21,n22,n23,n24,n25,n26,n27,n28,n29,n30,n31,n32,n33,n34,n35,n36,n37,n38,n39,n40,n41,n42,n43,n44,n45,n46,n47,n48,n49,n50,n51,n52,n53,n54,n55,n56,n57,n58,n59,n60,n61,n62,n63,n64,n65,n66,n67,n68,n69,n70,n71,n72,n73,n74,n75,n76,n77,n78,n79,n80,n81,n82,n83,n84,n85,n86,n87,n88,n90,n91,n92,n94,n95,n96,n97,n98,n99,n100,n101,n104,n105,n106,n107,n108,n109,n110,n111,n112,n113,n114,n115");

        GND = 0;
        VDD = 1;
        CK  = 0;

        for (i = 0; i < 8; i = i + 1) begin
            vec = i;
            #10; 

            // Exactly 1 %0d and 128 %b format specifiers
            $fdisplay(file_handle, "%0d,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b",
                i, CLR, FM, TEST, GRN1, GRN2, RED1, RED2, YLW1, YLW2, UUT.TESTLVIINLATCHVCDAD, UUT.FMLVIINLATCHVCDAD, UUT.TCOMB_YA2, UUT.Y1C, UUT.R2C,
                UUT.TCOMB_GA2, UUT.TCOMB_GA1, UUT.C3_Q3VD, UUT.C3_Q2VD, UUT.C3_Q1VD, UUT.C3_Q0VD, UUT.UC_16VD, UUT.UC_17VD, UUT.UC_18VD, UUT.UC_19VD, UUT.UC_8VD, UUT.UC_9VD, UUT.UC_10VD, UUT.UC_11VD, UUT.n1,
                UUT.n2, UUT.n3, UUT.n5, UUT.n7, UUT.n9, UUT.n18, UUT.n19, UUT.n20, UUT.n21, UUT.n22, UUT.n23, UUT.n24, UUT.n25, UUT.n26, UUT.n27,
                UUT.n28, UUT.n29, UUT.n30, UUT.n31, UUT.n32, UUT.n33, UUT.n34, UUT.n35, UUT.n36, UUT.n37, UUT.n38, UUT.n39, UUT.n40, UUT.n41, UUT.n42,
                UUT.n43, UUT.n44, UUT.n45, UUT.n46, UUT.n47, UUT.n48, UUT.n49, UUT.n50, UUT.n51, UUT.n52, UUT.n53, UUT.n54, UUT.n55, UUT.n56, UUT.n57,
                UUT.n58, UUT.n59, UUT.n60, UUT.n61, UUT.n62, UUT.n63, UUT.n64, UUT.n65, UUT.n66, UUT.n67, UUT.n68, UUT.n69, UUT.n70, UUT.n71, UUT.n72,
                UUT.n73, UUT.n74, UUT.n75, UUT.n76, UUT.n77, UUT.n78, UUT.n79, UUT.n80, UUT.n81, UUT.n82, UUT.n83, UUT.n84, UUT.n85, UUT.n86, UUT.n87,
                UUT.n88, UUT.n90, UUT.n91, UUT.n92, UUT.n94, UUT.n95, UUT.n96, UUT.n97, UUT.n98, UUT.n99, UUT.n100, UUT.n101, UUT.n104, UUT.n105, UUT.n106,
                UUT.n107, UUT.n108, UUT.n109, UUT.n110, UUT.n111, UUT.n112, UUT.n113, UUT.n114, UUT.n115
            );
        end

        $fclose(file_handle);
        $finish; 
    end
endmodule
