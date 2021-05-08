#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "x2017.h"
#include "x2017_decoder.c"

VM* vm_setup(ASC_BINARY **ab_array, int num_of_fns);
void print_type(VM *vm, int type, int value);
int read_address(VM *vm, int type, int value);
int read_value(VM *vm, int type, int value);
void assign_value(VM *vm, int type, int index, int value);
int check_stk_exist(VM *vm, int value);
void check_type(int type, int cnt, ...);
void run(VM *vm);

// initialise a new x2017 virtual machine
// copy the code from all functions into a long array
// move the program counter to label 0, where the vm should start
// registers are set to be zero
VM* vm_setup(ASC_BINARY **ab_array, int num_of_fns) {
    VM *vm = malloc(sizeof(VM));
    vm->raw_array = malloc(sizeof(int*));
    vm->RAM = calloc(256, sizeof(BYTE)); // 2^8 address of 1 byte each
    vm->registers = calloc(8, sizeof(BYTE)); // 8 registers of 1 byte each
 
    // vm->raw_array[0]= memcpy(malloc(sizeof(int)*1), (int[1]) {1005}, sizeof(int)*1);
 
    size_t len = 0;
    int found = 0;

    for(int i = 0; i < num_of_fns; i++){
        len++;
        vm->raw_array = realloc(vm->raw_array, (len+ab_array[i]->num_of_ins)*sizeof(int*));
        vm->raw_array[len-1] = malloc(sizeof(int));
        vm->raw_array[len-1][0]  = ab_array[i]->label + 1000;
        if(vm->raw_array[len-1][0] == 1000) found = 1;

        for(int j = 0; j < ab_array[i]->num_of_ins; j++){
            vm->raw_array[len+j] = malloc(sizeof(int)*ab_array[i]->line_cnt[j]);
            memcpy(vm->raw_array[len+j], ab_array[i]->ops_asc[j], sizeof(int)*ab_array[i]->line_cnt[j]);
        }
        len += ab_array[i]->num_of_ins;
    }

    if(len >= 256) {
        fprintf(stderr, "Program code out of bound\n");
        exit(1);
    }
    if(!found){
        fprintf(stderr, "FUNC LABEL 0 does not exist\n");
        exit(1);
    }
    for(int i = 0; i < len; i++){
        if(vm->raw_array[i][0] == 1000) vm->registers[7] = i;
    }
    vm->registers[6] = len;
    return vm;
        
}

// return the address of given argument, VAL and REG addresses cannot be retrieved
int read_address(VM *vm, int type, int value) {
    switch(type)
    {
        case 0: // VAL 8 bit
            fprintf(stderr, "VAL address cannot be referred to");
            exit(1);   
        case 1: //REG 3 bit
            fprintf(stderr, "REG address cannot be referred to");
            exit(1);
        case 2: // STK 5 bit
        /// return the actual address or address within the stack frame??????????
            return check_stk_exist(vm, value); 
        case 3: // PTR 5 bit
            return vm->RAM[check_stk_exist(vm, value)];       
    }
    // value is not one of 00, 01, 10, 11
    fprintf(stderr, "Type not exist 1\n");
    exit(1);

}

// return the value stored in given arugment
int read_value(VM *vm, int type, int value) {
     switch(type)
    {
        case 0: // VAL 8 bit
            return value;
        case 1: //REG 3 bit
            return (int)vm->registers[value];
        case 2: // STK 5 bit
            return vm->RAM[check_stk_exist(vm, value)]; 
        case 3: // PTR 5 bit
            return vm->RAM[vm->RAM[check_stk_exist(vm, value)]];
    }
    // value is not one of 00, 01, 10, 11
    fprintf(stderr, "Type not exist 2\n");
    exit(1);

}

// assign value to the given argument
// here if STK with its unique symbol is not declared before, RAM will give space for a new STK
void assign_value(VM *vm, int type, int index, int value) {
    
    if(value > 255){
        fprintf(stderr, "Stack overflow\n");
        exit(1);
    }
    switch(type)
    {
        case 0: // VAL 8 bit
        // value cannot be assigned to value type, error should be caught
            fprintf(stderr, "Invalid type, please use the specified type\n");
            exit(1);

        case 1: //REG 3 bit
            vm->registers[index] = value;
            break;

        case 2: // STK 5 bit
        // loop through values from EBP to ESP
            for(int i = (int)vm->registers[5]; i < (int)vm->registers[4]; i++) {
                if ((i-vm->registers[5]) == index) vm->RAM[i] = value;
            }
        // not found existing stack symbol corresponding to the value, create a new one
        // here vm->registers[4] - vm->registers[5] - 1 = index;
            vm->RAM[vm->registers[4]++] = value;
            break;

        case 3: // PTR 5 bit
        // loop through values from EBP to ESP
            vm->RAM[vm->RAM[check_stk_exist(vm, index)]] = value; 
            break;   
    }
}

