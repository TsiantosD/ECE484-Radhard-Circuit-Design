`timescale 1ns/1ps

module INV_X1 (input A, output ZN); assign ZN = ~A; endmodule
module AND2_X1 (input A1, A2, output ZN); assign ZN = A1 & A2; endmodule
module OR2_X1 (input A1, A2, output ZN); assign ZN = A1 | A2; endmodule
module NAND2_X1 (input A1, A2, output ZN); assign ZN = ~(A1 & A2); endmodule
module NAND3_X1 (input A1, A2, A3, output ZN); assign ZN = ~(A1 & A2 & A3); endmodule
module NOR2_X1 (input A1, A2, output ZN); assign ZN = ~(A1 | A2); endmodule

module DFF_X1 (input D, CK, output Q, QN);
    assign Q = 1'b1;
    assign QN = 1'b0;
endmodule
