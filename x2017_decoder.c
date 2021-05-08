

#include "opcode.h"

char* read_file(char *file_path, int *length);
void bit_check(int* byte, int* bit);
ASC_BINARY** decode_binary(char* op_binary, int length, int *num_of_fns);
int** chars_to_binary(char *c, int length);
int bit_by_bit(RAW_BINARY *rb, size_t len);
int* decode_op(RAW_BINARY *rb, size_t *len);
ASC_BINARY* decode_multi(RAW_BINARY *rb);
int type(int name);
void simplify_symbol(ASC_BINARY *ab);

// read from a file and return a char array with its length
char* read_file(char *file_path, int *length){

    FILE* fp = fopen(file_path,"rb");
        if(fp == NULL){
            perror("unable to open file");
            exit(1);
        }

    fseek(fp, 0, SEEK_END);
    *length = ftell(fp);
    char* op_binary = malloc((*length)*sizeof(char));
    rewind(fp);
    fread(op_binary, *length, 1, fp);

    fclose(fp);
    return op_binary;
}

// turn the char array from file to a ASC_BINARY array 
// ASC_BINARY stores the decoded binary code in ascending order
// it also contains the stack symbol correspondences
ASC_BINARY** decode_binary(char* op_binary, int length, int *num_of_fns) {
    
    *num_of_fns = 0;
    int** binary_array = chars_to_binary(op_binary, length);
    ASC_BINARY **ab_array = malloc(sizeof(ASC_BINARY*));

    RAW_BINARY *rb = &(RAW_BINARY) {
    .op_binary = binary_array,
    .byte = length-1,
    .bit = 7
    };

    while(rb->byte > 0){
        *num_of_fns = *num_of_fns + 1;
        ab_array = realloc(ab_array, sizeof(ASC_BINARY*)*(*num_of_fns)); 
        ab_array[*num_of_fns-1] = decode_multi(rb);
    }

    for(int i = 0; i < length; i++) {
        free(binary_array[i]);
    }
    
    free(binary_array);

    return ab_array;
}

// read the byte array in RAW_BINARY certain bits
// RAW_BIANRY stores the byte array retrived from hex file
// it contains the cursor represented by bit and byte value
int bit_by_bit(RAW_BINARY *rb, size_t len){
    int ret = 0;
    int base = 1;
    for(int j = 0; j < len; j++) {
        ret += rb->op_binary[rb->byte][rb->bit] * base;
        base *= 2;
        bit_check(&(rb->byte), &(rb->bit));
    }
    // printf("byte: %d, bit: %d\n",rb->byte, rb->bit);
    

    return ret;

}

// convert char array into byte array matrix
// each int only stores either 0 or 1, hence 8 int blocks as a byte
int** chars_to_binary(char* c, int length) {
    int i;
    int *binary;
    int **binary_array = malloc(sizeof(int*)*length);
    for(int j = 0; j < length; j ++){
        // printf("%c", *(c+j));
        binary = malloc(8*sizeof(int));
        for (i = 0; i < 8; i++) {
            binary[i] = !!((*(c+j) << i) & 0x80);
            // printf("%d", binary[i]);
        }
        binary_array[j] = binary;   
        // printf(" ");
    }
    
    return binary_array;
}
// when bit is 0 byte needs to minus one to move forward
void bit_check(int* byte, int* bit) {
    (*bit)--;
    if(*bit < 0){
        (*byte) --;
        *bit = 7;
    }
}
// decode a single line of operation in a function block, also figure out operation code length
int* decode_op(RAW_BINARY *rb, size_t *len) {
    int *asc_decode = malloc(sizeof(int));
    int name = bit_by_bit(rb, 3);
    *asc_decode = name;

    if (name == 2) {
        *len = 1;
        return asc_decode;
    }
    asc_decode = realloc(asc_decode, 3*sizeof(int));
    asc_decode[1] = bit_by_bit(rb, 2);
    asc_decode[2] = bit_by_bit(rb, type(asc_decode[1]));

    if(name == 1 || name == 5 || name == 6 || name == 7) {
        *len = 3;
        return asc_decode;
    }

    asc_decode = realloc(asc_decode, 5*sizeof(int));
    asc_decode[3] = bit_by_bit(rb, 2);
    asc_decode[4] = bit_by_bit(rb, type(asc_decode[3]));

    if(name == 0 || name == 3 || name == 4) {
        *len = 5;
        return asc_decode;
    }

    return NULL;
}

