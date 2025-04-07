#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_SYMBOLS 100
#define MAX_LITERALS 100
#define MAX_LENGTH 20

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

// Function to get symbol address
int getSymbolAddress(int index) {
    for (int i = 0; i < symbolCount; i++) {
        if (symbolTable[i].index == index) {
            return symbolTable[i].address;
        }
    }
    return -1;
}

// Function to get literal address
int getLiteralAddress(int index) {
    for (int i = 0; i < literalCount; i++) {
        if (literalTable[i].index == index) {
            return literalTable[i].address;
        }
    }
    return -1;
}

// Function to get constant value from intermediate code line
int getConstantFromLine(const char *line) {
    int constant;
    sscanf(line, "%*d (%*[^,],%*d) (%*d) (%d)", &constant);
    return constant;
}

// Read Symbol and Literal Tables from Intermediate Code
void readTables() {
    FILE *file = fopen("INTERMEDIATE.TXT", "r");
    if (!file) {
        perror("Error opening intermediate file");
        exit(EXIT_FAILURE);
    }

    char line[100];
    while (fgets(line, sizeof(line), file)) {
        if (strstr(line, "Symbol Table:")) {
            while (fgets(line, sizeof(line), file) && line[0] != '\n') {
                int index, address;
                char symbol[MAX_LENGTH];
                sscanf(line, "%d\t%s\t%d", &index, symbol, &address);
                symbolTable[symbolCount].index = index;
                strcpy(symbolTable[symbolCount].symbol, symbol);
                symbolTable[symbolCount].address = address;
                symbolCount++;
            }
        }
        if (strstr(line, "Literal Table:")) {
            while (fgets(line, sizeof(line), file) && line[0] != '\n') {
                int index, address;
                char literal[MAX_LENGTH];
                sscanf(line, "%d\t%s\t%d", &index, literal, &address);
                literalTable[literalCount].index = index;
                strcpy(literalTable[literalCount].literal, literal);
                literalTable[literalCount].address = address;
                literalCount++;
            }
        }
    }

    fclose(file);
}

// Generate Target Code from Intermediate Code
void generateTargetCode() {
    FILE *file = fopen("INTERMEDIATE.TXT", "r");
    if (!file) {
        perror("Error opening intermediate file");
        exit(EXIT_FAILURE);
    }

    char line[100];
    printf("\nTarget Code:\n");

    while (fgets(line, sizeof(line), file)) {
        int LC, opcode, regCode, index;
        char type[3], symbolType;
        int address = 0;

        // ✅ Handle direct -- -- -- -- lines
        if (strstr(line, "-- -- -- --")) {
            printf("-- -- -- --\n");
            continue;
        }

         // ✅ Handle (AD,2) - Display -- -- -- --
         if (sscanf(line, "%d (%[^,],%d)", &LC, type, &opcode) == 3 && strcmp(type, "AD") == 0 && opcode == 2) {
            printf("-- -- -- --\n");
            continue;
        }

        // ✅ Handle (DL,1) - Display -- -- -- --
        if (sscanf(line, "%d (%[^,],%d)", &LC, type, &opcode) == 3 && strcmp(type, "DL") == 0) {
            if (opcode == 1) {
                printf("-- -- -- --\n");
                continue;
            }
        }

        // ✅ Handle (DL,2) - Directly use constant value
        if (sscanf(line, "%d (%[^,],%d) (%d) (%d)", &LC, type, &opcode, &regCode, &index) == 5 && strcmp(type, "DL") == 0) {
            if (opcode == 2) {
                address = getConstantFromLine(line);
                printf("%d -- -- (%03d)\n", LC, address);
                continue;
            }
        }

        // ✅ First line handling for (AD,1) (C,100)
        if (sscanf(line, "%d (%[^,],%d) (%c,%d)", &LC, type, &opcode, &symbolType, &index) == 5) {
            if (symbolType == 'C') {
                address = index;
            }
            printf("%d (%02d) (%02d) (%03d)\n", LC, 0, 0, address);
            continue;
        }

        // ✅ General instruction handling
        if (sscanf(line, "%d (%[^,],%d) (%d) (%c,%d)", &LC, type, &opcode, &regCode, &symbolType, &index) == 6) {
            if (symbolType == 'S') {
                address = getSymbolAddress(index);
            } else if (symbolType == 'L') {
                address = getLiteralAddress(index);
            }

            printf("%d (%02d) (%02d) (%03d)\n", LC, opcode, regCode, address != -1 ? address : index);
            continue;
        }

        // ✅ Special case for STOP instruction `(IS,0)`
        if (sscanf(line, "%d (%[^,],%d)", &LC, type, &opcode) == 3) {
            if (opcode == 0) {
                printf("%d (%02d) (%02d) (%03d)\n", LC, opcode, 0, 0);
            }
        }
    }

    fclose(file);
}

int main() {
    readTables();
    generateTargetCode();
    return 0;
}
