# ARMv7-Emulator

## Description
An ARMv7 Emulator that runs specific assembly files.ARMv7 is a RISC assembly language used in some older ARM 32-bit processors. 
This emulator uses the official ARMv7 documentation to create the different opcodes, condition codes, and instruciton formats.
The emulator supports three different instruction types: 

1) Data Processing Instructions - Responsible for handling arithmetic and logical operations on registers. The format for this type of instruction looks like the following:


2) Memory Processing Instructions (Specifically Single Data Transfer, ldr & str) - This type of instruction loads and stores data
from memory. The format for memory processing instrucitons is completely different from the data processing instructions:


3) Branch and exchange instructions - Branch instructions allow the program to move to different lines and also return from
"function" (bxlr). Like the other two instruction types, branches have their own format for instructions. 

