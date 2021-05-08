#ifndef X2017_H
#define X2017_H

#define BYTE unsigned char

#include "opcode.h"

typedef struct {
    BYTE *RAM; // 2^8 address of 1 byte each

    BYTE *registers; // 8 registers of 1 byte each
    // REG 7 is the program counter
    // REG 6 total length
    // REG 5 EBP
    // REG 4 ESP
    // REG 0,1,2,3 reserved

    int **raw_array; 

}VM;


#endif



// FUNC LABEL 0
//     MOV REG 2 VAL 16
//     ADD REG 0 REG 2
//     PRINT REG 0
//     MOV REG 1 REG 0    <--- copy REG 0 to REG 1 to avoid modifying REG 0
//     EQU REG 1          <--- makes REG 1 hold 1 if REG 0 was 0
//     ADD REG 7 REG 1    <--- bypasses CAL if REG 1 is 1
//     CAL VAL 0
//     RET


// file
// 0 2 0 1 0 
// 0 1 0 3 0 
// 0 1 1 0 2 
// 4 1 0 1 1 
// 0 3 0 1 0 
// 2 
// FUNC LABEL 7
//     MOV STK A REG 0
//     MOV REG 0 PTR A
//     MOV REG 1 VAL 2
//     ADD REG 0 REG 1
//     MOV PTR A REG 0
//     RET
// test1
// FUNC LABEL 0
//     MOV REG 0 VAL 3
//     MOV REG 1 VAL 5
//     ADD REG 0 REG 1
//     MOV STK A REG 0
//     RET



// FUNC LABEL 4    
//     MOV STK A VAL 42      A now stores 42     
//     REF STK B STK A       B stores address of A
//     REF PTR B PTR B       *B = &*B   A now stores content of B which is &A
//     REF STK C STK B       C now stores the address of B
//     MOV REG 0 STK A       reg 0 now stores &A
//     MOV REG 1 STK C       reg 1 now stores &B
//     CAL VAL 7        
//     RET
// FUNC LABEL 7
//     MOV REG 2 VAL 1       
//     NOT REG 1
//     ADD REG 1 REG 2        reg 1 now stores -&B
//     ADD REG 0 REG 1        reg 0 now stores &A - &B
//     EQU REG 0              reg 0 evaluates to 0
//     PRINT REG 0            Print 0   
//     RET


// label: 5
// 0 2 0 1 0 
// 0 2 1 3 0 
// 0 1 0 3 1 
// 0 1 1 0 255 
// 4 1 0 1 1 
// 0 3 1 1 0 
// 2 
// label: 7
// 0 2 0 1 0 
// 3 2 1 2 0 
// 0 1 0 2 1 
// 1 0 5 
// 2 
// label: 0
// 0 2 0 0 18 
// 0 2 1 0 42 
// 3 1 0 2 0 
// 1 0 7 
// 3 1 0 2 1 
// 1 0 7 
// 5 2 0 
// 5 2 1 
// 2 