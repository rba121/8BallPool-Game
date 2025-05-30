// Copyright 1986-2020 Xilinx, Inc. All Rights Reserved.
// --------------------------------------------------------------------------------
// Tool Version: Vivado v.2020.2 (win64) Build 3064766 Wed Nov 18 09:12:45 MST 2020
// Date        : Fri Mar  7 18:27:08 2025
// Host        : ensc-pit-w23 running 64-bit major release  (build 9200)
// Command     : write_verilog -force -mode synth_stub
//               u:/ensc452/FinalProject/Zynq_book/adventures_with_ip/adventures_with_ip.gen/sources_1/bd/ip_design/ip/ip_design_clk_wiz_0_0/ip_design_clk_wiz_0_0_stub.v
// Design      : ip_design_clk_wiz_0_0
// Purpose     : Stub declaration of top-level module interface
// Device      : xc7z020clg484-1
// --------------------------------------------------------------------------------

// This empty module with port declaration file causes synthesis tools to infer a black box for IP.
// The synthesis directives are for Synopsys Synplify support to prevent IO buffer insertion.
// Please paste the declaration into a Verilog source file or add the file as an additional source.
module ip_design_clk_wiz_0_0(clk_out1, resetn, locked, clk_in1)
/* synthesis syn_black_box black_box_pad_pin="clk_out1,resetn,locked,clk_in1" */;
  output clk_out1;
  input resetn;
  output locked;
  input clk_in1;
endmodule
