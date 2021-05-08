

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "x2017_decoder.c"

int read_opcode(ASC_BINARY *ab, int line);
int read_type(ASC_BINARY *ab, int line, int arg);

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
    
    free(op_binary);

    for(int i = num_of_fns-1; i >= 0; i--){
        printf("FUNC LABEL %d\n", ab_array[i]->label);
        for(int j = 0; j < ab_array[i]->num_of_ins; j++) {
            printf("    ");
            read_opcode(ab_array[i], j);
            printf("\n");
        }

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

    return 0;
}

// receives an OPCODE to print and determine the following argument
int read_opcode(ASC_BINARY *ab, int line){
    OPCODE oc = ab->ops_asc[line][0];
    switch (oc)
    {
    case MOV:
        printf("MOV ");
        break;
    case CAL:
        printf("CAL ");
        break;
    case RET:
        printf("RET");
        return 0;
    case REF:
        printf("REF ");
        break;
    case ADD:
        printf("ADD ");
        break;
    case PRINT:
        printf("PRINT ");
        break;
    case EQU:
        printf("EQU ");
        break;
    case NOT:
        printf("NOT ");
        break;

    default:
        return -1;
    }
    read_type(ab, line, 1);
    if(ab->line_cnt[line] == 5) {
        printf(" ");
        read_type(ab, line, 3);
    }
    return 0;               
}

// receives a TYPE to print itself and the following value
int read_type(ASC_BINARY *ab, int line, int arg){
    
    TYPE t = ab->ops_asc[line][arg];
    switch (t) 
    {
    case VAL:
        printf("VAL %d", ab->ops_asc[line][arg+1]);
        break;
    case REG:
        printf("REG %d", ab->ops_asc[line][arg+1]);
        break;
    case STK:
        if(ab->ops_asc[line][arg+1] < 26) printf("STK %c", ab->ops_asc[line][arg+1]+65);
        else printf("STK %c", ab->ops_asc[line][arg+1]+71);
        break;        
    case PTR:
        if(ab->ops_asc[line][arg+1] < 26) printf("PTR %c", ab->ops_asc[line][arg+1]+65);
        else printf("PTR %c", ab->ops_asc[line][arg+1]+71);
        break; 
    default:
        return -1;
    }

    return 0;

}