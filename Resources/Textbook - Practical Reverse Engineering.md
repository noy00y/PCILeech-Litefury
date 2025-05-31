# Textbook - Practical Reverse Engineering

# Practical Reverse Engineering Notes:

# Chapter 1 – x86 and x64

- x86 is the 32 bit implementation of the intel architecture
- It can operate is real mode or protected mode
    - Real – processor state when first powered on
    - Protected mode – supports virtual memory, paging, etc…

## Register Sets and Data Types

- When operating in protected mode, x86 uses eight 32 bit general purpose registers (GPRs)
    
    [](https://lh7-rt.googleusercontent.com/docsz/AD_4nXfvIMNB2y4n2mTCSeBEJtAp2wVkxtXYJZz2HxLiVPFvKKHKOJeNcqt8SUX7j_69w8XhV_iRr_hpXlDC8pv3rof-D2fNO_UJTE0Lvq0Exh25Lpcjk73Ye95TugxNMJBkle6Xrx06?key=L0WoYa1WarcY3pdciZzHeHql)
    
- EAX, EBX, ECX, EDX, EDI, ESI, EBP, and ESP
- Some of these GPRs can be further subdivided into 8 bit and 16 bit registers
- Instruction pointer is stored in the EIP register

| **Register** | **Purpose** |
| --- | --- |
| ECX | Counter in loop |
| ESI | Source in string/memory operation |
| EDI | Destination in string/memory operation |
| EBP | Base frame pointer |
| ESP | Stack pointer |
- Common data types
    - Byte -> 8 bits (AL, BL, CL)
    - Word -> 16 bit (AX, BX, CX)
    - Double Word -> 32 bit (EAX, EBX, ECX)
- 32 bit eflag is used to store the status of arithmetic operations and other execution state (eg. Trap flag)
    - Eg. If the previous “add” operation resulted in 0 -> zf flag is set to 1
    - Flags in eflags are primarily used to implement conditional branching
- There are also registers which control low level system mechanisms such as
    - CR0 -> controls if paging is on or off
    - CR2 -> contains the linear address which caused a page to fault
    - CR3 -> base address of the paging data structure
    - CR4 -> controls hardware virtualization settings
    - DR0-7 -> used to set memory breakpoints
- Model Specific Registers (MSRs)
    - Each MSR is specific to platform (AMD/intel) and is identified by name and a 32 bit number
    - MSR is read/written to through the RDMSR/WDMSR instruction
    - Accessible only to code running in kernel mode and for low level functionality
    - Eg. Sysenter instruction transfers execution to the address stored in the IA32_sysenter_eip MSR (0x176), which is usually the OS’s system call handler

## Instruction Set

[](https://lh7-rt.googleusercontent.com/docsz/AD_4nXdDzoJJJOZCipm9lpeW_wdBVdZ1bvE1DjPipngtFFj--Rxg_QO7YFBKq9AAaLdvnQ0K8Q_zhNsiLv0ZhhamXWcIHbZ6xaDIEDsWlTTEU3sixr3IMmyuDaWmGGfxr1RjsdUaQu783A?key=L0WoYa1WarcY3pdciZzHeHql)

- This allows for high level of flexibility in terms of moving data b/w registers and computer memory
- This movement can be classified as one of the following
    - Intermediate to register
    - Register to register
    - Intermediate to memory
    - Register to memory
    - Memory to memory (specific only to x86)
- ARM can only do the first 4

## Data Movement

- Instructions operate on values that come from registers or main memory
- Eg. Using the most common instruction *MOV*

```nasm
- 01: BE 3F 00 0F 00	mov esi, 0F003Fh ; set ESI = 0xF00
- 02: 8B F1	mov esi, ecx ; set ESI = ECX
```

- Moving data to and from memory
- X86 utilizes [] brackets to indicate memory access
- Eg 1. Common operation and its pseudo
    - 
        
        [](https://lh7-rt.googleusercontent.com/docsz/AD_4nXdLLompOtV5Rzt3mT8LE6q88_oug1SLk3kxQia16bSmpokpRtAH5zo83yh7wSaLhPQAE8cORD_oy6IkYM5LU5M5qExqH-zO2356YotXRIN0Y3X_xGRZoXOrh2-YkhFIs29mrn25kg?key=L0WoYa1WarcY3pdciZzHeHql)
        
    - 
        
        [](https://lh7-rt.googleusercontent.com/docsz/AD_4nXd1njmeSpXROVEAzF86DfmwX6s8oGicEYsaPZb2mE3SvrU9H7t8dGClLVEhM2y0PItx6eGeUWd3pKyemcBdBtADyL9iuBec4QS0Jz5WCXRzF31CYaXi9PZ06DQjJvRrz-aLKYmPSg?key=L0WoYa1WarcY3pdciZzHeHql)
        
    - This is a good demonstration of memory access using a base register and a offset
    - We use this to access data buffers at a location computed during runtime (dynamic memory locations)
- Eg. 2 Suppose ECX points to a structure of type KDPC with the layout
    
    [](https://lh7-rt.googleusercontent.com/docsz/AD_4nXcAipraxIYuW_e0Sr-ogvdEbKSFP_XVmPKpUz3FQ_NGU3SHOo1TAKCtPeW9YZDn4GuEpxS6eQ4n3Elhjw4dP0ec53_f49UOOGR1U8vIdkXiIoEd_TW2oTONuBaIT5L__zDqzDZcmA?key=L0WoYa1WarcY3pdciZzHeHql)
    
- 01: 8B 45 0C mov eax, [ebp+0Ch]
    - § Load into EAX, whatever was stored at [ebp + 0Ch]
    - § Usually this comes from function params or return vars
- 02: 83 61 1C 00 and dword ptr [ecx+1Ch], 0
    - § DpcData field is zero’d out by ANDing it with 0
    - § DpcData field is 4 bytes stored at offset 0x1C from ecx
    - § In KDPC terms, ecx+0x1C is the p->DpcData field, and we are setting it to NULL
- 03: 89 0C 08 mov [ecx+0Ch], eax
    - § The value in eax is stored in [ecx + 0Ch]
    - § Ie. p->defferedRoutine = <value in eax>
- 04: 8B 45 10 mov eax, [ebp+10h]
    - § Load the next param / local into EAX
    - § Used as DefferedContext
- 05: C7 01 13 01 00 ... mov dword ptr [ecx], 0x113
    - § Double word value 0x113 is written to the base of the structure
    - § This sets the 3 fields (Number, Importance and Type)
    - 
        
        [](https://lh7-rt.googleusercontent.com/docsz/AD_4nXekMgKxyQIfz8tsRZ2OmMM79lKI6LAVpdo8jYtAZeAQaTHlPjaKu3YvsUWYuuERvJxoUi4KaCi2kq1zGcNHNVOwhtoNjbqxVnEuXIN-mFXYzIVkt_aauPJpmPYQEVA7VKiR5VwfMA?key=L0WoYa1WarcY3pdciZzHeHql)
        
    - § The compiler was able to set these 3 fields with just 1 instruction since it knew the field constants ahead of time. Normally this would have taken 3 instructions and an extra 7 bytes of space
- 06: 89 41 10 mov [ecx+10h], eax
    - § Setting DefferedContext field to what was stored in eax
- Memory access can be performed at 3 different levels
    - Byte level
    - Word level
    - Double world level
- Format for accessing array type objects is: [base + index * scale]. See eg’s below
    - 01: 8B 34 B5 40 05+ mov esi, _KdLogBuffer[esi*4]
        - § We typically use esi (extended source register) for copying string and memory arrays
        - § ; always written as mov esi, [_KdLogBuffer + esi * 4]
        - § ; _KdLogBuffer is the base address of a global array and we are accessing the element stored at the index (esi * 4)
        - § ; we know that each element in the array
        - § ; is 4 bytes in length (hence the scaling factor)
    - 02: 89 04 F7 mov [edi+esi*8], eax
        - § ; here is EDI is the array base address; ESI is the array
        - § ; index; element size is 8.
- Eg. 3 Code looping over an array

```nasm
- 01: loop_start:
- 02: 8B 47 04 mov eax, [edi+4]
- 03: 8B 04 98 mov eax, [eax+ebx*4]
- 04: 85 C0 test eax, eax
- 05: 74 14 jz short loc_7F627F
- 06: loc_7F627F:
- 07: 43 inc ebx
- 08: 3B 1F cmp ebx, [edi]
- 09: 7C DD jl short loop_start
```

- Explanation
    - § Line 2 reads double wrd from [edi + 4] and uses it as the base address into an array in line 3
    - § EDI register is likely a structure storing an array at +4 offset
    - § Line 7 increments the index
    - § Line 8 compares the index against a value at +0 offset in the same structure
- Given this info we can decompile this loop as follows:

```c
typedef struct _FOO
{
	DWORD size; // +0x00
	DWORD array[...]; // +0x04
} FOO, *PFOO;

PFOO bar = ...;

for (i = ...; i < bar->size; i++) {
	if (bar->array[i] != 0) {
	...
	}
}
```

- Movsb, movsw and movsb -> move single byte/word/double-wrd
    - These instructions move data with 1, 2 or 4 byte granularity between 2 memory addresses
    - EDI/ESI are implicitly used for destination/source address depending on the direction flag (DF) in EFLAGS
    - If DF is 0 -> the addresses are decremented, else incremented
    - These instructions are usually used for implementing string or memory copying functions when the length is known at compile time
    - Can be accompanied by REP prefix -> repeats an instruction up to ECX times
- Eg 4.

Assembly:

```nasm
01: BE 28 B5 41 00 mov esi, offset _RamdiskBootDiskGuid
; ESI = pointer to RamdiskBootDiskGuid
02: 8D BD 40 FF FF+ lea edi, [ebp-0C0h]
; EDI is an address somewhere on the stack
03: A5 movsd
; copies 4 bytes from EDI to ESI; increment each by 4
04: A5 movsd
; same as above
05: A5 movsd
; save as above
06: A5 movsd
; same as above
```

Equivalent C Code:

```c
/* a GUID is 16-byte structure */
GUID RamDiskBootDiskGuid = ...; // global
...
GUID foo;
memcpy(&foo, &RamdiskBootDiskGuid, sizeof(GUID));
```

- Explanation
    - § In line 2 -> although the lea instruction uses [] it does not read from a memory address but rather evaluates the expression in the square brackets and puts the result in the destination register
    - § if EBP was 0x1000 then EDI would be 0xF40 (=0x1000 – 0xC0) after executing line 2
- Eg 5. Nt!KiInitSystem uses the REP prefix

```nasm
01: 6A 08 push 8 ; push 8 on the stack (will explain stacks later)
02: ...
03: 59 pop ecx ; pop the stack. Basically sets ECX to 8.
04: ...
05: BE 00 44 61 00 mov esi, offset _KeServiceDescriptorTable
06: BF C0 43 61 00 mov edi, offset _KeServiceDescriptorTableShadow
07: F3 A5 rep movsd ; copy 32 bytes (movsd repeated 8 times)
; from this we can deduce that whatever these two objects are, they are
; likely to be 32 bytes in size.
```

The rough C equivalent of this would be as follows:

memcpy(&KeServiceDescriptorTableShadow, &KeServiceDescriptorTable, 32);

- ECX serves as the loop counter for the rep movsd instruction
- This is done by first pushing 8 and then popping that 8 into ecx
- Line 5 -> Loads the base address (offset) of the descriptor table into the esi register. ESI is used as the source pointer for the subsequent copy operation (rep movsd)
- Since each rep movsd moves a double word (size 4 bytes) and we are doing this 8 times -> we can conclude 32 bytes are being transferred from esi to edi
- each time we move a double word from esi to edi we increment the index by 4 and decrement ecx
- This is essentially the assembly implementation of memcpy
- Eg. 6 nt!MmInitializeProcessAddressSpace using a combination of instructions since the copy size is not a multiple of 4

```nasm
01: 8D B0 70 01 00+ lea esi, [eax+170h]
; EAX is likely the base address of a structure. Remember what we said
; about LEA ...
02: 8D BB 70 01 00+ lea edi, [ebx+170h]
; EBX is likely to be base address of another structure of the same type
03: A5 movsd
04: A5 movsd
05: A5 movsd
06: 66 A5 movsw
07: A4 movsb
```

```c
Equivalent C Code (GPT created):
// "Structures" or arrays at EAX and EBX, offset by 0x170 each
memcpy((void *)(ebx + 0x170), (void *)(eax + 0x170), 15);
```

- load esi w/ the address [eax + 170h]. This is likely some structure
- load in another structure into edi
- line 3 - 5 -> moving a 4 byte (dword) from [esi] to [edi] and then incrementing both. In total 12 bytes were copied (4 x 3)
- Line 6 - 7 -> moving a word (2 bytes) and then a just 1 byte
- In total we have moved 15 bytes from esi to edi
- We use LEA to place the effective address [eax + 170h] into the esi without affecting any flags
- SCAS and STOS Instructions
    - These are a class of data movement registers with implicit source and destination
    - Similar to MOV, these can use a granularity of 1, 2 or 4 bytes
    - SCAS implicity compare AL/AX/EAX with data starting at the memory address EDI
- Eg 7. Implementing C function strlen()

```nasm
01: 30 C0 xor al, al
; set AL to 0 (NUL byte). You will frequently observe the XOR reg, reg
; pattern in code.
02: 89 FB mov ebx, edi
; save the original pointer to the string
03: F2 AE repne scasb
; repeatedly scan forward one byte at a time as long as AL does not match the
; byte at EDI when this instruction ends, it means we reached the NUL byte in
; the string buffer
04: 29 DF sub edi, ebx
; edi is now the NUL byte location. Subtract that from the original pointer
; to the length.
```

- line 1: xor al, al
    - prepares a search byte of 0x00 to look for the end of a c-string (the \0 terminator)
    - xor reg, reg -> common way to *zero out* a register
- line 2: mov ebx, edi
    - set ebx to the same address at edi to preserve the starting address of the string
    - so now ebx = og string pointer and edi is still at the start
- line 3: repne scasb
    - *scan string byte and repeat while not equal (Zero Flag = 0)*
    - scasb compares AL (which is 0) with the byte at [edi] then increments edi by 1
    - scasb keeps scanning until
        - a match is found (AL == [EDI] == 0)
        - ECX becomes 0 → meaning you scanned the entire string which is the len ecx was set too
- line 4: sub edi, ebx
    - After the scasb completes → edi has advanced to or just past the null terminator \0
    - Subtracting ebx (og pointer) from EDI yields the distance between the start and end of the string. Ie. this gives you the string length

Pseudo C Code Equivalent

```c
size_t my_strlen(const char *str)
{
char *start = (char *)EDI;
// Search for '\0'
// while (*EDI != 0) EDI++;
// repne scasb does this behind the scenes.
return (char *)EDI - start;
}
```

- STOS functions similarly to SCAS except that it writes the value AL/AX/EAX to EDI.
- used to initialize a buffer to a constant value (eg. memset())
- LOD (load string) - load from [esi] into al/ax/eax
- STOS and LOD differ in the sense that
    - STOS → takes the value in eax and writes it into [edi]. EDI is then incremented/decremented. STOS **writes** to memory
        - Used to implement memset() which fills a buffer with a specific value
    - LODS → Loads from [esi] into eax. ESI is then incremented. LODS **reads** from memory
        - used to implement memcpy() which reads data from memory sequentially
- Eg 8. Here's an example of such

```nasm
01: 33 C0 xor eax, eax
; set EAX to 0
02: 6A 09 push 9
; push 9 on the stack
03: 59 pop ecx
; pop it back in ECX. Now ECX = 9.
04: 8B FE mov edi, esi
; set the destination address by pointing edi to the same location as esi
05: F3 AB rep stosd
; write 36 bytes of zero to the destination buffer (STOSD repeated 9 times)
; double word (4 bytes) x 9 times = 36 bytes
; this is equivalent lent to memset(edi, 0, 36)
```

Equivalent Pseudo C: memset( (void *)EDI, 0, 36 );

- mov edi, [ebp + 8] → this instruction retrieves a function param from the stack and places it in the edi register
    - ebp - stack params

## Arithmetic Operations

**Left and right bit shifting**

- Shifting operations involve either multiplying (left shift) or dividing (right shift) by the power of 2.
- Shifting Left -> Moves bits to the left, discards the leftmost bit and fills the rightmost bit with 0s
- Shifting Right -> Moves bits to the right, discards the rightmost bit and fills the leftmost bit with 0s
- Logical Shifts (SHL/ SHR) -> Fills new empty positions with 0s and treats new data as 0s
- Arithmetic Shift (SAL or SAR) -> for a right shift we fill the new empty position with the sign bit (left most bit) to preserve the sign of the number
- Eg.

```nasm
MOV R1, #10 ; move 00001010 into R1
SHL R1, #1 ; shifts left by 1 -> equivalent to multiplying the number by 2^1 = 2. Now R1 = 00010100
SHR R1 #2 ; shift right by 2 -> equiv of dividing by 2^2 = 4. Now R1 = 00000101
```

**Unsigned and Signed Multiplication**

- Done w/ MUL and IMUL
- MUL has the form: MUL reg/memory and can operate on either
- Eg 1

```nasm
01: F7 E1 mul ecx ; EDX:EAX = EAX * ECX
02: F7 66 04 mul dword ptr [esi+4] ; EDX:EAX = EAX * dword_at(ESI+4)
03: F6 E1 mul cl ; AX = AL * CL
04: 66 F7 E2 mul dx ; DX:AX = AX * DX
```

- Line 3 -> uses the 8 bit form of mul and multiplies AL (low byte of EAX) by CL (low byte of ECX). Resulting 16 bit product is then placed in AX
- What does something like *EDX:EAX* or *DX:AX* mean
    - result is split across 2 registers
    - EAX holds the lower 32 bit of the product and EDX holds the upper 32 bits
- Eg 2

```nasm
01: B8 03 00 00 00 mov eax,3 ; set EAX=3
02: B9 22 22 22 22 mov ecx,22222222h ; set ECX=0x22222222
03: F7 E1 mul ecx ; EDX:EAX = 3 * 0x22222222 =
; 0x66666666
; hence, EDX=0, EAX=0x66666666
04: B8 03 00 00 00 mov eax,3 ; set EAX=3
05: B9 00 00 00 80 mov ecx,80000000h ; set ECX=0x80000000
06: F7 E1 mul ecx ; EDX:EAX = 3 * 0x80000000 =
; 0x180000000
; hence, EDX=1, EAX=0x80000000
```

- We store the result in edx:eax for 32 bit multiplication since the resulting answer may not fit in just 1 single 32 bit register -> as seen from lines 4-6 in eg 2
- IMUL has 3 forms
    - IMUL reg/mem -> same as MUL
    - IMUL reg1, reg2/mem -> reg1 = reg1 * reg2/mem
    - IMUL reg1, reg2/mem, imm -> reg1 = reg2 * imm
        - In this last form of IMUL, you multiply reg2 or mem by some imm *intermediate constant* before storing that result in reg1
- Eg 3. Some disassemblers may shorten the parameters as well

```nasm
01: F7 E9 imul ecx ; EDX:EAX = EAX * ECX
02: 69 F6 A0 01 00+ imul esi, 1A0h ; ESI = ESI * 0x1A0
03: 0F AF CE imul ecx, esi ; ECX = ECX * ESI
```

**Unsigned and Signed Division**

- Jargon Overview
    - eg. 10 / 2 = 5
    - Operand - 10 and 2
    - Dividend - 10
    - Divider - 2
    - Quotient - 5
- Done w/ DIV and IDIV. Div - unsigned and Idiv is signed
- Dividend is implicitly stored in a register and only the divider is provided in the instruction
- Register storing depends on the operand size

| Operand Size (Divisor) | Dividend Register | Quotient Register | Remainder Register |
| --- | --- | --- | --- |
| 8-bit (div r/m8) | AX | AL | AH |
| 16-bit (div r/m16) | DX:AX | AX | DX |
| 32-bit (div r/m32) | EDX:EAX | EAX | EDX |
- Eg 1.

```nasm
01: F7 F1 div ecx ; EDX:EAX / ECX, quotient in EAX,
02: F6 F1 div cl ; AX / CL, quotient in AL, remainder in AH
03: F7 76 24 div dword ptr [esi+24h] ; see line 1
04: B1 02 mov cl,2 ; set CL = 2
05: B8 0A 00 00 00 mov eax,0Ah ; set EAX = 0xA
06: F6 F1 div cl ; AX/CL = A/2 = 5 in AL (quotient),
; AH = 0 (remainder)
07: B1 02 mov cl,2 ; set CL = 2
08: B8 09 00 00 00 mov eax,09h ; set EAX = 0x9
09: F6 F1 div cl ; AX/CL = 9/2 = 4 in AL (quotient),
; AH = 1 (remainder)
```

## Stack Operations and Function Invocation

- Stack is LIFO (Last in first out)
- In x86 -> the stack is a contiguous section of memory pointed to by ESP register (stack pointer)
- The stack grows downwards from the pointer
- Push/pop instructions implicity modify the esp register
    - push decrements esp and then writes data to the location pointed to by esp
    - pop reads the data and then increments esp
    - Stack **grows downwards** that's why we decrement esp when we push to the stack and increment when we pop
- default increment/decrement value is 4, but this can be changed w/ a prefix override
- Eg. 1

```nasm
; initial ESP = 0xb20000
01: B8 AA AA AA AA mov eax,0AAAAAAAAh
02: BB BB BB BB BB mov ebx,0BBBBBBBBh
03: B9 CC CC CC CC mov ecx,0CCCCCCCCh
04: BA DD DD DD DD mov edx,0DDDDDDDDh
05: 50 push eax
; address 0xb1fffc will contain the value 0xAAAAAAAA and ESP
; will be 0xb1fffc (=0xb20000-4)
06: 53 push ebx
; address 0xb1fff8 will contain the value 0xBBBBBBBB and ESP
; will be 0xb1fff8 (=0xb1fffc-4)
07: 5E pop esi
; ESI will contain the value 0xBBBBBBBB and ESP will be 0xb1fffc
; (=0xb1fff8+4)
08: 5F pop edi
; EDI will contain the value 0xAAAAAAAA and ESP will be 0xb20000
; (=0xb1fffc+4)
```

[](https://lh7-rt.googleusercontent.com/docsz/AD_4nXejFDykfPy-ysL4Uv_MoIfd2NLeTGBwplrOfMJaIl69tFnJffJtMhVpyU72bYhLSG4jm-vt0n-x62VNbKD0JHbg6nD55h6RPSQJFnIYpRE9HlGxkKo4U6lTyhSlj9rNXvbJy9bf?key=L0WoYa1WarcY3pdciZzHeHql)

- Line 5 -> ESP is decremented by 4 which now becomes 0xb1fffc. The value in EAX (0xAAAAAAAA) is now stored in this location. Same thing for line 6
- Line 7 -> Value at ESP *which is currently 0xb1fff8,* is read and then ESP is incremented. Same thing for line 8
- ESP can be modified by other instructions such as ADD or SUB
- Functions are implemented at the machine level using this stack data structure
- Eg 2. Consider the following C Code

```c
int __cdecl addme(short a, short b)
{
	return a+b;
}
```

```nasm
01: 004113A0 55 push ebp ; save the caller’s base pointer
02: 004113A1 8B EC mov ebp, esp ; set up the stack frame
03: ...
04: 004113BE 0F BF 45 08 movsx eax, word ptr [ebp+8] ; Load ‘a’ into EAX, sign extended
05: 004113C2 0F BF 4D 0C movsx ecx, word ptr [ebp+0Ch] ; Load ‘b’ into ECX, sign extended
06: 004113C6 03 C1 add eax, ecx ; EAX = EAX + ECX
07: ...
08: 004113CB 8B E5 mov esp, ebp ; Restore ESP to its original position
09: 004113CD 5D pop ebp ; Restore the caller’s EBP
10: 004113CE C3 retn ; return to the caller
```

Function is invoked with the following code:

`sum = addme(x, y);`

Assembly:

```nasm
01: 004129F3 50 push eax ; push a
02: ...
03: 004129F8 51 push ecx ; push b
04: 004129F9 E8 F1 E7 FF FF call addme ; call the func
05: 004129FE 83 C4 08 add esp, 8 ; adjust esp (caller cleans the stack)
```

- __cdecl calling convention
    - pushes arguments onto the stack from right to left.
    - caller cleans up the stack after the function call
    - return value stored in eax
- Stack layout for params (cdecls)
    - [EBP] - prev base pointer
    - [ebp + 4] - return address (saved by call instruction)
    - [ebp + 8] - first arg (short a)
    - [ebp + 12] - second arg (short b)
- Line 1 -> first we store the stack base pointer (EBP) allowing access to the caller’s stack
- Line 2 -> Then EBP is set as the new frame pointer (start of stack) for easy stack navigation
    - ESP (stack pointer) holds the top of the stack and EBP (base pointer) provides stable reference for local vars and function params
- Line 4-5 -> load both 16 bit shorts. A is loaded into EAX and B into ECX. We use the *sign extends* MOVSX since it loads the shorts as 32 bit integers. This is done since the function returns a int
- Line 8 -> Restores ESP back to its previous state before the function's execution. This allows us to back track on the stack and revert back to the original top of stack
- Line 9-10 -> Restores the callers base pointer, pop the return address from the stack and jump back to the original caller

Function Invocation from Eg 2

- Line 1-3 -> push args onto the stack and decrement esp by 4 bytes for each arg
    - args are pushed onto the stack in reverse order due to cdecls convention
- Line 4 -> Pushes the return address onto the stack and jumps to addme
- Line 5 -> Since cdecls requires the *caller to clean up the stack*, esp is incremented by 8 (2 args * 4 bytes). This restores ESP back to its original state before the function call
- Call instruction
    - pushes the return address (address immediately after the call instruction) onto the stack
    - Changes EIP (Extended Instruction Pointer) to the call destination which transfers control to the call target and proceeds executing from there
- RET instruction
    - Pops the address stored at the top of the stack into EIP and transfers control to it (ie. *pop EIP*). This returns control back to the calling function
- Calling Conventions
    - cdecls -> pushes onto the stack from right to left. Caller cleans up the stack afterwards
    - stdcall -> callee must clean up the stack
    - fastcall -> first 2 params are passed into the ecx and edx registers and the rest of the params are passed onto the stack

## Control Flow

- Control flow is implemented using CMP, TEST, JMP, JCC and EFLAGS
- Common EFLAGS
    - ZF - zero flag; set if the result of the previous arithmetic operation is 0
    - CF - carry flag; set when the result requires a carry. Applies to unsigned numbers
    - SF - sign flag; set to the most significant bit of the result
    - OF - overflow flag; set if the result overflows the max size. Applies to signed numbers
- EFLAGS are updated via arithmetic instructions based on the result
- Jcc instructions - jump given a conditional code
- Common Conditional Codes
    - b/nae - below/neither above nor equal. Used for unsigned operations
    - nb/ae - not below / above or equal. Used for unsigned operations
    - e/z - equal / zero
    - NE / NZ not equal / not zero
    - L - less than / neither greater or equal. Used for signed operations
    - GE / NL - greater or equal / not less then. used for signed operations
    - G / NLE - greater / not less nor equal. used for signed operations
- If-Else -> constructs are quite simple to recognize since they involve a compare / test followed by a jcc
- Eg 1.

```nasm
01: mov esi, [ebp+8] ; load first function argument into esi
02: mov edx, [esi] ; load the value at [esi] into edx (edx = *esi)
03: test edx, edx ; check if edx == 0
04: jz short loc_4E31F9 ; jump to cleanup/return if edx == 0
05: mov ecx, offset _FsRtlFastMutexLookasideList ; load ecx with a list of memory blocks associated exallocate
06: call _ExFreeToNPagedLookasideList@8 ; frees memory allocated from the non - paged lookaside list. ie. free memory in the structure
07: and dword ptr [esi], 0 ; set first field of the structure to 0 (*esi = 0)
08: lea eax, [esi+4] ; load ‘esi + 4’ into eax
09: push eax ; push eax as a arg into the next function
10: call _FsRtlUninitializeBaseMcb@4 ; func call
11: loc_4E31F9: ; function epilogue for cleanup and return
12: pop esi
13: pop ebp
14: retn 4
15: _FsRtlUninitializeLargeMcb@4 endp
```

Pseudo C

```nasm
if (*esi == 0) {
return;
}
ExFreeToNPagedLookasideList(...);
- esi = 0;
...
return;
OR
if (*esi != 0) {
...
ExFreeToNPagedLookasideList(...);
*esi = 0;
...
}
return;
```

- Non-paged lookaside lists
    - part of the windows kernel
    - memory allocation mechanism for objs that are frequently allocated and freed
    - When a object is freed -> instead of returning the memory to the heap, it is placed in a lookaside list, so that when it is inevitably allocated again, the kernel can first pull memory from this list before making a expensive heap allocation
- Sometimes when dealing with many if/else or switch statements, the compiler may find it cheaper to build a jump table to reduce the # of comparisons. The jump table is an array of function pointers (addresses), each pointing to a handler for a specific case. An example of this below
- Eg 2.

```nasm
01: cmp edi, 5 ;
02: ja short loc_10001141 ; *jump if above*, ie. if edi > 5 → jump to loc_…141. this is the default case
03: jmp ds:off_100011A4[edi*4] ; program jumps to the address stored at off_1…A4[edi*4]. ie index into the jump table using the edi value + 4 byte long address
04: loc_10001125:
05: mov esi, 40h ; esi = 0x40
06: jmp short loc_10001145 ; exit switch statement
07: loc_1000112C:
08: mov esi, 20h
09: jmp short loc_10001145
10: loc_10001133:
11: mov esi, 38h
12: jmp short loc_10001145
13: loc_1000113A:
14: mov esi, 30h
15: jmp short loc_10001145
16: loc_10001141: ; this will execute if we initially jumped from EDI > 5
17: mov esi, [esp+0Ch] ; esi = *(esp + 0xC)
18: ...
19: off_100011A4 dd offset loc_10001125
20: dd offset loc_10001125
21: dd offset loc_1000113A
22: dd offset loc_1000112C
23: dd offset loc_10001133
24: dd offset loc_1000113A
```

Pseudo C:

```c
switch(edi) {
case 0:
case 1:
// goto loc_10001125;
esi = 0x40;
break;
case 2:
case 5:
// goto loc_1000113A;
esi = 0x30;
break;
case 3:
// goto loc_1000112C;
esi = 0x20;
break;
case 4:
// goto loc_10001133;
esi = 0x38;
break;
default:
// goto loc_10001141;
esi = *(esp+0xC)
break;
}
```

- Loops are implemented using a combination of JCC and JMP instructions. ie. implemented using if/else and goto constructs

**Eg 3.**

C Code:

```c
for (int i=0; i<10; i++) {
	printf("%d\n", i);
}
printf("done!\n");
```

Assembly Code:

```nasm
01: 00401002 mov edi, ds:__imp__printf ; address of printf function imported dynamically into edi. EDI now acts as the function pointer to printf
02: 00401008 xor esi, esi ; zero out esi → this will be used as the loop counter (ie. i = 0)
03: 0040100A lea ebx, [ebx+0] ; placeholder for alignment (whatever that means)
04: 00401010 loc_401010: ; label for loop. Entry point of the loop body
05: 00401010 push esi ;
06: 00401011 push offset Format ; "%d\n" ;
07: 00401016 call edi ; __imp__printf ; call print function after pushing the counter and format to the stack. This is the equivalent of passing both as args to printf()
08: 00401018 inc esi ; increment loop counter
09: 00401019 add esp, 8 ; moves the pointer to the stack 8 bytes down to clean up the 2 args passed into printf (“%d\n”, i). We have to clean up the stack since cdecls is used
10: 0040101C cmp esi, 0Ah ; compares loop counter to 10, ie . i < 10
11: 0040101F jl short loc_401010; if esi < 10, jump back to the start of the loop. Since we used jl *jump if less then* we know the comparison was being done with signed integers
12: 00401021 push offset aDone ; "done!\n" ; push a str onto the stack
13: 00401026 call edi ; __imp__printf ; call print function
14: 00401028 add esp, 4 ; incrementing stack pointer removes done from the stack
```

**Example 4**

Assembly

```nasm
01: sub_1000AE3B proc near ; declares start of function routine

; save both edi and esi onto the stack
02: push edi
03: push esi

04: call ds:lstrlenA ; call windows api func to get str len and store in eax
05: mov edi, eax ; move this result into edi

; Zero out ecx and edx
06: xor ecx, ecx ;  ECX will track where chars are stored
07: xor edx, edx ;  EDX will track which chars to copy

08: test edi, edi ; checks if edi == 0
09: jle short loc_1000AE5B ; if edi <= 0 --> jump to loc_1...5B where str[0] = 0 and return

; Looping through the str
10: loc_1000AE4D: ; 
11: mov al, [edx+esi] ; fetches char at index j = edx. ie (str[j])
12: mov [ecx+esi], al ; stores it at index i = ecx. ie. str[i] == str[j]
13: add edx, 3 ; increment j+=3
14: inc ecx ; move to the next storage pos i+=1
15: cmp edx, edi ; compare j (edx) with len of str (edi)
16: jl short loc_1000AE4D ; if j < len(str) --> repeat loop

; Adding nul terminator if edi <= 0 --> str[0] = 0
17: loc_1000AE5B:
18: mov byte ptr [ecx+esi], 0

; Return the modified str
19: mov eax, esi
20: pop edi
21: retn ; return execution to the func caller
22: sub_1000AE3B endp ; 
```

C Code:

```c
char *sub_1000AE3B (char *str)
{
 int len, i=0, j=0;
 len = lstrlenA(str);
 if (len <= 0) {
 str[j] = 0;
 return str;
 }
 while (j < len) {
 str[i] = str[j];
 j = j+3;
 i = i+1;
 }
 str[i] = 0;
 return str;
}
```

**Example 5:**

```nasm
01: 8B CA mov ecx, edx ; move loop counter into ecx
02: loc_CFB8F: ; loop entry
	03: AD lodsd ; load double wrd (4 byte) from [esi] into eax and increment esi. Ie eax = *esi
04: F7 D0 not eax ; invert all bits in eax. ie. eax = ~eax
05: AB stosd ; store the val in eax at [edi], then increment edi
06: E2 FA loop loc_CFB8F ; loop cntrl 
```

```c
while (ecx != 0) {
 eax = *edi;
 edi++;
 *esi = ~eax;
 esi++;
 ecx--;
}
```

## System Mechanism - Address Translation

- Physical memory on a computer is divided into 4kb unit called *Pages*
- Memory addresses can be physical or virtual
- CPU instructions use virtual addresses and uses the memory management unit to translate those addresses into physical for accessing into memory. This is done with physical address extension (PAE) support
- A virtual memory address can be divided into indices into 3 tables and offset
    - Page Directory Pointer Table (PDPT) - array of 4 x 8 byte elements. Each pointing to a PD
    - Page Directory (PD) - array of 512 x 8 byte elements. Each pointing to a PT
    - Page Table (PT) - array of 512 x 8 byte elements w/ each containing a PTE
    - Page Table Entry (PTE) - each entry is a physical page of memory
- Eg. 1: We can interpret the virtual address 0xBF80EE6B as the following

![image.png](image.png)

- Each index points to a physical structure and the final page offset leads us to the final physical memory address
- The 8 byte elements contain data about tables, memory, permissions and other memory characteristics. There are specific bits that determine whether a page is read only, writable, executable/non executable, accessible by users or not, etc…
- CR3 register is used to hold the physical base address of the PDPT

**Example 2: Translation of 0xBF80EE6B virtual address on a real system:**

```nasm
kd> r @cr3 ; CR3 is the physical address for the base of a PDPT
cr3=085c01e0 ; this is the physical address
kd> !dq @cr3+2*8 L1 ; read the PDPT entry at index 2
                    ; 85c01f0 00000000`0d66e001
```

- Indexing into the page directory pointer table
    - Since CR3 = `085c01e0` (physical addy of PDPT) and *we think* the index is the 2nd byte (0, 1, 2, 3)
        - 085c01e0 + (2 * 8 bytes) = 0x85c01f0 → the contents of this memory space are `0x00000000 0d66e001`
    - Of `0d66e001` → if we clear the bottom 12 bits (001) we are left with `0d66e000` . This is the physical base address of the page directory
- Indexing into the Page Directory
    - Now that know the base addy of the page directory is `0x0d66e000`  → compute the index from the original virtual address `0xBF80EE6B`
    
    ```nasm
    kd> !dq 0d66e000+0x1fc*8 L1 ; read the PD entry at index 0x1FC 
    # d66efe0 00000000`0964b063
    ```
    
    - we are reading the PD entry at index 0x1FC → 0d66e000 + (0x1FC * 8) = `0x00000000 0964b063`
    - Given this address `0x0964b063` → the base address of the page table is `0x0964b000`
- Accessing the page table
    - Now using this page table base address `0x0964b000` we index into 0xE to get the page table entry
    
    ```nasm
    kd> !dq 0964b000+e*8 L1 ; read the PT entry at index 0xE
    # 964b070 00000000`06694021
    ```
    
    - `0x0964b000` + (e * 8) = `0x00000000 06694021`
    - Clearing the bottom 12 bits we get `06694000`. This is the base address of the physical page of memory
- Final Offset in the physical page
    - Add the lower 12 bits of the original virtual address `0xBF80EE6B` to the base address of the physical page of memory
    - 0xE6B + `0x06694000`
    - = `0x06694E6B` → This is the final physical address

Some benefits of using PAE (physical Address Enabled) Support Include …

- Process Isolation → Each process has its own CR3 value, meaning different PDPT
- Same virtual address can map to **different physical memory pages**

## Walkthrough Example

```nasm
01: ; BOOL __stdcall DllMain(HINSTANCE hinstDLL, DWORD fdwReason,
 ; LPVOID lpvReserved)
02: _DllMain@12 proc near
03: 55 push ebp ; save the old base pointer onto the stack
04: 8B EC mov ebp, esp ; update the base pointer with the current stack frame
05: 81 EC 30 01 00+ sub esp, 130h ; reserve 0x130 (304) bytes of mem on the stack for local vars
06: 57 push edi ; saves the value of the edi register onto the stack
07: 0F 01 4D F8 sidt fword ptr [ebp-8] ; current IDT register is written into mem/stack (2 byte limit, 4 byte base)
08: 8B 45 FA mov eax, [ebp-6] ; move base address of IDT into EAX 

; Performing a range check on IDT base address
09: 3D 00 F4 03 80 cmp eax, 8003F400h
10: 76 10 jbe short loc_10001C88 (line 18) ; jumb if below/equal <=
11: 3D 00 74 04 80 cmp eax, 80047400h
12: 73 09 jnb short loc_10001C88 (line 18) ; jump if not below >=

13: 33 C0 xor eax, eax ; zero out eax
14: 5F pop edi
15: 8B E5 mov esp, ebp ; discard all local vars at once by setting esp back to ebp
16: 5D pop ebp ; restores the old base ptr
17: C2 0C 00 retn 0Ch ; stdcall style return 

; If IDT in range, continue
18: loc_10001C88:
19: 33 C0 xor eax, eax
20: B9 49 00 00 00 mov ecx, 49h ; storing a loop counter in ecx -> (4*16 + 9) = 73 time loop counter
21: 8D BD D4 FE FF+ lea edi, [ebp-12Ch] ; load effective address into edi -> edi = &local_var[0]
                                        ; edi is set to point to the start of the buffer 300 bytes below EBP
                                        ; This is the destination buffer for store string cmd   
	22: C7 85 D0 FE FF+ mov dword ptr [ebp-130h], 0 ; Zero a DWORD at [ebp-130h] for init

; CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0)
23: 50 push eax ; dwProcessID param -> 0 means current process
24: 6A 02 push 2 ; TH32CS_SNAPPROCESS flag -> 0x2 gives a snapshot of all processes
25: F3 AB rep stosd ; Fill 73 dwords at EDI with EAX=0
26: E8 2D 2F 00 00 call CreateToolhelp32Snapshot ; call the snapshot function which will return the 
                                                 ; # of running windows processes                
27: 8B F8 mov edi, eax ; store this result back in edi
28: 83 FF FF cmp edi, 0FFFFFFFFh ; compare with invalid (0)
29: 75 09 jnz short loc_10001CB9 (line 35) ; jump if not 0 (ie. valid)

; Snapshot failed => return 0
30: 33 C0 xor eax, eax ; set return val to false (return 0)
31: 5F pop edi ;
32: 8B E5 mov esp, ebp ; reset esp back to base pointer to clear all local vars
33: 5D pop ebp ; pop base ptr and return
34: C2 0C 00 retn 0Ch

; Jump if valid sequence
; Snapshot succeeded => enumerating processes
35: loc_10001CB9: 
36: 8D 85 D0 FE FF+ lea eax, [ebp-130h] ; load the pointer (to the address of a struct) into eax
																				; this struct buffer is holding PROCESSENTRY32
37: 56 push esi 
38: 50 push eax ; pointer to PROCESSENTRY32 struct
39: 57 push edi ; snapshot handle
40: C7 85 D0 FE FF+ mov dword ptr [ebp-130h], 128h ; take the dword starting at -130h from ebp
                                                   ; set the first 4 bytes of each buffer (ie. dwSize) to 128h
41: E8 FF 2E 00 00 call Process32First ; call the process enumerating func w/ args (eax, edi and [ebp-130h])
42: 85 C0 test eax, eax ; test if func() succeeded.                        
43: 74 4F jz short loc_10001D24 (line 70) ; If not valid (ie. eax == 0) -> jump if zero flag is set


; Valid Scan -> Proceed
; We have at least one process => _stricmp loop
44: 8B 35 C0 50 00+ mov esi, ds:_stricmp ; load the function pointer addy of str insensitive cmp func into esi
                                         ; this func returns 0 if match is found
45: 8D 8D F4 FE FF+ lea ecx, [ebp-10Ch] ; Load the ptr to the szExeFile from the processSentry struct into ecx
                                        ; szExeFile contains the process name and we compare this str w/ a target
46: 68 50 7C 00 10 push 10007C50h ; pointer to a hard coded target str is pushed onto the stack
47: 51 push ecx ; pointer is pushed to the stack
48: FF D6 call esi ; _stricmp -> return 0 if match, non zero if not
49: 83 C4 08 add esp, 8 ; clean up stack due to the 2 args that were pushed
50: 85 C0 test eax, eax ; test result of stricmp if match (0)
51: 74 26 jz short loc_10001D16 (line 66) ; jump if 0 -> valid

; No match => get next process in a loop
52: loc_10001CF0: 
53: 8D 95 D0 FE FF+ lea edx, [ebp-130h] ; load ptr to the processentry struct again, so we can call the func again
54: 52 push edx ; push ptr and snapshot as func args for Process32Next
55: 57 push edi
56: E8 CD 2E 00 00 call Process32Next ; eax != 0 -> success
                                      ; eax == 0 -> end of list
57: 85 C0 test eax, eax ; check if done w/ the process list -> if 0 -> jump
58: 74 23 jz short loc_10001D24 (line 70)
59: 8D 85 F4 FE FF+ lea eax, [ebp-10Ch] ; again load in ptr to process name
60: 68 50 7C 00 10 push 10007C50h ; push ptr to target str
61: 50 push eax 
62: FF D6 call esi ; _stricmp
63: 83 C4 08 add esp, 8 ; clear stack after calling cmp func
64: 85 C0 test eax, eax ; test if we done with the process list
65: 75 DA jnz short loc_10001CF0 (line 52) ; jump back to the start of the loop if (jnz) not zero

; Found match => jump
66: loc_10001D16: ; we jump here if a match is found b/w process name and target str "ollydbg"
67: 8B 85 E8 FE FF+ mov eax, [ebp-118h] ; Load some local value into EAX and ECX
68: 8B 8D D8 FE FF+ mov ecx, [ebp-128h] 
69: EB 06 jmp short loc_10001D2A (line 73) ; unconditional jump for final return/exit logic

; If done enumerating or no match => set EAX = [ebp+0Ch], etc.
70: loc_10001D24: ; jump here if done with process list (when process32First returns 0)
71: 8B 45 0C mov eax, [ebp+0Ch] ; 3rd func param (lpvReserved) -> this val will be returned back to the caller
72: 8B 4D 0C mov ecx, [ebp+0Ch] ; complier quirk

; Common return logic
73: loc_10001D2A: ; unconditional jump for return logic 
74: 3B C1 cmp eax, ecx ; cmp eax and ecx -> jump if not 0 (no match)
75: 5E pop esi ; restore 
76: 75 09 jnz short loc_10001D38 (line 82) ; jump if not zero (alternate return handling)
77: 33 C0 xor eax, eax ; eax set to 0
78: 5F pop edi ; restore
79: 8B E5 mov esp, ebp ; reset stack by setting esp to ebp
80: 5D pop ebp ; pop ebp to return back to caller
81: C2 0C 00 retn 0Ch ; return

82: loc_10001D38:
83: 8B 45 0C mov eax, [ebp+0Ch] ; set the return val of eax to lpvReserve 
84: 48 dec eax ; decrement eax and then check its val -> jump if not 0
85: 75 15 jnz short loc_10001D53 (line 93) 
; pushing args to stack for CreateThreadFunc()
86: 6A 00 push 0 ; lpThreadAttributes
87: 6A 00 push 0 ; dwStackSize
88: 6A 00 push 0 ; lpParameter
89: 68 D0 32 00 10 push 100032D0h ; lpStartAddress
90: 6A 00 push 0 ; dwCreationFlags
91: 6A 00 push 0 ; lpThreadId 
92: FF 15 20 50 00+ call ds:CreateThread ; call thread creation func

93: loc_10001D53: ; if lpvReserve is != 0 -> we jump jere
94: B8 01 00 00 00 mov eax, 1 ; set eax to 1 (return true)??
95: 5F pop edi ; restore
96: 8B E5 mov esp, ebp ; reset esp back to base ptr and clear stack
97: 5D pop ebp restore ebp
98: C2 0C 00 retn 0Ch ; return
99: _DllMain@12 endp ; end dllMain function
```

Equivalent C Code:
```c
BOOL __stdcall DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
    DWORD idtBase = GetIDTBase();  // lines 7-8 are basically "sidt" and extracting IDT base

    // 1) Check if IDT base is in [0x8003F400, 0x80047400).
    if (idtBase < 0x8003F400 || idtBase >= 0x80047400) {
        return FALSE; // Suspicious environment (VM?), bail out
    }

    // 2) Create a snapshot of all processes
    HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnap == INVALID_HANDLE_VALUE) {
        return FALSE; // Snapshot failed
    }

    // 3) Prepare a PROCESSENTRY32 struct
    PROCESSENTRY32 pe32 = {0};
    pe32.dwSize = sizeof(PROCESSENTRY32);

    // 4) Get first process
    if (!Process32First(hSnap, &pe32)) {
        // no processes? Strange environment? 
        // Return whatever "lpvReserved" is (the code does some other manip).
        return (BOOL)lpvReserved; 
    }

    // 5) We'll do a loop, calling _stricmp(pe32.szExeFile, "someTarget") until match or end
    // Hardcoded "someTarget" at 0x10007C50, e.g. "ollydbg.exe" or "procmon.exe"

    do {
        if (_stricmp(pe32.szExeFile, (char*)0x10007C50) == 0) {
            // 6) If match found, load some local values from [ebp-118h], [ebp-128h], 
            //    then jump to final logic
            goto MatchFound;
        }
    } while (Process32Next(hSnap, &pe32));

    // If no match found, jump here -> lines 70-72
    // Return lpvReserved basically, or set up for final logic
    {
        // pseudo-simplified reading of line 70: EAX = [ebp+0Ch]
        // return (BOOL)lpvReserved, but eventually we land in shared exit block
    }

MatchFound:
    // lines 67..69 load local flags -> then line 73..81 do compare (eax vs ecx).
    // If they differ, goto loc_10001D38:
    // else return 0

    // loc_10001D38 checks if (lpvReserved == 1):
    // if so, create a new thread with start address 0x100032D0
    // after that, returns 1
    // else skip thread creation, also return 1

    if (lpvReserved == (LPVOID)1) {
        // CreateThread(...) with start address 0x100032D0
    }
    return TRUE;
}

``` 

## x64:
- this is a extension of x86 so only minor differences in regards to register sizes and the unavailablility of some instructions such as `PUSHAD`

### Register Set and Data Types:
- The register set has 18 64-bit general purpose registers

```txt
Register Set and Data Types

RAX Register (General-Purpose Register)
+-------------------------------+ 63
|            RAX                |
+---------------+               |
|     EAX       |               | 31
+-------+-------+               |
|  AX   |                       | 15
+---+---+                       |
|AH |AL |                       |  7
+---+---+-----------------------|  0

RBP Register (Base Pointer Register)
+-------------------------------+ 63
|            RBP                |
+---------------+               |
|     EBP       |               | 31
+-------+-------+               |
|  BP   |                       | 15
+---+---+                       |
|   PL  |                       |  7
+---+---+-----------------------|  0
```

- While RBP can be used as a base pointer -> most x64 compilers just use it as a regular GPR and reference local vars relative to RSP

### Data Movement:
- x64 uses a concept called RIP - *relative addressing* 
- This allows instructions to reference data at a relative position to RIP. 
- Eg 1.
```nasm
01: 0000000000000000 48 8B 05 00 00+ mov rax, qword ptr cs:loc_A ; move the 64-bit value stored at the address `RIP + displacement` into RAX
02: ; originally written as "mov rax,
[rip]"
03: 0000000000000007 loc_A:
04: 0000000000000007 48 31 C0 xor rax, rax ; zero out the RAX register
05: 000000000000000A 90 nop
```
- Line 1 reads the address of loc_A which is 0x7 and saves it in RAX
- RIP facilitates position independant code

- Most arithmetic instructions are automatically promoted to 64 bits even though the operands are only 32 bits
- Eg 2.
```nasm
48 B8 88 77 66+ mov rax, 1122334455667788h ; move this operand into RAX
31 C0 xor eax, eax ; will also clear the upper 32bits of RAX.
 ; i.e., RAX=0 after this
48 C7 C0 FF FF+ mov rax,0FFFFFFFFFFFFFFFFh ; set RAX to max unsigned value ie. -1
FF C0 inc eax ; RAX=0 after this
```
- This behaviour is not symmetric -> when we write to RAX, this does not effect EAX, but writing to EAX does 0 out the upper 32 bits of RAX

### Canonical Address:
- On x64 virtual addresses are 64 bits in width, buth most processeors do not support a full 64 bit virtual address space, but rather 48 bits of address space
- A virtual address must be in canonical form and this is true if bits 63 to the most significant bit are either all 1s or 0s
- In practical terms this means bits 48-63 need to match bit 47
- Eg 3.
```nasm
0xfffff801`c9c11000 = 11111111 11111111 11111000 00000001 11001001 11000001
 00010000 00000000 ; canonical
0x000007f7`bdb67000 = 00000000 00000000 00000111 11110111 10111101 10110110
 01110000 00000000 ; canonical
0xffff0800`00000000 = 11111111 11111111 00001000 00000000 00000000 00000000
 00000000 00000000 ; non-canonical
0xffff8000`00000000 = 11111111 11111111 10000000 00000000 00000000 00000000
 00000000 00000000 ; canonical
0xfffff960`000989f0 = 11111111 11111111 11111001 01100000 00000000 00001001
 10001001 11110000 ; canonical
```
- If code tries to dereference a non-canonical address, an exception will be thrown
- Why use this the canonical rule form
    - efficent sign extension for pointers
    - prevents accidental memory access into non-existent space
    - Makes address space seperation b/w user space and kernel space simplier (based on MSBs being 0 or 1)

### Function Invocation:
- Some calling conventions require params to be passed on the stack on x86
- On x64 there is only one calling convention and the first 4 params are passed through `RCX`, `RDX`, `R8` and `R9`. The remaining are pushed onto the stack from right to left

# Chapter 2 – Arm
## Basic Features
- ARM is a RISC (reduced instruction set) architecture type, meanwhile x86 is CISC (complex instruction set)
- some new versions of intel processor offer some RISC features as well
- The arm instruction set is much smaller then x86 and it offers more GPRs
- Instruction length is fixed width (16/32 bits)
- ARM offers load store model for memory access
- data must be moved into registers before operations and only load/store can access it --> `LDR` or `STR`
- Eg. To increment a 32 bit value at a given memory address -> first load the value at that address into a register, perform the increment operation and then store it back. 3 instructions just to perform a addition operation, which only takes 1 total instruction on x86
- Privilege Isolation in ARM
    - In x86 privilege was defined w/ 4 rings, with 0 being the highest priv
    - In arm there are instead eight different modes
        - User (Usr)
        - Fast Interrupt Req (FIQ)
        - Interupt Req (IRQ)
        - Supervisor (SVC)
        - Monitor (MON)
        - Abort (ABT)
        - Undefined (UND)
        - System (SYS)
    - Each mode offers certain privileges and register access
    - SVC is ring 0 and USR is ring 3
- ARM processors operate in 2 states: `ARM` or `Thumb` and this determines instruction set to use
    - ARM -> 32 bit instructions
    - Thumb -> 16 or 32 bit
    - Which state the processor executes in depends on 2 conditions
        - When branching with the `BX` or `BLX` instruction -> if the destination register's LSB is 1 then -> switch to Thumb state
        - If the T bit in the current program status register (CPSR) is set -> then its in thumb mode
    - Recently: booting into Thumb state is more preferable for the 16/32-bit instruction flexibility
    - Thumb 32-bit instructions have a *.w suffix*

- Conditional execution
    - ARM also supports conditional execution. An instruction can encode certain arithmetic conditions that must be met in order for it to be executed. Eg. an instruction can specifiy that it will only execute if the result of the previous instruction is 0. Contrasting this w/ x86 -> most instructions execute unconditionally
    - This is useful since it cuts down on branching instructions (which are expensive)

- ARM Barrel Shifter
    - certain instructions can "contain" another arithmetic instruction that shifts or rotates a register
    - useful for shrinkingm multiple instructions into 1
    - eg. wanting to multiply a register by 2 and store the result in another register. Normally this would require 2 instructions (multiply and move) -> but in a barrel instruction you can perform the left shift by 1 within the mov instruction: `MOV R1, R0, LSL #1 ; R1 = R0 * 2`

### Data Types and Registers
- Supported ARM data types include 8-bit, 16 bit (half wrd), 32 bit (wrd) and 64 bit (double wrd)
- ARM defines 16 32-bit general purpose registers from `R0` to `R15`
    - the first 12 are for gpr usage and the last 3 do special stuff
    - `R13` is the stack pointer and equivalent of ESP/RSP in x86
    - `R14` is the link register (LR) and it holds the return address during a function call. 
        - Instructions may implicity use this register such as `BL` which would store the return address in LR before branching
    - `R15` is the program counter (PC). When executing in ARM state, PC is the address of the current instruction + 8 (2 arm instructions ahead). In thumb state this would be address of the current instruction + 4 (2 16 bit thumb instructions ahead). This is similar to `EIP`/`RIP` from x86, execept we are always pointing to the address of the next instruction to execute
        - The reason for this as follows. Arm uses a pipelining behaviour for fetching instructions. ARM uses a 3 stage pipeline (fetch, decode and execute). While executing the current instruction -> fetch the next 2 instructions. So while reading R15, we dont grab the current address but rather the address 2 instructions ahead
        - Code can directly read from and write to the PC register. 
            - Reading PC will give you the address of the instruction at a fixed offset ahead of the one currently being executed. ARM state offset is +8 bytes and thumb state offset is +4 bytes
            - Writing PC (eg `MOV` to PC or `POP {pc}`) immediately alters control flow and execution jumps to the new PC value
    - Consider the snippet below in thumb state:
        ```ARM
        1: 0x00008344 push {lr} ; push link reg onto stack (which holds return addy)
        2: 0x00008346 mov r0, pc ; r0 ← PC = 0x8346 + 4 = 0x834A
        3: 0x00008348 mov.w r2, r1, lsl #31 ; 32 bit thumb instruction (.w). performs logical left shift on r1 reg by 31 bits
                                            ; result stored in r2
        4: 0x0000834c pop {pc} ; pop val from stack into pc reg -> since the last thing pushed was the link reg, we are branching execution immediately to whatever val is stored in `LR`. This returns control back to caller
        ```
        GDB Demo
        ```ARM
        (gdb) br main
        Breakpoint 1 at 0x8348
        ...
        Breakpoint 1, 0x00008348 in main () ; GDB breaks before executing this. 
        (gdb) disas main
        Dump of assembler code for function main:
        0x00008344 <+0>: push {lr}
        0x00008346 <+2>: mov r0, pc
        => 0x00008348 <+4>: mov.w r2, r1, lsl #31 ; breakpoint hits before execution, at this point we show the `PC` and `R0` reg
        0x0000834c <+8>: pop {pc}
        0x0000834e <+10>: lsls r0, r0, #0
        End of assembler dump.
        (gdb) info register pc
        pc 0x8348 0x8348 <main+4>
        (gdb) info register r0
        r0 0x834a 33610
        ```
        - When the breakpoint at 0x8348 hits...
            - PC points to the 3rd instruction at 0x8348 (about to be executed) 
            - R0 shows the previously read PC value
- Similar to other architectures ARM stores info about the current execution state in the *current program status register* (CPSR). From the programmer perspective CPSR is similar to EFLAG/RFLAG reg in x86
- There are many flags within the CPSR. Heres a couple detailed below
    - E (Endianness bit) - ARM can operate in either big or little endian. Set this bit to 0 for lil or 1 for big endian. Mostly lil E is used though
    - T (Thumb bit) - if this is set to 1 -> you are in thumb state. Else you are in ARM state
    - M (Mode bits) - These bits specify the current privilege mode (USR, SVC,)
    ```txt
           31                 26                                   15      10 9        5 4      0    
            +-----------------------------------------------------------------------------------+
    CPSR    | cond. flag       |                                   |   IT   |E|        |T|  M   |
            +-----------------------------------------------------------------------------------+
    ```

### System Level Controls and Settings
- ARM offers the *concept* of coprocessors to support additional instructions and system level settings
- Eg. if a system supports a memory management unit then its settings must be exposed to boot or kernel code. On x86/x64, these settings are stored in `CR0` and `CR4`. On ARM it would be stored in a coprocessor (`CPO` - `CP15`)
- The first 13 coprocessors are reserved by ARM and and other 2 can be used by manufacturers to custom instructions 
- Each coprocessor contains additional "opcodes" and registers that can be controlled via special ARM instructions
- CP15 is known as the system control coprocessor and stores the system settings such as caching, paging, exceptions, etc...
- Each coprocessor has 16 registers and 8 opcodes. Semantics of these registers and opcodes is specific to the coprocessor
- MRC (read) and MCR (write) instructions
- it takes a coprocessor #, register # and opcodes
- Eg. to read the translation base register (similar to CR3 in x86) and save it in R0, you would do the following:
    ```ARM
    MRC p15, 0, r0, c2, c0, 0 ; save TTBR in r0
    ; Read coprocessor 15's C2/C0 reg using opcode 0/0 and store the result in gpr R0
    ```

## Introduction to ARM instruction set
- ARM has some quirks that arent in x86 such as instructions operating on a range of registers in sequence
- Eg. to store 5 regs R6-R10 at a particular memory location referenced by R1, you would write `STM R1, {R6-R10}`. This would result in R6 being stored at memory address R1, R7 at R1 + 4, R8 at R1 + 8 and so on
-  