`timescale 1ns/1ps

module tb_s1423;
    reg GND, VDD, CK;

    reg [16:0] vec; 

    wire G0 = vec[0];
    wire G1 = vec[1];
    wire G10 = vec[2];
    wire G11 = vec[3];
    wire G12 = vec[4];
    wire G13 = vec[5];
    wire G14 = vec[6];
    wire G15 = vec[7];
    wire G16 = vec[8];
    wire G2 = vec[9];
    wire G3 = vec[10];
    wire G4 = vec[11];
    wire G5 = vec[12];
    wire G6 = vec[13];
    wire G7 = vec[14];
    wire G8 = vec[15];
    wire G9 = vec[16];

    // Primary Outputs
    wire G701BF, G702, G726, G727, G729;

    s1423 UUT (
        .GND(GND), .VDD(VDD), .CK(CK), .G0(G0), .G1(G1),
        .G10(G10), .G11(G11), .G12(G12), .G13(G13), .G14(G14),
        .G15(G15), .G16(G16), .G2(G2), .G3(G3), .G4(G4),
        .G5(G5), .G6(G6), .G7(G7), .G8(G8), .G9(G9),
        .G701BF(G701BF), .G702(G702), .G726(G726), .G727(G727), .G729(G729)
    );

    integer file_handle;
    integer i;

    initial begin
        file_handle = $fopen("s1423_golden.csv", "w");

        // Dynamically generated header
        $fdisplay(file_handle, "VEC,G0,G1,G10,G11,G12,G13,G14,G15,G16,G2,G3,G4,G5,G6,G7,G8,G9,G701BF,G702,G726,G727,G729,G22,G23,G109,G113,G118,G125,G129,G140,G144,G149,G154,G159,G166,G175,G189,G193,G198,G208,G214,G218,G237,G242,G247,G252,G260,G303,G309,G315,G321,G360,G365,G373,G379,G384,G392,G397,G405,G408,G416,G424,G427,G438,G441,G447,G451,G459,G464,G469,G477,G494,G498,G503,G526,G531,G536,G541,G548,G565,G569,G573,G577,G590,G608,G613,G657,G663,G669,G675,G682,G687,G693,G705,G707,G713,G328,n2,n3,n4,n6,n7,n8,n10,n11,n12,n13,n14,n15,n16,n17,n18,n19,n20,n21,n23,n24,n26,n27,n29,n30,n32,n33,n34,n35,n37,n38,n39,n40,n41,n42,n43,n44,n45,n46,n47,n49,n50,n51,n52,n53,n54,n55,n60,n61,n64,n66,n69,n72,n75,n77,n78,n80,n81,n82,n83,n84,n85,n86,n89,n90,n91,n92,n93,n94,n95,n96,n97,n98,n99,n100,n101,n102,n103,n104,n105,n106,n107,n108,n109,n110,n111,n112,n113,n114,n115,n116,n117,n118,n119,n120,n121,n122,n123,n124,n125,n126,n127,n128,n129,n130,n131,n132,n133,n134,n135,n136,n137,n138,n139,n140,n141,n142,n143,n144,n145,n146,n147,n148,n149,n150,n151,n152,n153,n154,n155,n156,n157,n158,n159,n160,n161,n162,n163,n164,n165,n166,n167,n168,n169,n170,n171,n172,n173,n174,n175,n176,n177,n178,n179,n180,n181,n182,n183,n184,n185,n186,n187,n188,n189,n190,n191,n192,n193,n194,n195,n196,n197,n198,n199,n200,n201,n202,n203,n204,n205,n206,n207,n208,n209,n210,n212,n213,n214,n215,n216,n217,n218,n219,n220,n221,n222,n223,n224,n225,n226,n227,n228,n229,n230,n231,n232,n233,n234,n235,n236,n237,n238,n239,n240,n241,n242,n243,n244,n245,n246,n247,n248,n249,n250,n251,n252,n253,n254,n255,n256,n257,n258,n259,n260,n261,n262,n263,n264,n265,n266,n267,n268,n269,n270,n271,n272,n273,n274,n275,n276,n277,n278,n279,n280,n281,n282,n283,n284,n285,n286,n287,n288,n289,n290,n291,n292,n293,n294,n295,n296,n297,n298,n299,n300,n301,n302,n303,n304,n305,n306,n307,n308,n309,n310,n311,n312,n313,n314,n315,n316,n317,n318,n319,n320,n321,n322,n323,n324,n325,n326,n327,n328,n329,n330,n331,n332,n333,n334,n335,n336,n337,n338,n339,n340,n341,n342,n343,n344,n345,n346,n347,n348,n349,n350,n351,n352,n353,n354,n355,n356,n357,n358,n359,n360,n361,n362,n363,n364,n365,n366,n367,n370,n371,n372,n373,n374,n375,n376,n377,n378,n379,n380,n381,n382,n383,n384,n385,n386,n387,n388,n389,n390,n391,n392,n393,n394,n395,n396,n398,n399,n400,n401,n402,n403,n404,n405,n406,n407,n408,n409,n410,n411,n412,n413,n414,n415,n417,n420,n421,n422,n424,n425,n426,n430,n431,n432,n433,n434,n435,n436,n437,n438,n439,n440,n442,n445,n446,n447,n449,n450,n451,n452,n453,n454,n455,n456,n458,n459,n460,n461,n463,n464,n466,n467,n468,n469,n470,n471,n472,n473,n474,n475,n476,n477,n478,n479,n480,n481,n482,n483,n484,n485,n486,n487,n488,n489,n490,n491,n492,n493,n494,n495,n496,n497");

        GND = 0;
        VDD = 1;
        CK  = 0;

        for (i = 0; i < 1024; i = i + 1) begin
            vec = i;
            #10; 

            // Exactly 1 %0d and 550 %b format specifiers
            $fdisplay(file_handle, "%0d,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b,%b",
                i, G0, G1, G10, G11, G12, G13, G14, G15, G16, G2, G3, G4, G5, G6,
                G7, G8, G9, G701BF, G702, G726, G727, G729, UUT.G22, UUT.G23, UUT.G109, UUT.G113, UUT.G118, UUT.G125, UUT.G129,
                UUT.G140, UUT.G144, UUT.G149, UUT.G154, UUT.G159, UUT.G166, UUT.G175, UUT.G189, UUT.G193, UUT.G198, UUT.G208, UUT.G214, UUT.G218, UUT.G237, UUT.G242,
                UUT.G247, UUT.G252, UUT.G260, UUT.G303, UUT.G309, UUT.G315, UUT.G321, UUT.G360, UUT.G365, UUT.G373, UUT.G379, UUT.G384, UUT.G392, UUT.G397, UUT.G405,
                UUT.G408, UUT.G416, UUT.G424, UUT.G427, UUT.G438, UUT.G441, UUT.G447, UUT.G451, UUT.G459, UUT.G464, UUT.G469, UUT.G477, UUT.G494, UUT.G498, UUT.G503,
                UUT.G526, UUT.G531, UUT.G536, UUT.G541, UUT.G548, UUT.G565, UUT.G569, UUT.G573, UUT.G577, UUT.G590, UUT.G608, UUT.G613, UUT.G657, UUT.G663, UUT.G669,
                UUT.G675, UUT.G682, UUT.G687, UUT.G693, UUT.G705, UUT.G707, UUT.G713, UUT.G328, UUT.n2, UUT.n3, UUT.n4, UUT.n6, UUT.n7, UUT.n8, UUT.n10,
                UUT.n11, UUT.n12, UUT.n13, UUT.n14, UUT.n15, UUT.n16, UUT.n17, UUT.n18, UUT.n19, UUT.n20, UUT.n21, UUT.n23, UUT.n24, UUT.n26, UUT.n27,
                UUT.n29, UUT.n30, UUT.n32, UUT.n33, UUT.n34, UUT.n35, UUT.n37, UUT.n38, UUT.n39, UUT.n40, UUT.n41, UUT.n42, UUT.n43, UUT.n44, UUT.n45,
                UUT.n46, UUT.n47, UUT.n49, UUT.n50, UUT.n51, UUT.n52, UUT.n53, UUT.n54, UUT.n55, UUT.n60, UUT.n61, UUT.n64, UUT.n66, UUT.n69, UUT.n72,
                UUT.n75, UUT.n77, UUT.n78, UUT.n80, UUT.n81, UUT.n82, UUT.n83, UUT.n84, UUT.n85, UUT.n86, UUT.n89, UUT.n90, UUT.n91, UUT.n92, UUT.n93,
                UUT.n94, UUT.n95, UUT.n96, UUT.n97, UUT.n98, UUT.n99, UUT.n100, UUT.n101, UUT.n102, UUT.n103, UUT.n104, UUT.n105, UUT.n106, UUT.n107, UUT.n108,
                UUT.n109, UUT.n110, UUT.n111, UUT.n112, UUT.n113, UUT.n114, UUT.n115, UUT.n116, UUT.n117, UUT.n118, UUT.n119, UUT.n120, UUT.n121, UUT.n122, UUT.n123,
                UUT.n124, UUT.n125, UUT.n126, UUT.n127, UUT.n128, UUT.n129, UUT.n130, UUT.n131, UUT.n132, UUT.n133, UUT.n134, UUT.n135, UUT.n136, UUT.n137, UUT.n138,
                UUT.n139, UUT.n140, UUT.n141, UUT.n142, UUT.n143, UUT.n144, UUT.n145, UUT.n146, UUT.n147, UUT.n148, UUT.n149, UUT.n150, UUT.n151, UUT.n152, UUT.n153,
                UUT.n154, UUT.n155, UUT.n156, UUT.n157, UUT.n158, UUT.n159, UUT.n160, UUT.n161, UUT.n162, UUT.n163, UUT.n164, UUT.n165, UUT.n166, UUT.n167, UUT.n168,
                UUT.n169, UUT.n170, UUT.n171, UUT.n172, UUT.n173, UUT.n174, UUT.n175, UUT.n176, UUT.n177, UUT.n178, UUT.n179, UUT.n180, UUT.n181, UUT.n182, UUT.n183,
                UUT.n184, UUT.n185, UUT.n186, UUT.n187, UUT.n188, UUT.n189, UUT.n190, UUT.n191, UUT.n192, UUT.n193, UUT.n194, UUT.n195, UUT.n196, UUT.n197, UUT.n198,
                UUT.n199, UUT.n200, UUT.n201, UUT.n202, UUT.n203, UUT.n204, UUT.n205, UUT.n206, UUT.n207, UUT.n208, UUT.n209, UUT.n210, UUT.n212, UUT.n213, UUT.n214,
                UUT.n215, UUT.n216, UUT.n217, UUT.n218, UUT.n219, UUT.n220, UUT.n221, UUT.n222, UUT.n223, UUT.n224, UUT.n225, UUT.n226, UUT.n227, UUT.n228, UUT.n229,
                UUT.n230, UUT.n231, UUT.n232, UUT.n233, UUT.n234, UUT.n235, UUT.n236, UUT.n237, UUT.n238, UUT.n239, UUT.n240, UUT.n241, UUT.n242, UUT.n243, UUT.n244,
                UUT.n245, UUT.n246, UUT.n247, UUT.n248, UUT.n249, UUT.n250, UUT.n251, UUT.n252, UUT.n253, UUT.n254, UUT.n255, UUT.n256, UUT.n257, UUT.n258, UUT.n259,
                UUT.n260, UUT.n261, UUT.n262, UUT.n263, UUT.n264, UUT.n265, UUT.n266, UUT.n267, UUT.n268, UUT.n269, UUT.n270, UUT.n271, UUT.n272, UUT.n273, UUT.n274,
                UUT.n275, UUT.n276, UUT.n277, UUT.n278, UUT.n279, UUT.n280, UUT.n281, UUT.n282, UUT.n283, UUT.n284, UUT.n285, UUT.n286, UUT.n287, UUT.n288, UUT.n289,
                UUT.n290, UUT.n291, UUT.n292, UUT.n293, UUT.n294, UUT.n295, UUT.n296, UUT.n297, UUT.n298, UUT.n299, UUT.n300, UUT.n301, UUT.n302, UUT.n303, UUT.n304,
                UUT.n305, UUT.n306, UUT.n307, UUT.n308, UUT.n309, UUT.n310, UUT.n311, UUT.n312, UUT.n313, UUT.n314, UUT.n315, UUT.n316, UUT.n317, UUT.n318, UUT.n319,
                UUT.n320, UUT.n321, UUT.n322, UUT.n323, UUT.n324, UUT.n325, UUT.n326, UUT.n327, UUT.n328, UUT.n329, UUT.n330, UUT.n331, UUT.n332, UUT.n333, UUT.n334,
                UUT.n335, UUT.n336, UUT.n337, UUT.n338, UUT.n339, UUT.n340, UUT.n341, UUT.n342, UUT.n343, UUT.n344, UUT.n345, UUT.n346, UUT.n347, UUT.n348, UUT.n349,
                UUT.n350, UUT.n351, UUT.n352, UUT.n353, UUT.n354, UUT.n355, UUT.n356, UUT.n357, UUT.n358, UUT.n359, UUT.n360, UUT.n361, UUT.n362, UUT.n363, UUT.n364,
                UUT.n365, UUT.n366, UUT.n367, UUT.n370, UUT.n371, UUT.n372, UUT.n373, UUT.n374, UUT.n375, UUT.n376, UUT.n377, UUT.n378, UUT.n379, UUT.n380, UUT.n381,
                UUT.n382, UUT.n383, UUT.n384, UUT.n385, UUT.n386, UUT.n387, UUT.n388, UUT.n389, UUT.n390, UUT.n391, UUT.n392, UUT.n393, UUT.n394, UUT.n395, UUT.n396,
                UUT.n398, UUT.n399, UUT.n400, UUT.n401, UUT.n402, UUT.n403, UUT.n404, UUT.n405, UUT.n406, UUT.n407, UUT.n408, UUT.n409, UUT.n410, UUT.n411, UUT.n412,
                UUT.n413, UUT.n414, UUT.n415, UUT.n417, UUT.n420, UUT.n421, UUT.n422, UUT.n424, UUT.n425, UUT.n426, UUT.n430, UUT.n431, UUT.n432, UUT.n433, UUT.n434,
                UUT.n435, UUT.n436, UUT.n437, UUT.n438, UUT.n439, UUT.n440, UUT.n442, UUT.n445, UUT.n446, UUT.n447, UUT.n449, UUT.n450, UUT.n451, UUT.n452, UUT.n453,
                UUT.n454, UUT.n455, UUT.n456, UUT.n458, UUT.n459, UUT.n460, UUT.n461, UUT.n463, UUT.n464, UUT.n466, UUT.n467, UUT.n468, UUT.n469, UUT.n470, UUT.n471,
                UUT.n472, UUT.n473, UUT.n474, UUT.n475, UUT.n476, UUT.n477, UUT.n478, UUT.n479, UUT.n480, UUT.n481, UUT.n482, UUT.n483, UUT.n484, UUT.n485, UUT.n486,
                UUT.n487, UUT.n488, UUT.n489, UUT.n490, UUT.n491, UUT.n492, UUT.n493, UUT.n494, UUT.n495, UUT.n496, UUT.n497
            );
        end

        $fclose(file_handle);
        $finish; 
    end
endmodule
