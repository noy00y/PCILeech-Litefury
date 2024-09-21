###############################################################################
# Clock Signal (updated to the correct clock pins)
###############################################################################
# Using MRCC pins IO_L12P_T1_MRCC_16 (D17) and IO_L12N_T1_MRCC_16 (C17)
set_property PACKAGE_PIN D17 [get_ports sys_clk_clk_p]
set_property PACKAGE_PIN C17 [get_ports sys_clk_clk_n]
set_property IOSTANDARD LVDS_25 [get_ports sys_clk_clk_p]
set_property IOSTANDARD LVDS_25 [get_ports sys_clk_clk_n]

###############################################################################
# LED_A1 Pin (no change needed)
###############################################################################
set_property PACKAGE_PIN G3 [get_ports LED_A1]
set_property IOSTANDARD LVCMOS33 [get_ports LED_A1]
set_property PULLUP true [get_ports LED_A1]
set_property DRIVE 8 [get_ports LED_A1]
