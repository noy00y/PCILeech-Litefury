`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 10/26/2018 12:43:09 PM
// Design Name: 
// Module Name: dna_reader
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


module dna_reader(
    input clk,
    output ready, // ready signal when DNA read operation is complete

    // Outputs two 32 bit dna values (64 bit total)
    output [31:0] dna_ls, // lower bit
    output [31:0] dna_ms); // upper bit
    
    // Storage
    reg ready_w = 0;
    reg [63:0] dna_read = 0; // 57 bit - unique device identifier 
    
    // Map output ports 
    assign ready = ready_w; // register that signals DNA read process is complete. Init at 0
    assign dna_ls = dna_read[31:0]; // register to hold DNA value. Init at 0
    assign dna_ms = dna_read[63:32]; // 32 bit counter to control the read process
    
    // DNA port wires
    wire DNAP_DOUT; // output data from the DNA port
    reg DNAP_READ = 1; // control signal for the DNA port. High = read DNA, Low = shift out DNA bits
    reg DNAP_SHIFT = 0; // control signal to shift DNA output when set High
    
    // DNA_PORT: Device DNA Access Port
    //           Artix-7
    // Xilinx HDL Language Template, version 2018.2
    /*
    - xilinx specific module for access to DNA  
    */
    
    DNA_PORT #(
       .SIM_DNA_VALUE(57'h000000000000000)  // Specifies a sample 57-bit DNA value for simulation
    )
    DNA_PORT_inst (
       .DOUT(DNAP_DOUT),   // 1-bit output: DNA output data.
       .CLK(clk),          // 1-bit input: Clock input.
       .DIN(0),            // 1-bit input: User data input pin.
       .READ(DNAP_READ),   // 1-bit input: Active high load DNA, active low read input.
       .SHIFT(DNAP_SHIFT)  // 1-bit input: Active high shift enable input.
    );
    // End of DNA_PORT_inst instantiation
    
    // local stoarge
    
    
    reg [31:0] count = 0;
    
    // Note- do this logic on the opposite edge that DNA port is active (ie. perform read process when clock is on falling edge)
    always @(negedge clk)
    if (0 == ready_w) // when 0 -> DNA read process begins. Counter increments on each clk cycle
    begin
        count <= count+1;   
        if (count == 1) // dna port read signal is dasserted and shift signal is asserted. This starts 
                        // the process of bit shifting
            begin
                DNAP_READ <= 0; // shift out dna bits
                DNAP_SHIFT <= 1; // set signal to high to shift out bits
            end

        // shifting continues only up to 56 bits which corresponds to the DNA
        // so once shifting is complete we will accumulate the dna port bit values into the dna device identifier register
        // append shifted in bits to the right most position using left shift operator
        else if (count < (66-8)) // 56 bits
            begin
                dna_read <= (dna_read << 1) | DNAP_DOUT;
            end
        else // counter has reached the end -> set ready_w = 1 --> DNA value is ready to be read
            begin
            // Done- all bits shifted out
            ready_w <= 1;
            end 
    end
endmodule