// check if the STK already exist, the absolute address in RAM will be returned
// if not, an error will be thrown
int check_stk_exist(VM *vm, int value) {
    for(int i = (int)vm->registers[5]; i < (int)vm->registers[4]; i++) {
        if ((i-vm->registers[5]) == value) return i;
    }
    // pointer to stack is not declared before, error
    fprintf(stderr, "Stack is not declared before, hence the pointer cannot be reached\n");
    exit(1);  
}

// each OPCODE has its own TYPE requirements, here is to check whether there is violation or not
// if so, an error of be thrown
void check_type(int type, int cnt, ...) {
    va_list valist;
    va_start(valist, cnt);
    for (int i = 0; i < cnt; i++) {
        if(type == va_arg(valist, int)) {
            fprintf(stderr, "Invalid type, please use the specified type\n");
            exit(1);
        }
    }
    va_end(valist);
}

// the heart of x2017 vm, beating as the program counter incrementing
// a total of 8 possible operations are handled separately
void run(VM *vm) {     
    vm->registers[7]++;
    // run opcode
    // printf("opcode: %d\n", vm->raw_array[(int)vm->registers[7]][0]);
    switch(vm->raw_array[(int)vm->registers[7]][0])
    {
        case 0: // MOV A B
        // Copies the value at some point B in memory to another point A in memory
        // (register or stack). The destination may not be value typed.
            
            // A cannot be VAL type
            check_type(vm->raw_array[(int)vm->registers[7]][1], 1, 0);
            assign_value(vm, vm->raw_array[(int)vm->registers[7]][1], vm->raw_array[(int)vm->registers[7]][2], 
                    read_value(vm, vm->raw_array[(int)vm->registers[7]][3], vm->raw_array[(int)vm->registers[7]][4]));
            // void assign_value(VM *vm, int type, int index, int value)
            break;

        case 1: // CAL A
        // Calls another function the first argument is a single byte 
        // (using the VALUE type) containing the label of the calling function.
            check_type(vm->raw_array[(int)vm->registers[7]][1], 3, 1, 2, 3);
            for(int i = 0; i < vm->registers[6]; i++) {    
                if(vm->raw_array[i][0] == 1000 + (int)vm->raw_array[(int)vm->registers[7]][2]){
                    // printf("%d\n", vm->raw_array[i][0]);
                    vm->RAM[vm->registers[4]++] = vm->registers[7];
                    vm->RAM[vm->registers[4]++] = vm->registers[5];
                    
                    // REG 5 EBP moves forward
                    vm->registers[5] = vm->registers[4];
                    // REG 7 moves to the FUNC LABEL A
                    vm->registers[7] = i;
                    return;
                }
            }
            fprintf(stderr, "FUNC LABEL %d is not found\n", vm->raw_array[(int)vm->registers[7]][2]);
            exit(1);    
    
        case 2: // RET
            if(vm->registers[5] == 0){
                // end of code
                exit(0);
            }
            // move pc to CAL operation
            vm->registers[7] = vm->RAM[vm->registers[5]-2];
            // move EBP & ESP to previous position
            vm->registers[4] = vm->registers[5]-2;
            vm->registers[5] = vm->RAM[vm->registers[5]-1];        
            break;

        case 3: // REF A B
        // Takes a stack symbol B and stores its corresponding stack address in A.
            check_type(vm->raw_array[(int)vm->registers[7]][1], 1, 0);
            check_type(vm->raw_array[(int)vm->registers[7]][3], 2, 0, 1);
            
            assign_value(vm, vm->raw_array[(int)vm->registers[7]][1], vm->raw_array[(int)vm->registers[7]][2],
                    read_address(vm, vm->raw_array[(int)vm->registers[7]][3], vm->raw_array[(int)vm->registers[7]][4]));
            break;
        
        case 4: // ADD A B
        // Takes two register addresses and ADDs their values,storing the 
        // result in the first listed register.
            check_type(vm->raw_array[(int)vm->registers[7]][1], 3, 0, 2, 3);
            check_type(vm->raw_array[(int)vm->registers[7]][3], 3, 0, 2, 3);
            vm->registers[vm->raw_array[(int)vm->registers[7]][2]] = (int)vm->registers[vm->raw_array[(int)vm->registers[7]][2]]+(int)vm->registers[vm->raw_array[(int)vm->registers[7]][4]];
            break;

        case 5: // PRINT A
        // Takes any address type and prints the contents to a new line of 
        // standard output as an unsigned integer.
            print_type(vm, vm->raw_array[(int)vm->registers[7]][1], vm->raw_array[(int)vm->registers[7]][2]);
            break;

        case 6: // NOT A
        // Takes a register address and performs a bitwise not operation on the 
        //value at that address. The result is stored in the same register
            check_type(vm->raw_array[(int)vm->registers[7]][1], 3, 0, 2, 3);
            vm->registers[vm->raw_array[(int)vm->registers[7]][2]] = ~vm->registers[vm->raw_array[(int)vm->registers[7]][2]];
            break;

        case 7: // EQU A
        // Takes a register address and tests if it equals zero. 
        // The value in the register will be set to 1 if it is 0, 
        // or 0 if it is not. The result is stored in the same register.  
            check_type(vm->raw_array[(int)vm->registers[7]][1], 3, 0, 2, 3);
            if(vm->registers[vm->raw_array[(int)vm->registers[7]][2]] == 0) vm->registers[vm->raw_array[(int)vm->registers[7]][2]] = 1;
            else vm->registers[vm->raw_array[(int)vm->registers[7]][2]] = 0;
            break;
    }
    // printf("PC: %d\n", (int)vm->registers[7]);
    // printf("EBP: %d\n", (int)vm->registers[5]);
    // printf("ESP: %d\n", (int)vm->registers[4]);        
    // printf("vm->registers[0]: %d\n", (int)vm->registers[0]);
    // printf("vm->registers[1]: %d\n", (int)vm->registers[1]);
    // printf("vm->registers[2]: %d\n", (int)vm->registers[2]);
    // printf("STK A: %d\n", (int)vm->RAM[vm->registers[5]]);
    // printf("STK B: %d\n", (int)vm->RAM[vm->registers[5]+1]);
}

