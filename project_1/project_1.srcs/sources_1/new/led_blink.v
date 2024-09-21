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
    input sys_clk_clk_p,   // Differential clock (positive input)
    input sys_clk_clk_n,   // Differential clock (negative input)
    output reg LED_A1      // Output to LED
);
    wire clk;  // Single-ended clock after conversion
    
    // Convert differential clock to single-ended clock using IBUFDS
    IBUFDS ibufds_inst (
        .O(clk),              // Output single-ended clock
        .I(sys_clk_clk_p),    // Input positive clock
        .IB(sys_clk_clk_n)    // Input negative clock
    );

    reg [25:0] counter;

    // LED blink logic
    always @(posedge clk) begin
        counter <= counter + 1;
        LED_A1 <= counter[25];  // Toggle LED at a slower rate
    end
endmodule
