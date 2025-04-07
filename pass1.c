#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_TOKENS 10
#define MAX_LENGTH 20
#define MAX_SYMBOLS 100
#define MAX_LITERALS 100

// Symbol Table Structure
typedef struct {
    int index;
    char symbol[MAX_LENGTH];
    int address;
} SymbolTableEntry;

SymbolTableEntry symbolTable[MAX_SYMBOLS];
int symbolCount = 0;

// Literal Table Structure
typedef struct {
    int index;
    char literal[MAX_LENGTH];
    int address;
} LiteralTableEntry;

LiteralTableEntry literalTable[MAX_LITERALS];
int literalCount = 0;

// Opcode Table Structure
typedef struct {
    char mnemonic[MAX_LENGTH];
    char type[3];  // AD, IS, DL
    int code;
} OpcodeTableEntry;

// Opcode Table
OpcodeTableEntry opcodeTable[] = {
    {"START", "AD", 1}, {"END", "AD", 2}, {"LTORG" ,"AD" ,3},{"ORIGIN","AD",4},{"EQU", "AD", 5},
    {"DS", "DL", 1}, {"DC", "DL", 2},
    {"STOP", "IS", 0},{"ADD", "IS", 1},{"SUB", "IS", 2}, {"MULT", "IS", 3},
    {"MOVER", "IS", 4}, {"MOVEM", "IS", 5},{"COMP", "IS", 6},{"BC", "IS", 7},
    {"DIV", "IS", 8},{"READ", "IS", 9},{"PRINT", "IS", 10}     
};
int opcodeCount = sizeof(opcodeTable) / sizeof(opcodeTable[0]);

// Register Table Structure
typedef struct {
    char regName[5];
    char regCode[3];
} Register;

Register registerTable[] = {
    {"AREG", "01"}, {"BREG", "02"}, {"CREG", "03"}, {"DREG", "04"}
};
int registerCount = sizeof(registerTable) / sizeof(registerTable[0]);

int LC = 0; // Location Counter

// Function to get opcode
OpcodeTableEntry* getOpcode(char* mnemonic) {
    for (int i = 0; i < opcodeCount; i++) {
        if (strcmp(opcodeTable[i].mnemonic, mnemonic) == 0) {
            return &opcodeTable[i];
        }
    }
    return NULL;
}

// Function to get register code
char* getRegisterCode(char *regName) {
    char cleanedRegName[MAX_LENGTH];
    strcpy(cleanedRegName, regName);

    // Remove unwanted characters (e.g., comma)
    for (int i = 0; cleanedRegName[i] != '\0'; i++) {
        if (cleanedRegName[i] == ',' || isspace(cleanedRegName[i])) {
            cleanedRegName[i] = '\0';
            break;
        }
    }

    for (int i = 0; i < registerCount; i++) {
        if (strcmp(registerTable[i].regName, cleanedRegName) == 0) {
            return registerTable[i].regCode;
        }
    }
    return "00"; // Default if not found
}

// Function to search for symbol
int getSymbolIndex(char* symbol) {
    for (int i = 0; i < symbolCount; i++) {
        if (strcmp(symbolTable[i].symbol, symbol) == 0) {
            return i;
        }
    }
    return -1;
}

void insertSymbol(char* symbol, int address) {
    char cleanedSymbol[MAX_LENGTH];
    strcpy(cleanedSymbol, symbol);

    // Remove trailing colon (if present)
    int len = strlen(cleanedSymbol);
    if (len > 0 && cleanedSymbol[len - 1] == ':') {
        cleanedSymbol[len - 1] = '\0';
    }

    int index = getSymbolIndex(cleanedSymbol);
    if (index == -1) {
        symbolTable[symbolCount].index = symbolCount + 1;
        strcpy(symbolTable[symbolCount].symbol, cleanedSymbol);
        symbolTable[symbolCount].address = address;
        symbolCount++;
    } else if (address != -1 && symbolTable[index].address == -1) {
        symbolTable[index].address = address; // ✅ Update address if available
    }
}

// Function to search for literal
int getLiteralIndex(char* literal) {
    for (int i = 0; i < literalCount; i++) {
        if (strcmp(literalTable[i].literal, literal) == 0) {
            return i;
        }
    }
    return -1;
}

// Insert Literal
void insertLiteral(char* literal, int address) {
    int index = getLiteralIndex(literal);
    if (index == -1) {
        literalTable[literalCount].index = literalCount + 1;
        strcpy(literalTable[literalCount].literal, literal);
        literalTable[literalCount].address = address;
        literalCount++;
    }
}

