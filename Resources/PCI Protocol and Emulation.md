# FPGA PCIe Configuration & Shadow Emulation

- Updated: March 2025 
- Author: Ozair Khan
---

## 1. Overview

- **Objective**: Create a system where the host PC sees a PCIe device (the FPGA). Depending on certain conditions or analysis needs, the FPGA can present **real DMA** registers or **emulated Ethernet** registers.  
- **Mechanism**:  
  1. Host PC sends PCIe TLPs (configuration or memory transactions).  
  2. FPGA firmware intercepts and forwards these TLPs via UART to a **second PC** (which is running a kernel driver).  
  3. Second PC decides which response to send (shadow vs. physical config) and returns the TLP data back over UART.  
  4. FPGA re-injects the TLP completion onto PCIe for the host PC.

---

## 2. Base Address Registers (BARs)

A **Base Address Register** is how the host OS maps the device’s memory or I/O regions into system address space. Each PCI device can have up to six BARs in its configuration space (BAR0–BAR5).

### 2.1. 32-Bit vs. 64-Bit BARs
- **32-bit BAR**: Contains a 32-bit base address.
  - If the device claims memory in a 32-bit address range, you’ll typically see this in lower system memory.  
![Screenshot 2024-12-25 180043](https://github.com/user-attachments/assets/f7b769e9-535c-4d4a-a57d-9ea3e443ced8)  
- **64-bit BAR**: Spans two adjacent BAR registers (e.g., BAR0 & BAR1).  
  - Allows a device to claim a 64-bit address range for higher memory addresses and large memory maps.
![Screenshot 2024-12-26 003121](https://github.com/user-attachments/assets/1decdd92-1324-476d-a41c-0ba0dc30ec85)

### 2.2. Prefetchable vs. Non-Prefetchable
- **Prefetchable Memory Base/Limit Registers**: 
  - Indicate that the region does not produce side effects; the system may read ahead (prefetch) without issues. Commonly used for “normal” memory resources.  
![Screenshot 2024-12-26 103033](https://github.com/user-attachments/assets/723f5dc8-75cd-4fee-8a07-350ea84b6441)
- **Non-Prefetchable Memory Base/Limit Registers**: 
  - Indicate reads have side effects or cannot be safely prefetched. Often used for device registers or memory where each read matters.
![Screenshot 2024-12-27 201906](https://github.com/user-attachments/assets/51eca179-06c2-4bbf-9307-d6bbb0b76cbf)
![Screenshot 2024-12-26 185507](https://github.com/user-attachments/assets/2079eb12-cd04-4d9a-8823-a604b5b30cb7)

**Key Points**:
- The host uses these base/limit registers to decide how to map your device into its address space.  
- Shadowing or modifying these in the “shadow config” can make the device appear to have a different memory layout.





---

## 3. PCIe TLP Structure

A Transaction Layer Packet (TLP) is the fundamental unit of transfer on PCI Express.

### 3.1. Config TLP (Read/Write)
- Used by the host during device enumeration or reconfiguration.  
- **Header fields** typically include:
  - **Type** (Config Read / Config Write)
  - **Requestor ID**
  - **Register Number** (which part of config space is being accessed)
  - **3DW or 4DW Header**:
    - **3DW Header** is common when there’s no extended address field.  
    - **4DW Header** is used if additional address or fields are needed.
  - **Data** (in the case of Config Write).
![Screenshot 2024-12-24 123708](https://github.com/user-attachments/assets/1df713bf-4253-46c5-baba-0f497cb76cbb)
![Screenshot 2024-12-29 180621](https://github.com/user-attachments/assets/f06b4cde-4190-48e1-a577-40e530438195)

### 3.2. Memory/Data TLP
- Used for normal device operations (e.g., reading/writing BAR spaces).  
- **Header fields** can include:
  - **Address** (lower 32 bits or full 64 bits for larger addresses).
  - **Byte enables** to specify which bytes in a payload are valid.
- The **Payload** may carry data for writes or be empty for reads (the data then arrives in a Completion TLP).

### 3.3. Messages & Implicit Routing
- **Message TLPs**: Used for interrupts, power management events, error reporting, etc.  
---

## Register Pointer Details

When a PCIe configuration transaction targets a device, the **Address/Pointer** field in the header determines which register (in doubleword increments) is accessed. Below is a typical breakdown for a Type 0 Configuration TLP address field:

| Bits      | Field Name                      | Description                                                                                                                          |
|-----------|---------------------------------|--------------------------------------------------------------------------------------------------------------------------------------|
| 31        | Enable Config Space Mapping (E) | Some implementations use this bit (set to 1) to indicate that the lower bits are used for config-space addressing.                   |
| 30:24     | Reserved                        | Typically reserved in Type 0 config access; should remain 0.                                                                         |
| 23:16     | Bus Number                      | Identifies which bus the transaction is targeted at (for multi-bus systems or behind PCIe-PCI bridges).                              |
| 15:11     | Device Number                   | Identifies which device on that bus to access (0–31).                                                                                |
| 10:8      | Function Number                 | Identifies which function within the device (0–7).                                                                                  |
| 7:2       | Register Pointer (64 DW)        | The register offset in the configuration space, measured in 4-byte doublewords (DW). For instance, `0x00` refers to the Device ID / Vendor ID. |
| 1:0       | Alignment Bits (Always 00)      | Must be 00 to align to a 32-bit (4-byte) boundary.                                                                                   |

![Screenshot 2024-12-24 114612](https://github.com/user-attachments/assets/b42d0907-b7a9-48cf-815a-95622e0dd95d)
### Why It Matters

1. **Access Granularity**  
   - Configuration space is typically accessed on 32-bit boundaries, hence the lower two bits (bits [1:0]) must be zero.  
   - The value in bits [7:2] points to which 4-byte register is being accessed. For example, a `Register Pointer` of `0x04` refers to the Command/Status register in standard PCI config space.

2. **Multiplexing Within the FPGA**  
   - The FPGA logic examines these bits to route the config TLP to the correct register block (physical or shadow).  
   - If using shadow logic, you might intercept or modify the returning data for specific offsets (e.g., changing the BAR registers or Vendor/Device IDs).

3. **Enabling Config Space Mapping**  
   - Some designs or documentation reference bit 31 (or 30/31) as a signal that config-space mapping is active. In many standard PCIe IP cores, you might simply see reserved bits there. The important part is that the `Register Pointer` bits (7:2) always align to a 4-byte boundary, ensuring the correct configuration register is addressed.

Overall, these pointer bits are **crucial** for indexing into the device’s 256-byte (or extended) configuration space. In a typical setup, you’ll parse them to determine exactly which register is being accessed and then decide whether to respond with your **physical** or **shadow** config data.


## 4. Physical vs. Shadow Configuration Space

### 4.1. Physical Config Space
- Contains the “real” vendor ID, device ID, BARs, command/status registers, etc., that correspond to your genuine DMA or custom logic.  
- Host sees these values to load the correct driver (e.g., a custom DMA driver).

### 4.2. Shadow Config Space
- Maintained in RTL or a separate block inside the FPGA.  
- **Purpose**: 
  - Allow the system to respond differently to configuration reads/writes, effectively emulating a legit controller such as a Ethernet, SSD, etc...  
  - Let you analyze or manipulate certain transactions without affecting real DMA traffic.
- **Mechanism**:
  1. A config read TLP arrives at the FPGA.  
  2. FPGA forwards the TLP to the second PC via UART.  
  3. Second PC analyzes the TLP (e.g., which register is being accessed).  
  4. Second PC decides if the reply should come from **Shadow** or **Physical** config data.  
  5. Second PC returns the selected values via UART to the FPGA.  
  6. FPGA re-injects this into a completion TLP to the host.  
- **What is Modified?**  
  - Possibly **BAR values**, **Vendor/Device IDs**, **Subsystem IDs**, class codes, interrupt configuration, or any other standard or extended capability register.  
  - The second PC can rewrite these fields to present entirely different “personalities” to the host.

---
## 5. Emulation -> Sample pcileech-fpga firmware 
**[pcileech-rt5392](https://github.com/ret2c/pcileech-rt5392)** - PCILeech firmware, masquerading as legal Ralink RT5392 device.  
**[pcileech-multimedia-hd](https://github.com/dom0ng/pcileech-multimedia-hd)** - FPGA card looks like an capture card, but hides inside [pcileech-fpga](https://github.com/ufrisk/pcileech-fpga) researching tool.  
**[pcileech-wifi-v2](https://github.com/dom0ng/pcileech-wifi-v2)** - FPGA card looks like a wireless adapter, but hides inside [pcileech-fpga](https://github.com/ufrisk/pcileech-fpga) researching tool.   
**[pcileech-audio](https://github.com/dom0ng/pcileech-audio)** - FPGA card looks like an audio card, but hides inside [pcileech-fpga](https://github.com/ufrisk/pcileech-fpga) researching tool.   
**[pcileech-cardreader](https://github.com/dom0ng/pcileech-cardreader)** - FPGA card looks like a sd card reader, but hides inside [pcileech-fpga](https://github.com/ufrisk/pcileech-fpga) researching tool.   
**[pcileech-modem](https://github.com/dom0ng/pcileech-modem)** - FPGA card looks like a pcie modem, but hides inside [pcileech-fpga](https://github.com/ufrisk/pcileech-fpga) researching tool.   
**[pcileech-serialport](https://github.com/dom0ng/pcileech_serialport)** - FPGA card looks like a pcie serial port, but hides inside [pcileech-fpga](https://github.com/ufrisk/pcileech-fpga) researching tool.   
### Example Emulated Ethernet vs. DMA

Once the host detects a device’s config space, it:
- Loads an **Ethernet driver** if the class code and IDs indicate a network controller.  
- Loads a **DMA or custom driver** if the device identifies as some accelerator or other function.

By selectively returning:
- **Ethernet** config/class codes in the “shadow” space, the host OS or user process sees a NIC and might attempt standard NIC operations (registering an interface, etc.).  
- **DMA** config/class codes in the “physical” space, the host sees a custom DMA device.

**Memory-Mapped I/O** (the host’s reads/writes to BAR regions) can also be forwarded to the second PC for analysis. If you want the system to behave as a real device, the second PC can pass that traffic on to the FPGA’s logic or simply respond with emulated data.

---

## 6. Template Kernel Driver (Second PC)

Below is a **simplified** C-style driver template that demonstrates how you might handle TLPs arriving via UART (with two additional pins controlled by an FTDI FT232H). This driver sketches how you could:

1. **Read** incoming TLP data from the FPGA (through UART).  
2. **Process** or log it for reverse engineering/malware analysis.  
3. **Decide** whether to respond with shadow or physical info.  
4. **Send** the resulting data (completion TLP) back to the FPGA for injection onto the PCIe bus.

```c
/*
 * Author: Ozair Khan, 2025
 * Driver Template for basic TLP Forwarding / Emulation 
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/uaccess.h>
#include <linux/fs.h>
#include <linux/tty.h>         // Potentially for UART
#include <linux/gpio/consumer.h> // Additional pins (FT232H)
#include <linux/interrupt.h>

#define DEVICE_NAME "tlp_emulation"

// Pseudocode struct representing a TLP
struct pcie_tlp {
    u8  header[16];  // Enough for up to a 4DW header
    u8  payload[256]; // Example max size
    u32 length;
};

static int tlp_uart_open(struct inode *inode, struct file *file)
{
    // 1. Open / initialize UART or FT232H interface
    // 2. Configure baud rate, 2 extra pins for custom signals, etc.
    return 0;
}

static ssize_t tlp_uart_read(struct file *file, char __user *buf,
                             size_t count, loff_t *ppos)
{
    // 1. Read data from internal buffer or directly from UART
    // 2. Copy to user space if needed
    return 0;
}

static ssize_t tlp_uart_write(struct file *file, const char __user *buf,
                              size_t count, loff_t *ppos)
{
    // 1. Send data over UART or handle TLP creation
    return count;
}

// Hypothetical function to process TLP
static int process_incoming_tlp(struct pcie_tlp *tlp)
{
    // Parse the TLP to see if it's Config Read, Config Write, Memory, etc.

    // Decide if we respond with shadow config or physical config
    // Possibly do forensic or malware analysis here

    // Modify fields for the completion TLP if needed
    // e.g., if (emulateEthernet) tlp->header[...] = <Ethernet-like data>

    return 0;
}

static irqreturn_t tlp_rx_irq_handler(int irq, void *dev_id)
{
    struct pcie_tlp rx_tlp;
    // 1. Retrieve TLP from UART or a ring buffer
    // 2. Call process_incoming_tlp(&rx_tlp)
    // 3. Send back the updated TLP to FPGA via UART
    return IRQ_HANDLED;
}

static struct file_operations tlp_fops = {
    .owner   = THIS_MODULE,
    .open    = tlp_uart_open,
    .read    = tlp_uart_read,
    .write   = tlp_uart_write,
};

static int __init tlp_emulation_init(void)
{
    int ret;

    // Register a character device or relevant subsystem
    // Setup interrupts / GPIO for FT232H additional pins

    printk(KERN_INFO "TLP Emulation Driver: Init\n");
    return 0;
}

static void __exit tlp_emulation_exit(void)
{
    // Cleanup resources, free IRQ, etc.
    printk(KERN_INFO "TLP Emulation Driver: Exit\n");
}

module_init(tlp_emulation_init);
module_exit(tlp_emulation_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Security & Forensics Team");
MODULE_DESCRIPTION("PCIe TLP Forwarding & Emulation Driver for Reverse Engineering");
```
