`timescale 1ns/1ps

module tb_s510;
    reg GND, VDD, CK;

    reg [18:0] vec; 

    wire cnt10 = vec[0];
    wire cnt13 = vec[1];
    wire cnt21 = vec[2];
    wire cnt261 = vec[3];
    wire cnt272 = vec[4];
    wire cnt283 = vec[5];
    wire cnt284 = vec[6];
    wire cnt44 = vec[7];
    wire cnt45 = vec[8];
    wire cnt509 = vec[9];
    wire cnt511 = vec[10];
    wire cnt567 = vec[11];
    wire cnt591 = vec[12];
    wire john = vec[13];
    wire pcnt12 = vec[14];
    wire pcnt17 = vec[15];
    wire pcnt241 = vec[16];
    wire pcnt27 = vec[17];
    wire pcnt6 = vec[18];

    // Primary Outputs
    wire cblank, cclr, csm, csync, pc, pclr, vsync;

    s510 UUT (
        .GND(GND), .VDD(VDD), .CK(CK), .cnt10(cnt10), .cnt13(cnt13),
        .cnt21(cnt21), .cnt261(cnt261), .cnt272(cnt272), .cnt283(cnt283), .cnt284(cnt284),
        .cnt44(cnt44), .cnt45(cnt45), .cnt509(cnt509), .cnt511(cnt511), .cnt567(cnt567),
        .cnt591(cnt591), .john(john), .pcnt12(pcnt12), .pcnt17(pcnt17), .pcnt241(pcnt241),
        .pcnt27(pcnt27), .pcnt6(pcnt6), .cblank(cblank), .cclr(cclr), .csm(csm),
        .csync(csync), .pc(pc), .pclr(pclr), .vsync(vsync)
    );

    integer file_handle;
    integer i;

    initial begin
        file_handle = $fopen("s510_golden.csv", "w");

        // Dynamically generated header
        $fdisplay(file_handle, "VEC,cnt10,cnt13,cnt21,cnt261,cnt272,cnt283,cnt284,cnt44,cnt45,cnt509,cnt511,cnt567,cnt591,john,pcnt12,pcnt17,pcnt241,pcnt27,pcnt6,cblank,cclr,csm,csync,pc,pclr,vsync,st_5,st_4,st_3,st_2,st_1,st_0,n1,n2,n3,n4,n5,n6,n7,n8,n9,n10,n11,n12,n13,n14,n15,n16,n17,n18,n19,n20,n21,n22,n23,n24,n25,n26,n27,n28,n29,n30,n31,n32,n33,n34,n35,n36,n37,n38,n39,n40,n41,n42,n43,n44,n45,n46,n47,n48,n49,n50,n51,n52,n53,n54,n55,n56,n57,n58,n59,n60,n61,n62,n63,n64,n65,n66,n67,n68,n69,n70,n71,n72,n73,n74,n75,n76,n77,n78,n79,n80,n81,n82,n83,n84,n85,n86,n87,n88,n89,n90,n91,n92,n93,n94,n95,n96,n97,n98,n99,n100,n101,n102,n103,n104,n105,n106,n107,n108,n109,n110,n111,n112,n113,n114,n115,n116,n117,n118,n119,n120,n121,n122,n123,n124,n125,n126,n127,n128,n129,n130,n131,n132,n133,n134,n135,n136,n137,n138,n139,n140,n141,n142,n143,n144,n145,n146,n147,n148,n149,n150,n151,n152,n153,n154,n155,n156,n157,n158,n159,n160,n161,n162,n163,n164,n165,n166,n167,n168,n169,n170,n171,n172,n173,n174,n175,n180,n182,n184");

        GND = 0;
        VDD = 1;
        CK  = 0;

        for (i = 0; i < 1024; i = i + 1) begin
            vec = i;
            #10; 

            // Exactly 1 %0d and 210 %b format specifiers
            $fdisplay(file_handle, "%0d,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b",
                i, cnt10, cnt13, cnt21, cnt261, cnt272, cnt283, cnt284, cnt44, cnt45, cnt509, cnt511, cnt567, cnt591, john,
                pcnt12, pcnt17, pcnt241, pcnt27, pcnt6, cblank, cclr, csm, csync, pc, pclr, vsync, UUT.st_5, UUT.st_4, UUT.st_3,
                UUT.st_2, UUT.st_1, UUT.st_0, UUT.n1, UUT.n2, UUT.n3, UUT.n4, UUT.n5, UUT.n6, UUT.n7, UUT.n8, UUT.n9, UUT.n10, UUT.n11, UUT.n12,
                UUT.n13, UUT.n14, UUT.n15, UUT.n16, UUT.n17, UUT.n18, UUT.n19, UUT.n20, UUT.n21, UUT.n22, UUT.n23, UUT.n24, UUT.n25, UUT.n26, UUT.n27,
                UUT.n28, UUT.n29, UUT.n30, UUT.n31, UUT.n32, UUT.n33, UUT.n34, UUT.n35, UUT.n36, UUT.n37, UUT.n38, UUT.n39, UUT.n40, UUT.n41, UUT.n42,
                UUT.n43, UUT.n44, UUT.n45, UUT.n46, UUT.n47, UUT.n48, UUT.n49, UUT.n50, UUT.n51, UUT.n52, UUT.n53, UUT.n54, UUT.n55, UUT.n56, UUT.n57,
                UUT.n58, UUT.n59, UUT.n60, UUT.n61, UUT.n62, UUT.n63, UUT.n64, UUT.n65, UUT.n66, UUT.n67, UUT.n68, UUT.n69, UUT.n70, UUT.n71, UUT.n72,
                UUT.n73, UUT.n74, UUT.n75, UUT.n76, UUT.n77, UUT.n78, UUT.n79, UUT.n80, UUT.n81, UUT.n82, UUT.n83, UUT.n84, UUT.n85, UUT.n86, UUT.n87,
                UUT.n88, UUT.n89, UUT.n90, UUT.n91, UUT.n92, UUT.n93, UUT.n94, UUT.n95, UUT.n96, UUT.n97, UUT.n98, UUT.n99, UUT.n100, UUT.n101, UUT.n102,
                UUT.n103, UUT.n104, UUT.n105, UUT.n106, UUT.n107, UUT.n108, UUT.n109, UUT.n110, UUT.n111, UUT.n112, UUT.n113, UUT.n114, UUT.n115, UUT.n116, UUT.n117,
                UUT.n118, UUT.n119, UUT.n120, UUT.n121, UUT.n122, UUT.n123, UUT.n124, UUT.n125, UUT.n126, UUT.n127, UUT.n128, UUT.n129, UUT.n130, UUT.n131, UUT.n132,
                UUT.n133, UUT.n134, UUT.n135, UUT.n136, UUT.n137, UUT.n138, UUT.n139, UUT.n140, UUT.n141, UUT.n142, UUT.n143, UUT.n144, UUT.n145, UUT.n146, UUT.n147,
                UUT.n148, UUT.n149, UUT.n150, UUT.n151, UUT.n152, UUT.n153, UUT.n154, UUT.n155, UUT.n156, UUT.n157, UUT.n158, UUT.n159, UUT.n160, UUT.n161, UUT.n162,
                UUT.n163, UUT.n164, UUT.n165, UUT.n166, UUT.n167, UUT.n168, UUT.n169, UUT.n170, UUT.n171, UUT.n172, UUT.n173, UUT.n174, UUT.n175, UUT.n180, UUT.n182,
                UUT.n184
            );
        end

        $fclose(file_handle);
        $finish; 
    end
endmodule