ASC_BINARY* decode_multi(RAW_BINARY *rb){

    int num_of_ins = bit_by_bit(rb, 5);
    ASC_BINARY *ab = malloc(sizeof(ASC_BINARY));
    
    ab->line_cnt = malloc(sizeof(size_t)*num_of_ins);
    ab->ops_asc = malloc(sizeof(int*)*num_of_ins);
    ab->num_of_ins = num_of_ins;
    ab->stk_cnt = 0;
    ab->stk_ptr = malloc(sizeof(int));

    for(int i = num_of_ins-1; i >= 0; i--) {
        ab->ops_asc[i] = decode_op(rb, &ab->line_cnt[i]);
    }
    // printf("inside:\n");
    // for(int j = 0; j < num_of_ins; j++) {
    //         int x = ab->line_cnt[j];
    //         for(int k = 0; k < x; k++){
    //             printf("%d ", ab->ops_asc[j][k]);
    //         }
    //     printf("\n");
    // }

    simplify_symbol(ab);


    ab->label = bit_by_bit(rb, 3);
    return ab;

}

// find the length of following value should be, refering to its type
int type(int name) {

    if(name == 0) return 8;
    if(name == 1) return 3;
    if(name == 2 || name == 3) return 5;

    return 0;

}

// symbol will be simplified before going into virtual machine x2017
// the value will be reassigned based on appearance
// the correspondence of original value and assigned value is stored in ASC_BINARY struct 
void simplify_symbol(ASC_BINARY *ab) {
    // convert symbol appearance to be in order
    for(int i = 0; i < ab->num_of_ins; i++){
        if(ab->line_cnt[i]>= 3 && (ab->ops_asc[i][1] == 2 || ab->ops_asc[i][1] == 3)) {
            int l;
            for(l = 0; l < ab->stk_cnt; l++) {
                if(ab->stk_ptr[l] == ab->ops_asc[i][2]) {
                    ab->ops_asc[i][2] = l;
                    break;
                }
            }
            // not found previous declared symbol, create new STK symbol, PTR can only exist if STK already exists
            if(l == ab->stk_cnt) {
                if(ab->ops_asc[i][1] == 2){
                    ab->stk_cnt++;
                    ab->stk_ptr = realloc(ab->stk_ptr, sizeof(int)*ab->stk_cnt);
                    ab->stk_ptr[ab->stk_cnt-1] = ab->ops_asc[i][2];
                    ab->ops_asc[i][2] = ab->stk_cnt-1;
                }else{
                    fprintf(stderr, "Stack is not declared before, hence the pointer cannot be reached\n");
                    exit(1);  
                }
            }
        }

        if(ab->line_cnt[i]>= 5 && (ab->ops_asc[i][3] == 2 || ab->ops_asc[i][3] == 3)) {
            int l;
            for(l = 0; l < ab->stk_cnt; l++) {
                if(ab->stk_ptr[l] == ab->ops_asc[i][4]) {
                    ab->ops_asc[i][4] = l;
                    break;
                }
            }
            // not found previous declared symbol, create new 
            if(l == ab->stk_cnt) {
                if(ab->ops_asc[i][3] == 2){
                    ab->stk_cnt++;
                    ab->stk_ptr = realloc(ab->stk_ptr, sizeof(int)*ab->stk_cnt);
                    ab->stk_ptr[ab->stk_cnt-1] = ab->ops_asc[i][4];
                    ab->ops_asc[i][4] = ab->stk_cnt-1;
                }else{
                    fprintf(stderr, "Stack is not declared before, hence the pointer cannot be reached\n");
                    exit(1);  
                }
            }

        }
        
    }
}