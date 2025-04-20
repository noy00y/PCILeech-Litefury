# Glossary + Fundamentals

Pertains to FPGA, x86, reverse engineering, decompiling, virtual machines, linux, etc...

**ACPI Page Table:**
[ACPI Software Programming Model](https://uefi.org/htmlspecs/ACPI_Spec_6_4_html/05_ACPI_Software_Programming_Model/ACPI_Software_Programming_Model.html)
- The ACPI page table is the middleman between the host's firmware and the running kernel
- The ACPI page table contains info about different device hardware such as the NIC, usbs, power system information, cpu stats, etc...
- By doing a full patch of this table and doing a full pass through for every NICs, USB and GPU, etc... that interfaces through your host --> as close as possible to being undetectable during static analysis

**CreateThread Function:**
```c
HANDLE CreateThread(
  LPSECURITY_ATTRIBUTES lpThreadAttributes,   // 1st arg: Security attributes 
  SIZE_T dwStackSize,                         // 2nd arg: Initial stack size (0 means default)
  LPTHREAD_START_ROUTINE lpStartAddress,      // 3rd arg: Address of thread start function
	                                            // we will be using the address specified here for various actions such as debugging, payloads, etc...
  LPVOID lpParameter,                         // 4th arg: Parameter passed to the thread
  DWORD dwCreationFlags,                      // 5th arg: Creation flags (0 = run immediately)
  LPDWORD lpThreadId                          // 6th arg: Pointer to receive thread ID
);

```

### Dynamic Analysis Overview
**DA Method #1 - Remote Kernel Debugging:**

```bash
┌────────────┐        TCP / COM Pipe        ┌──────────────┐
│  Host A    │            <———————————————► │  Host B      │
│  Ubuntu    │                              │  Windows /   │
│  QEMU‑KVM  │                              │  WinDbg x64  │
│  VM: Win10 │                              └──────────────┘
└────────────┘
```

- Host is our linux enviroment containing QEMU-KVM and our windows 10 guest OS
- We setup a virtual serial port (or KDNET adapter) at vm creation to communicate with a second windows vm on a different bridge. 
    - this faciliates a TCP/COM Pipe for the windows 10 guest OS to then perform communication with ``Windows /`` as well as ``WinDbg x64`` which was hiding
- Pros
    - No user-mode KD handles. The anti cheat can't enumerate \Devices\KD... and seee if a debugger is present even though we have enabled KD on the guest
    - Its easy to take dynamic snapshots. can revert snapshots instantly
    - full access to WinDbg engine
- Cons
    - ``The KdDebuggerEnalbed = 1`` flag is still flipped so we have to patch that. (some AC's may not catch it tho)
    - Requires 2 hosts during DA
    - boot time flag can stil lbe detected tho. If the AC polls at ring-0 (maybe only vanguard possibly). 

**DA Method #2 - Custom Hypervisor Debugger**
```bash
┌───────────────┐       VM‑exit / EPT violation
│    Hypervisor │  <─────────────────────┐
└───────────────┘                        │
        ▲                                │
        │                                │
┌───────┴───────┐   vmcall / normal exec │
│  Windows 10   │ ───────────────────────┘
│  (Ring‑0/3)   │
└───────────────┘
```
- Concept
    - Write (fork) a minimal Type 2 hypervisor that loads under the Windows kernel (vmx root)
    - All guest kernel code executes in the vmx non - root; you intercept the VM-exit events 
    or explicit `VMCALL` instructions you patch into the target code
    - Allows you to log, dump, modify memory w/o any windows debugger api, handles or KD flags
    - Capabilities
        - Set hidden breakpoints. Mark target page non executable in the EPT (extended page table). First fetch triggers EPT-violation; hypervisor logs and sets page back.
        - Single step entire kernel. Set the TF (trap flag) on guest-return or use Monitor Trap flag bit for hardware single step
        - Memory watchpoints. Remove write permission in EPT; VM-exit occurs on first write to struct
        - Code Patching. Modify guest memory in VM-exit handler, re-inject and resume 
    - Pros
        - Invisible to Windbg checks -> no `KD` device, no flags
        - Can spoof anything -> EPT can present fake code/data to anti-cheat, real code to your logger
        - Works even on bare-metal and doesn't rely on QEMU
    - Cons
        - Complexity of setup. Needs VMX/SVM; 2-5k lines of C/ASM
        - PatchGuard / DSE can be triggered if your not careful
        - Kernel level bsods
    - Performance costs
        - each vm-exit can be thousands of cpu cycles. This adds up if you single step frequently, but most modern CPUs can handle like 100k/s
- Vendors may ship a production driver that doubles as a mini hypervisor so the "Chair" can see everything while hiding from the AC
- Lots of maintenance goes into maintaining this driver against constant Windows updates + AC rootkit scans
- Walkthrough Overview
    1. You enable VMX in a tiny driver (`vmxon`, load a VMCS).
    2. You “launch” the current Windows kernel into **VMX non‑root** mode.
    3. The code runs *normally* until one of the events you care about triggers a **VM‑exit** to your hypervisor handler *(running in VMX root)*.
    4. In the exit handler you can:
        - read/modify guest registers & memory,
        - single‑step,
        - hide breakpoints,
        - patch code pages on the fly,
        - then `vmlaunch/vmresume` the guest to continue execution.

    Everything happens **below** Windows; AC’s kernel driver sees no WinDbg, no `INT 3`, no `DR7` breakpoints—only normal code flow.

**EPT (Extended Page Table):**
- Second-lvl address translation used by VT-x. You control per-page R/W/X bits. Denying a permission causes an EPT violation (VM-Exit)

**Fword:**

- `fword ptr` - refers to far word pointer
- 6 bytes total (2 byte segment selector and 4 byte offset)
- Used for read/write descriptor table registers

**General Purpose Registers:**

[Registers](https://www.eecg.utoronto.ca/~amza/www.mindsec.com/files/x86regs.html)

- ECX (counter register) → holds base pointer to structure (such as KDPC)
    - [ecx + _] → this means to store/load from this field inside p
- EAX (accumulator register)
    - typically used as a scratch register for temporarily holding params taken from local vars (eg. [ebp + 0Ch], [ebp + 10h] so they can moved into a structure
- LEA (load effective address)
    - calculate the address of a memory space and stores it in a register
    - does not load the contents of that location
    - This can be beneficial if we don’t want to update the CPU’s status flag (unlike ADD)
- REP (repeat register)
    - Used with string instructions such as movsb, ,movsw, etc…
    - When combined (eg. `rep movsd`) → tells the cpu to repeat that instruction ecx times

**Hypervisor:**

- also known as a virtual machine monitor
- abstracts the system's physical hardware (cpu, memory, etc...) so that multiple "guests" OS's can run indepentalty on the same machine
- type 1 (bare metal): runs directly on the host's hardware, with no general purpose OS underneath
- Type 2 (Hosted): runs ontop of the convential OS
    - virtual box, vmware workstations, parallels
- QEMU/KVM - this is a bit of a blend of both types since the vm is integrated in the kernel
    - the kernel manages the virtualization and provides a OS

**Interrupt Descriptor Table:**

- The IDT register is 6 bytes (48 bits) long
- first 2 bytes → Size of the IDT (how many descriptors it holds)
- next 4 bytes → Physical address of the IDT in memory

**Kernel Deferred Procedure Call:**

- Data structured used by windows kernel to schedule “deferred” work at a later time
- This deferred work runs at a lower priority → Lower interrupt request level (IRQL)
- eg. When a certain high prioirty / interrupt code runs, it has the option of queuing up a KDPC that will run when the system determines it convenient
- DPC is a small pckt of work to be done
    - holds a pointer to the data/context needed for the routine as well as the specific routine operation to perform
    - **Windows API:**  `KeInitializeDpc`, `KeInsertQueueDpc`
- KeInitializeDpc
    - This function prepares a `_KDPC` structure to be used for the dpc routine that are scheduled to run later at dispatch_lvl, usually by the kernel scheduler or i/o subsystem
- _KDPC Fields
    - `TargetInfoAsUlong`: packed bitfield which encodes type, importance and processor target info
    - `DpcListEntry.Prev`: used when inserted into DPC queue (double linked list) 
    - `DeferredRoutine`: pointer to the function that will be called when the DPC is executed   
    - `DefferedContext`: passed to DefferedRoutine when it is invoked
    - `DpcData`:  Kernel internal metadata (tracking if the DPC is active or pending)


**Kernel Debugger:**

- `_KdLogBuffer` - global array used by the windows kernel used for logging kernel events

**Nt!KiInitSystem:**

- Internal windows kernel routine for performing system lvl initiation
- During early system start up, the kernel needs to configure various data structures (eg. service descriptor table) before the OS is fully operational.


**SCAS and STOS Instructions:**

- SCAS (Scan String)
    - Overview
        - Compares a register (AX, AL, EAX) against the memory byte/word/dword at an address pointed to by EDI
        - Typically used to find a particular value in a string or array
        - Variants of SCAS include `SCASB` (scan string byte), `SCASW` (word), `SCASD`, `SCASQ` (quad word)
    - What “compare” means
        - internally SCAS performs the subtraction → (accumulator register) - (memory pointed to by EDI)
        - The result of this subtraction affects the CPU flags
    - SCAS is typically used with `repne` / `repe` (repeat not equal, repeat equal) until a match or mismatch is found
- STOS (Store String)
    - Stores the contents of an accumulator register into memory at the address pointed to by EDI
    - Used to quickly initialize or fill blocks of memory with a certain value
    - Unlike SCAS, STOS does not perform a compare and simply writes data from the register to memory
    - STOS is used with rep, repeating until ecx == 0

**Stack:**

Heres what the stack looks like

```nasm
[High Memory]
...
| Function arguments (from caller)
|--------------------|
| Return address     |
|--------------------|
| Old EBP            |  ← saved by `push ebp` (line 03)
|--------------------|
| Local variables    |  ← created by `sub esp, 130h` (line 05)
|--------------------|
| Temporary values
|--------------------|
| Saved registers    |  ← eg `push edi` (line 06)
|--------------------|
[Low Memory]
```

```nasm
[HIGH MEMORY]
|-----------------------------|
| arg2            [ebp + 0Ch] |
| arg1            [ebp + 08h] |
| return address  [ebp + 04h] |
| saved old EBP   [ebp]       | ← current EBP
| local var 1     [ebp - 04h] |
| local var 2     [ebp - 08h] |
| ...             [ebp -130h] |
| pushed edi      [esp]       | ← current ESP
|-----------------------------|
[LOW MEMORY]
```

**Store Interrupt Descriptor Table:**

- Stores the Interrupt Descriptor Table Register into memory
- Interrupt Descriptor Table → used by processor to handle interrupts and exceptions
- This instruction is privileged so it runs in ring 0 (kernel level)

**Trap Flag**
- Bit 0 of `RFLAGS`
- When set, CPU raises a #DB (debug exception) after each instruction - classic single step
- MTF - Monitor Trap Flag
    - A VMCS control bit. Tells hardware to VM-exit after one guest instruction with 0 overhead of #DB or patching TF

**VT-x Intel Terminology**
- VMX Root refers to hypervisor code
- VMX Non Root is the guest OS (windows OS)
- VMCALL -> special instruction the guest executes to voluntarily trap the hypervisor (ie. hypervisor syscall)
- VM-exit -> Any event (CPUID, MSR access, EPT violation, external interrupt, or your `VMCALL`) that makes the CPU leave VMX non-root and enter root to run your handler