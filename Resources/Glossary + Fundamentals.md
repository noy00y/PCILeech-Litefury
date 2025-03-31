# Glossary + Fundamentals

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
