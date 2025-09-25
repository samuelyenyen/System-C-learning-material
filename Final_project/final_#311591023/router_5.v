`timescale 1ns/10ps
module router_5 (
    input clk,  rst,
    output reg [33:0] out_flit_0,out_flit_1,out_flit_2,out_flit_3,out_flit_4,
    output out_req_0, out_req_1, out_req_2, out_req_3, out_req_4,
    input in_ack_0, in_ack_1, in_ack_2, in_ack_3, in_ack_4, 
    
    input [33:0] in_flit_0, in_flit_1, in_flit_2, in_flit_3, in_flit_4,
    input in_req_0, in_req_1, in_req_2, in_req_3, in_req_4, 
    output out_ack_0, out_ack_1, out_ack_2, out_ack_3, out_ack_4 
);


assign out_req_0 =1;
assign out_req_1 =1;
assign out_req_2 =1;
assign out_req_3 =1;
assign out_req_4 =1;
assign out_ack_0 =1;
assign out_ack_1 =1;
assign out_ack_2 =1;
assign out_ack_3 =1;
assign out_ack_4 =1;

always @(posedge clk) begin
    out_flit_4 <= in_flit_0;
    out_flit_2 <= in_flit_4;
end

endmodule