// Display Symbol Table
void displaySymbolTable() {
    printf("\nSymbol Table:\n");
    printf("Index\tSymbol\tAddress\n");
    for (int i = 0; i < symbolCount; i++) {
        printf("%d\t%s\t%d\n", symbolTable[i].index, symbolTable[i].symbol, symbolTable[i].address);
    }
}

// Display Literal Table
void displayLiteralTable() {
    printf("\nLiteral Table:\n");
    printf("Index\tLiteral\tAddress\n");
    for (int i = 0; i < literalCount; i++) {
        printf("%d\t%s\t%d\n", literalTable[i].index, literalTable[i].literal, literalTable[i].address);
    }
}

// Function to handle ORIGIN instruction
void handleOrigin(char* operand) {
    char symbol[MAX_LENGTH];
    int offset = 0;

    if (strchr(operand, '+') || strchr(operand, '-')) {
        sscanf(operand, "%[^+-]%d", symbol, &offset);
        int index = getSymbolIndex(symbol);
        if (index != -1) {
            LC = symbolTable[index].address + offset;
        } else {
            printf("Error: Undefined symbol %s\n", symbol);
            LC = -1;
        }
    } else {
        LC = atoi(operand);
    }
    printf("--      -- -- -- --\n");
}

// ✅ Handle LTORG instruction separately
void processLTORG() {
    for (int i = 0; i < literalCount; i++) {
        if (literalTable[i].address == -1) {
            literalTable[i].address = LC;
            printf("%d\t(AD,3) (00) (%s)\n", LC, literalTable[i].literal);
            LC++;
        }
    }
}

// ✅ Handle END instruction separately
void processEND() {
    int pendingLiterals = 0;
    
    // Count pending literals
    for (int i = 0; i < literalCount; i++) {
        if (literalTable[i].address == -1) {
            pendingLiterals++;
        }
    }

    // Process pending literals (if any)
    if (pendingLiterals > 0) {
        for (int i = 0; i < literalCount; i++) {
            if (literalTable[i].address == -1) {
                literalTable[i].address = LC;
                printf("%d\t(AD,2) (00) (%s)\n", LC, literalTable[i].literal);
                LC++;
            }
        }
    }

    // ✅ Output (AD,2) even if no literals are defined
    printf("%d\t(AD,2)\n", LC);
}