// print the value specified by the given type
void print_type(VM *vm, int type, int value) {
    switch (type)
    {
        case 0:
            printf("%u\n", value);
            break;
        case 1:
            printf("%u\n", (int)vm->registers[value]);
            break;
        case 2:
            printf("%u\n", (int)vm->RAM[vm->registers[5]+value]);
            break;
        case 3:
            printf("%u\n", (int)vm->RAM[vm->RAM[vm->registers[5]+value]]);
            break;
    }
        
}

int main(int argc, char ** argv) {
    char* op_binary;
    int length;
    int num_of_fns;

    if(argc != 2) {
        fprintf(stderr, "incorrect command line argument");
        return 1;
    }
       
    op_binary = read_file(argv[1], &length);
    ASC_BINARY **ab_array = decode_binary(op_binary, length, &num_of_fns);

    // for(int i = 0; i < num_of_fns; i++){   
    //     printf("label: %d\n", ab_array[i]->label); 
    //     for(int j = 0; j < ab_array[i]->num_of_ins; j++) {
    //         int x = ab_array[i]->line_cnt[j];
    //         for(int k = 0; k < x; k++){
    //             printf("%d ", ab_array[i]->ops_asc[j][k]);
    //         }
    //     printf("\n");
    //     }
    // }

    VM *vm = vm_setup(ab_array, num_of_fns);

    
    // printf("In VM: \n");
    // int len = 0;
    // for(int i = 0; i < num_of_fns; i++){ 
    //     len++;
    //     printf("label: %d\n", vm->raw_array[len-1][0]);
    //     for(int j = 0; j < ab_array[i]->num_of_ins; j++) {
    //         for(int k = 0; k < ab_array[i]->line_cnt[j]; k++){
    //             printf("%d ", vm->raw_array[len+j][k]);
    //         }
    //     printf("\n");
    //     }
    //     len += num_of_fns;
    // }

    // int cnt = 0;
    while (!(vm->raw_array[(int)vm->registers[7]][0] == 2 && (int)vm->registers[5] == 0))
    {
        if((int)vm->registers[4] >= 254) { 
            // since CAL append 2 values on stack each time, there is no enough space for a CAL operation
                fprintf(stderr, "Stack overflow\n");
                exit(1);
        }
        run(vm);
    }
    int len = 0;
    for(int i = 0; i < num_of_fns; i++){
        len++;
        vm->raw_array = realloc(vm->raw_array, (len+ab_array[i]->num_of_ins)*sizeof(int*));
        free(vm->raw_array[len-1]);
        for(int j = 0; j < ab_array[i]->num_of_ins; j++){
            free(vm->raw_array[len+j]);
        }
        len += ab_array[i]->num_of_ins;
    }

    free(vm->raw_array);
    free(vm->RAM);
    free(vm->registers);
    free(vm);

    free(op_binary);
    for(int i = num_of_fns-1; i >= 0; i--){
        for(int j = 0; j < ab_array[i]->num_of_ins; j++) {
            free(ab_array[i]->ops_asc[j]);
        }

        free(ab_array[i]->line_cnt);
        free(ab_array[i]->ops_asc);
        free(ab_array[i]->stk_ptr);
    }

    for(int i = 0; i < num_of_fns; i++) {
        free(ab_array[i]);
    }
    free(ab_array);
    
}