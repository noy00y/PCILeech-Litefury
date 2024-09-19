`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 09/19/2024 12:41:16 PM
// Design Name: 
// Module Name: led_blink
// Project Name: 
// Target Devices: 
// Tool Versions: 
// Description: 
// 
// Dependencies: 
// 
// Revision:
// Revision 0.01 - File Created
// Additional Comments:
// 
//////////////////////////////////////////////////////////////////////////////////


module led_blink(
    input clk,
    output reg LED_A1  // Change the signal name to match the constraint file
);
    reg [25:0] counter;
    
    always @(posedge clk) begin
        counter <= counter + 1;
        LED_A1 <= counter[25];  // Toggle LED at slower rate
    end
endmodule