// Process Assembly File
void processAssemblyFile(char* filename) {
    FILE* inputFile = fopen(filename, "r");
    if (!inputFile) {
        perror("Error opening file");
        return;
    }

    char line[100];
    printf("\nIntermediate Code:\n");
    printf("LC\tCode\n");

    while (fgets(line, sizeof(line), inputFile)) {
        char tokens[MAX_TOKENS][MAX_LENGTH];
        int tokenCount = 0;
        char* token = strtok(line, " \t\n");

        while (token) {
            strcpy(tokens[tokenCount++], token);
            token = strtok(NULL, " \t\n");
        }

        if (tokenCount == 0) continue;

        if (strcmp(tokens[0], "LTORG") == 0) {
            processLTORG(); // ✅ Handle LTORG
            continue;
        }

        if (strcmp(tokens[0], "END") == 0) {
            processEND(); // ✅ Handle END
            break;
        }
        int tokenIndex = 0;

// ✅ Handle ORIGIN Instruction in processAssemblyFile
if (strcmp(tokens[tokenIndex], "ORIGIN") == 0) {
    if (tokenIndex + 1 < tokenCount) {
        handleOrigin(tokens[tokenIndex + 1]); // Call handleOrigin with the operand
    } else {
        printf("Error: ORIGIN instruction requires an operand.\n");
    }
    continue; // ✅ Skip further processing for this line
}



// ✅ Handle EQU Instruction with Arithmetic Expression
if (tokenCount > 2 && strcmp(tokens[tokenIndex + 1], "EQU") == 0) {
    int value = 0;
    char operandCopy[20];
    strcpy(operandCopy, tokens[tokenIndex + 2]);

    char operator;
    int offset = 0;

    // ✅ Check if operand contains an operator (+ or -)
    if (strchr(operandCopy, '+') || strchr(operandCopy, '-')) {
        char baseSymbol[20];
        sscanf(operandCopy, "%[^+-]%c%d", baseSymbol, &operator, &offset);

        // ✅ Get address of the referenced symbol
        int refIndex = getSymbolIndex(baseSymbol);
        if (refIndex != -1) {
            value = symbolTable[refIndex].address;
            if (operator == '+') {
                value += offset;
            } else if (operator == '-') {
                value -= offset;
            }
        } else {
            printf("Error: Undefined symbol '%s' in EQU.\n", baseSymbol);
            continue;
        }
    } else {
        // ✅ Direct symbol or constant value
        int refIndex = getSymbolIndex(operandCopy);
        if (refIndex != -1) {
            value = symbolTable[refIndex].address;
        } else if (isdigit(operandCopy[0]) || operandCopy[0] == '-') {
            value = atoi(operandCopy);
        } else {
            printf("Error: Undefined symbol '%s' in EQU.\n", operandCopy);
            continue;
        }
    }

    // ✅ Insert or update symbol value
    int symIndex = getSymbolIndex(tokens[tokenIndex]);
    if (symIndex == -1) {
        insertSymbol(tokens[tokenIndex], value);
    } else {
        symbolTable[symIndex].address = value;
    }

    // ✅ Display -- -- -- -- in intermediate code
    printf("--\t-- -- -- --\n");

   

    continue; // ✅ Skip LC increment for EQU
}



        // ✅ Handle START
        if (tokenCount > 1 && strcmp(tokens[0], "START") == 0) {
            LC = atoi(tokens[1]);
            printf("00\t(AD,1) (C,%d)\n", LC);
            continue;
        }

        // ✅ If first token is a label
        OpcodeTableEntry* opcode = getOpcode(tokens[tokenIndex]);
        if (!opcode && tokenCount > 1) {
            OpcodeTableEntry* nextOpcode = getOpcode(tokens[tokenIndex + 1]);
            if (nextOpcode) {
                int symIndex = getSymbolIndex(tokens[tokenIndex]);
                if (symIndex == -1) {
                    insertSymbol(tokens[tokenIndex], LC);
                } else if (symbolTable[symIndex].address == -1) {
                    symbolTable[symIndex].address = LC; // ✅ Update address if it's undefined
                }
                tokenIndex++;
            }
        }
        
        opcode = getOpcode(tokens[tokenIndex]);
        if (opcode) {
            printf("%d\t(%s,%d)", LC, opcode->type, opcode->code);
            tokenIndex++;

            if (strcmp(opcode->mnemonic, "DS") == 0) {
                // ✅ Handle DS (Declare Storage)
                int value = atoi(tokens[tokenIndex]);
                printf(" (00) (%d)", value);
                LC += value;
            } 
            else if (strcmp(opcode->mnemonic, "DC") == 0) {
                // ✅ Handle DC (Declare Constant)
                int value = atoi(tokens[tokenIndex]);
                printf(" (00) (%d)", value);
                LC++;
            } 
            else if (strcmp(opcode->mnemonic, "READ") == 0 || strcmp(opcode->mnemonic, "PRINT") == 0) {
                if (tokenIndex < tokenCount) {
                    // ✅ Handle symbol reference for READ and PRINT
                    int symIndex = getSymbolIndex(tokens[tokenIndex]);
                    if (symIndex == -1) {
                        insertSymbol(tokens[tokenIndex], -1);
                        symIndex = symbolCount - 1;
                    }
                    printf(" (%s) (S,%d)", "00", symIndex + 1);
                } else {
                    printf(" (00) (00)"); // ✅ Handle cases with missing operand
                }
                LC++; // ✅ Increment LC after handling the instruction
            }
            
            else {
                if (tokenIndex < tokenCount) {
                    // ✅ Handle Register Code
                    char* regCode = getRegisterCode(tokens[tokenIndex]);
                    printf(" (%s)", regCode);
                    tokenIndex++;

                    if (tokenIndex < tokenCount) {
                        if (tokens[tokenIndex][0] == '=') {
                            // ✅ Handle Literal
                            int literalIndex = getLiteralIndex(tokens[tokenIndex]);
                            if (literalIndex == -1) {
                                insertLiteral(tokens[tokenIndex], LC);
                                literalIndex = literalCount - 1;
                            }
                            printf(" (L,%d)", literalIndex + 1);
                        } else {
                            // ✅ Handle Symbol Reference
                            int symIndex = getSymbolIndex(tokens[tokenIndex]);
                            if (symIndex == -1) {
                                insertSymbol(tokens[tokenIndex], -1);
                                symIndex = symbolCount - 1;
                            }
                            printf(" (S,%d)", symIndex + 1);
                        }
                    }
                }
                LC++;
            }
            printf("\n");
        }

       
        // ✅ Process other instructions here
        // ...
    }

    fclose(inputFile);
    displaySymbolTable();
    displayLiteralTable();
}

// Main Function
int main() {
    processAssemblyFile("ASSEMBLY.TXT");
    return 0;
}
