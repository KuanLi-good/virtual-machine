#ifndef OPCODE_H
#define OPCODE_H

// RAW_BIANRY stores the byte array retrived from hex file
// it contains the cursor represented by bit and byte value
typedef struct {

    int **op_binary;
    
    int byte;
    int bit;

}RAW_BINARY;

// ASC_BINARY stores the decoded binary code in ascending order
// it also contains the stk symbol correspondence
typedef struct {
    int **ops_asc;
    size_t *line_cnt;
    
    int stk_cnt;
    int *stk_ptr;

    int label;
    int num_of_ins;

}ASC_BINARY;

typedef enum {
    MOV = 0, // MOV A B
    CAL, // CAL A
    RET, //
    REF, // REF A B
    ADD, // ADD A B
    PRINT, // PRINT A
    NOT, // NOT A
    EQU // EQU A    
}OPCODE;

typedef enum {
    VAL = 0, // 1 byte
    REG, // 3 bits
    STK, // 5 bits
    PTR  // 5 bits
}TYPE;

#endif