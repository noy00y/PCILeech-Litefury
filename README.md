# FPGA-Firmware: PCILeech ZDMA + LiteFury Extension

## Overview
**PCILeech ZDMA FPGA Firmware** with a modified **UART IP** using an **FTDI FT232H** chip.  
This design enables **DMA support** on a LiteFury FPGA board (Artix-7 XC7A100T) at a more affordable price point (≈ **\$250** vs. \$600 for the typical ZDMA board).  

- **UART over USB**: We’re extending DMA capabilities to systems lacking Thunderbolt access, using USB-to-UART (FT232H) for data transfer.  
- **Mobile Device Possibility**: If your mobile device supports USB OTG and has the right drivers, this approach **could** facilitate DMA operations over mobile.  
- **Low-Volume, High-Speed Processing**: While PCIe TLP exchanges can be very fast (potentially multiple GB/s in theory), the **UART link** is a bottleneck.  
  - **FT232H** can reach up to **~40 MB/s** in optimized conditions (USB 2.0 High Speed). Realistically, you might see **~10–15 MB/s**.  
  - Thunderbolt solutions can push **tens of GB/s**, so this approach trades raw throughput for **cost-effectiveness** and **broader device compatibility**.

## RTL Design + Synthesis
- **Verilog Source**: Contains the main RTL files implementing PCIe TLP handling and the custom UART IP.  
- **XDC Constraints**: Pin assignments and timing constraints tailored for the LiteFury Artix-7 FPGA.  
- **Synthesis Tools**: Compatible with Xilinx Vivado. Make sure to target the **XC7A100T** device variant.

## Timing Analysis
- **Clock Domains**: Separated for PCIe logic vs. UART logic.  
- **Achievable Frequencies**: Tested up to **100 MHz** for the core PCIe logic; UART typically runs at **48 MHz** or a suitable clock derived for USB.  
- **Performance Considerations**: The limiting factor is often the USB-to-UART throughput rather than the internal FPGA speeds.

## Resources
**[PCILeech ZDMA Firmware](https://github.com/ufrisk/pcileech-fpga/tree/master/ZDMA)**.
**[Litefury FPGA](https://github.com/RHSResearchLLC/NiteFury-and-LiteFury)**.
