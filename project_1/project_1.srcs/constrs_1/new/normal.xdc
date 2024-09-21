###############################################################################
# Clock Signal (add the appropriate clock pin)
###############################################################################
set_property PACKAGE_PIN F6 [get_ports sys_clk_clk_p]
set_property PACKAGE_PIN E6 [get_ports sys_clk_clk_n]
set_property IOSTANDARD LVDS_25 [get_ports sys_clk_clk_p]
set_property IOSTANDARD LVDS_25 [get_ports sys_clk_clk_n]


###############################################################################
# LED_A1 Pin
###############################################################################
set_property PACKAGE_PIN G3 [get_ports LED_A1]
set_property IOSTANDARD LVCMOS33 [get_ports LED_A1]
set_property PULLUP true [get_ports LED_A1]
set_property DRIVE 8 [get_ports LED_A1]
