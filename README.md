# FPGA-Firmware: PCILeech ZDMA + LiteFury Extension

## Overview
**PCILeech ZDMA FPGA Firmware** with a modified **UART IP** using an **FTDI FT232H** chip.  
This design enables **DMA support** on a LiteFury FPGA board (Artix-7 XC7A100T) at a more affordable price point (≈ **\$250** vs. \$600 for the typical ZDMA board).  

- **UART over USB**: We’re extending DMA capabilities to systems lacking Thunderbolt access, using USB-to-UART (FT232H) for data transfer.  
- **Mobile Device Possibility**: If your mobile device supports USB OTG and has the right drivers, this approach **could** facilitate DMA operations over mobile.  
- **Low-Volume, High-Speed Processing**: While PCIe TLP exchanges can be very fast (multiple GB/s), using an FT232H will bottleneck our system to about **~10–15 MB/s**.  
  - Thunderbolt solutions can push **tens of GB/s**, so this approach trades raw throughput for **cost-effectiveness** and **broader device compatibility**.

## RTL Design Roadmap
- ZDMA Firmware Review (In Progress)
- UART IP Review (Not Started)
- Timing Verification (Not Started)

## Resources
**[PCILeech ZDMA Firmware](https://github.com/ufrisk/pcileech-fpga/tree/master/ZDMA)**.
**[Litefury FPGA](https://github.com/RHSResearchLLC/NiteFury-and-LiteFury)**.
