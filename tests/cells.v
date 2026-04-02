`timescale 1ns/1ps

// ==========================================
// Standard Cell Behavioral Definitions
// ==========================================

// Flip-Flop
module DFF_X1 (input D, CK, output Q, QN);
    assign Q = 1'b1;
    assign QN = 1'b0;
endmodule

// Inverter
module INV_X1 (input A, output ZN);
    assign ZN = ~A;
endmodule

// 2-Input Gates
module NAND2_X1 (input A1, input A2, output ZN);
    assign ZN = ~(A1 & A2);
endmodule

module NOR2_X1 (input A1, input A2, output ZN);
    assign ZN = ~(A1 | A2);
endmodule

module AND2_X1 (input A1, input A2, output ZN);
    assign ZN = (A1 & A2);
endmodule

module OR2_X1 (input A1, input A2, output ZN);
    assign ZN = (A1 | A2);
endmodule

// 3-Input Gates
module NAND3_X1 (input A1, input A2, input A3, output ZN);
    assign ZN = ~(A1 & A2 & A3);
endmodule

module NOR3_X1 (input A1, input A2, input A3, output ZN);
    assign ZN = ~(A1 | A2 | A3);
endmodule

// 4-Input Gates
module NOR4_X1 (input A1, input A2, input A3, input A4, output ZN);
    assign ZN = ~(A1 | A2 | A3 | A4);
endmodule

module AND4_X1 (input A1, input A2, input A3, input A4, output ZN);
    assign ZN = (A1 & A2 & A3 & A4);
endmodule