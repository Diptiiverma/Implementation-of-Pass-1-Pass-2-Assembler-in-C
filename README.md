# Implementation-of-Pass-1-Pass-2-Assembler-in-C

This project demonstrates the implementation of a two-pass assembler using the C programming language. The assembler processes a simple assembly language with directives like START, END, LTORG, ORIGIN, EQU, and instructions such as ADD, SUB, MOVER, etc.


🔁 Overview
The assembler works in two passes:

🔹 Pass 1
1.Parses the assembly source code (ASSEMBLY.TXT)
2.Generates intermediate code
3.Builds the Symbol Table and Literal Table
4.Handles:
  Directives: START, END, LTORG, ORIGIN, EQU
  Declarative statements: DS, DC
  Imperative statements with registers and operands
5.Assigns memory addresses to literals and symbols
6.Outputs intermediate representation and symbol/literal tables

🔹 Pass 2
1.Reads the intermediate code, symbol table, and literal table
2.Resolves symbol and literal addresses
3.Produces the final target machine code
4.Handles constants, register references, and memory references appropriately

🗂️ Files
1.pass1.c – Implements Pass 1 (intermediate code + tables generation)
2.pass2.c – Implements Pass 2 (target code generation)
3.ASSEMBLY.TXT – Sample input file containing the assembly program
4.INTERMEDIATE.TXT – Output of Pass 1, used as input for Pass 2

📌 Features
1.Handles pseudo-ops like ORIGIN, EQU, and LTORG
2.Manages label resolution and forward referencing
3.Maintains clean symbol and literal tables
4.Intermediate code helps in separating address assignment and code generation
5.Clear, structured output for both development and educational purposes

📚 Educational Use
This project is ideal for students learning about:
1.Compiler design
2.Assemblers and loaders
3.Symbol and literal management
4.Two-pass processing strategies
