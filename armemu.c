#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#define NREGS 16
#define SP 13
#define LR 14
#define PC 15

// functions to be emulated, calls the implementations of these methods from the assembly files included
int sum_array_a(int *a, int len);
int find_max_a(int *array, int len);
int fib_iter_a_v2(int nth_val);
int fib_rec_a(int n);
int find_str_a(char *s, char *sub);
int test_a(int a, int b);

int tarr[2000]; // test array

// struct that keeps track of all the different registers 
struct arm_state {
    unsigned int regs[NREGS]; // array of registers
    unsigned int cpsr;
    unsigned int stack_size;
    unsigned char *stack;
    unsigned int num_instructions_total;
    unsigned int data_proc_instr_counter; // Keeps track of data processing instructions
    unsigned int memory_instr_counter; // Keeps track of memory processing instructions
    unsigned int branch_instr_counter; // Keeps track of branch instructions
};

// Return type is struct for this funcion
struct arm_state *arm_state_new(unsigned int stack_size, unsigned int *func, 
        unsigned int arg0, unsigned int arg1, 
        unsigned int arg2, unsigned int arg3) 
{
    struct arm_state *as;
    int i;

    as = (struct arm_state *) malloc(sizeof(struct arm_state));
    
    as->num_instructions_total = 0;
    as->data_proc_instr_counter = 0;
    as->memory_instr_counter = 0;
    as->branch_instr_counter = 0;
    

    if (as == NULL) {
        printf("malloc() failed, exiting. \n");
        exit(-1);
    }

    as->stack = (unsigned char *) malloc(stack_size);

    if (as->stack == NULL) {
        printf("malloc() failed, exiting. \n");
        exit(-1);
    }

    as->stack_size = stack_size;

    //Initialize all registers to zero 
    for (i = 0; i < NREGS; i++) {
        as->regs[i] = 0;
    }

    as->regs[PC] = (unsigned int) func;
    as->regs[SP] = (unsigned int) as->stack + as->stack_size;
    as->cpsr = 0;

    as->regs[0] = arg0;
    as->regs[1] = arg1;
    as->regs[2] = arg2;
    as->regs[3] = arg3;

    return as;
}

void arm_state_free(struct arm_state *as) {
    free(as->stack);
    free(as);
}

void arm_state_print(struct arm_state *as){
    int i;

    printf("stack size = %d\n", as->stack_size);

    for(int i = 0; i < NREGS; i++) {
        printf("regs[%d] = (%X) %d\n", i, as->regs[i], (int) as->regs[i]);
    }
 
}

// CONDITION FUNCTIONS
//---------------------------------------------------------------------------------
bool is_eq_cond(unsigned int iw){
    unsigned int cond;

    cond = (iw >> 28) & 0b1111;
    return (cond == 0);
}

bool is_ne_cond(unsigned int iw){
    unsigned int cond;
    cond = (iw >> 28) & 0b1111;
    return (cond == 1);
}

bool is_ge_cond(unsigned int iw){
    unsigned int cond;

    cond = (iw >> 28) &0b1111;
    return (cond == 10);
}

bool is_lt_cond(unsigned int iw){
    unsigned int cond;
    
    cond = (iw >> 28) &0b1111;
    return (cond == 11);
}

bool is_gt_cond(unsigned int iw){
    unsigned int cond;

    cond = (iw >> 28) &0b1111;
    return (cond == 12);
}

bool is_le_cond(unsigned int iw){
    unsigned int cond;
   
    cond = (iw >> 28) &0b1111;
    return (cond == 13);
}

bool is_al_cond(unsigned int iw) {
    unsigned int cond;

    cond = (iw >> 28) &0b1111;
    return (cond == 14);
}

//-----------------------------------------------------------------------------------------------

// FLAG CHECK, SET, AND CLEAR FUNCTIONS
//-----------------------------------------------------------------------------------------------
bool is_n_flag_set(unsigned int cpsr){
    unsigned int n;

    n = (cpsr >> 31) &0b1;
    return (n == 1);
}

bool is_z_flag_set(unsigned int cpsr){
    unsigned int z;

    z = (cpsr >> 30) &0b1;
    return (z == 1);
}

bool is_c_flag_set(unsigned int cpsr){
    unsigned int c;

    c = (cpsr >> 29) &0b1;
    return (c == 1);
}

bool is_v_flag_set(unsigned int cpsr){
    unsigned int v;

    v = (cpsr >> 28) &0b1;
    return (v == 1);
}

unsigned int set_n_flag(unsigned int cpsr){
    cpsr |= 1 << 31;
    return cpsr;
}

unsigned int set_z_flag(unsigned int cpsr){
    cpsr |= 1 << 30;
    return cpsr;
}

unsigned int set_c_flag(unsigned int cpsr){
    cpsr |= 1 << 29;
    return cpsr;
}

unsigned int set_v_flag(unsigned int cpsr){
    cpsr |= 1 << 28;
    return cpsr;
}

unsigned int clear_n_flag(unsigned int cpsr){
    cpsr &= ~(1 << 31);
    return cpsr;
}

unsigned int clear_z_flag(unsigned int cpsr){
    cpsr &= ~(1 << 30);
    return cpsr;
}

unsigned int clear_c_flag(unsigned int cpsr){
    cpsr &= ~(1 << 29);
    return cpsr;
}

unsigned int clear_v_flag(unsigned int cpsr){
    cpsr &= ~(1 << 28);
    return cpsr;
}

unsigned int get_n_flag(unsigned int cpsr){
    unsigned int n;

    n = (cpsr >> 31) &0b1;
    return n;
}

unsigned int get_z_flag(unsigned int cpsr){
    unsigned int z;

    z = (cpsr >> 30) &0b1;
    return z;
}

unsigned int get_c_flag(unsigned int cpsr){
    unsigned int c;

    c = (cpsr >> 29) &0b1;
    return c;
}

unsigned int get_v_flag(unsigned int cpsr){
    unsigned int v;

    v = (cpsr >> 28) &0b1;
    return v;
}

// Checks to see if the condition is met for execution
bool is_cond_met(unsigned int iw, unsigned int cpsr){
    bool met = false;
    unsigned int n = get_n_flag(cpsr);
    unsigned int v = get_v_flag(cpsr);

    if(is_al_cond(iw)){
        met = true;
    } else if(is_eq_cond(iw)){
        met = is_z_flag_set(cpsr);
    } else if(is_ne_cond(iw)){
        // Z flag should be clear for this condition to be met
        if( !(is_z_flag_set(cpsr)) ){
            met = true;
        } else {
            met = false;
        }
    } else if (is_ge_cond(iw)){
        if(n == v){
            met = true;
        } else {
            met = false;
        }
    } else if (is_lt_cond(iw)){
        if(n != v){
            met = true;
        } else {
            met = false;
        }
    } else if (is_gt_cond(iw)){
        if( !(is_z_flag_set(cpsr)) && n == v){
            met = true;
        } else {
            met = false;
        }
    } else if (is_le_cond(iw)){
        if( (is_z_flag_set(cpsr)) || n != v){
            met = true;
        } else {
            met = false;
        }
    }

    return met;    
}

//--------------------------------------------------------------------------------------------------

// Tells you if operand2 is an immediate, register, or register-shifted register
int check_i_bit(unsigned iw){
    unsigned int i = (iw >> 25) &0b1;
    return i;
}

// Function that takes in iw and finds out what type of shift (sh) to do
int get_shift_type(unsigned int iw) {
    unsigned int sh = (iw >> 5) & 0b11;
    return sh;
}

bool iw_is_add_instruction(unsigned int iw) {
    unsigned int op;
    unsigned int opcode;

    op = (iw >> 26) & 0b11;
    opcode = (iw >> 21) & 0b1111;

    return (op == 0) && (opcode == 4);
}

bool iw_is_sub_instruction(unsigned int iw){
    unsigned int op;
    unsigned int opcode;

    op = (iw >> 26) & 0b11;
    opcode = (iw >> 21) & 0b1111;

   return (op == 0) && (opcode == 2);
}

bool iw_is_cmp_instruction(unsigned int iw){
    unsigned int op;
    unsigned int opcode;

    op = (iw >> 26) &0b11;
    opcode = (iw >> 21) & 0b1111;

    return (op == 0) && (opcode == 10);
}

bool iw_is_mov_instruction(unsigned int iw){
    unsigned int op;
    unsigned int opcode;

    op = (iw >> 26) &0b11;
    opcode = (iw >> 21) &0b1111;

    return (op == 0) && (opcode == 13);
}

bool iw_is_mvn_instruction(unsigned int iw){
    unsigned int op;
    unsigned int opcode;

    op = (iw >> 26) &0b11;
    opcode = (iw >> 21) &0b1111;

    return (op == 0) && (opcode == 15);
}


void execute_data_proc_instruction(struct arm_state *as, unsigned int iw) {
    unsigned int rd; // register destination
    unsigned int rn; // 1st operand
    unsigned int rm; // 2nd operand (format varies)
    unsigned int rs; // for register-shifted register format
    unsigned int i; // used for the format of rm
    unsigned int rot; // rotation for the immediate
    unsigned int shamt5; // amount to shift value by
    unsigned int sh; // type of shift
    unsigned int fourth_bit; // 4th bit in rm when i = 0
    int result; // for cmp instructions
    int negative; // for mov instruction check
    bool cond; // checks condition

    cond = is_cond_met(iw, as->cpsr);

    if(cond) {
        rd = (iw >> 12) &0b1111;
        rn = (iw >> 16) &0b1111;
        // Amount of bits in rm depends on the formatting of the whole 2nd operand
        // so that'll be checked later

        i = check_i_bit(iw); // bit 25, checks format of rm

        // indicates the rm is an immediate value
        if(i == 1){
            rm = iw &0b11111111;
            rot = (iw >> 8) &0b1111;
        } else if (i == 0) { // means that rm is either a register or a register-shifted register
            fourth_bit = (iw >> 4) &0b1;// tells me whether rm is a register or a register-shifted register
            rm = iw &0b1111;
            sh = (iw >> 5) &0b11;

            if(fourth_bit == 0){ // register case
                shamt5 = (iw >> 7) & 0b11111;

                if(sh == 0){ // lsl
                    as->regs[rm] = as->regs[rm] << shamt5;
                } else if (sh == 1){ // lsr
                    as->regs[rm] = as->regs[rm] >> shamt5;
                }

            } else if (fourth_bit == 1){ // register-shifted register case
                rs = (iw >> 8) &0b1111; // "Shift amount specified in the bottom byte of rs"

                if(sh == 0) { // lsl
                    as->regs[rm] = as->regs[rm] << as->regs[rs];
                } else if (sh == 1){ // lsr
                    as->regs[rm] = as->regs[rm] >> as->regs[rs];
                } 
            }
        }

        // Now that the data has been extracted, it's a matter of doing the correct operation
        if(iw_is_add_instruction(iw)){
            if(i == 1){ // immediate value in rm
                as->regs[rd] = as->regs[rn] + rm;
            } else if (i == 0){ // register in rm
                as->regs[rd] = as->regs[rn] + as->regs[rm];
            }
        } else if (iw_is_sub_instruction(iw)){
            if(i == 1){ // immediate value in rm
                as->regs[rd] = as->regs[rn] - rm;
            }else if(i == 0){ // register in rm
                as->regs[rd] = as->regs[rn] - as->regs[rm];
            }
        } else if (iw_is_mov_instruction(iw)){
            if(i == 1){ // immediate value in rm
                as->regs[rd] = rm;
            } else if (i == 0){ // register in rm
                as->regs[rd] = as->regs[rm];
            }
        } else if (iw_is_mvn_instruction(iw)){
            if(i == 1) { // immediate value in rm
                as->regs[rd] = ~(rm);
            } else if (i == 0){ // register in rm
                as->regs[rd] = ~(as->regs[rm]);
            }

        } else if (iw_is_cmp_instruction(iw)){  
            if(i == 1){ // immediate value in rm
                result = as->regs[rn] - rm;

                if(as->regs[rn] >= rm){
                    as->cpsr = set_c_flag(as->cpsr);
                } else {
                    as->cpsr = clear_c_flag(as->cpsr);
                }

            } else if (i == 0){ //register in rm
                result = as->regs[rn] - as->regs[rm];
                
                if(as->regs[rn] >= as->regs[rm]){
                    as->cpsr = set_c_flag(as->cpsr);
                } else {
                    as->cpsr = clear_c_flag(as->cpsr);
                }
            }

            if(result == 0){ //equal values
                as->cpsr = set_z_flag(as->cpsr);
                as->cpsr = clear_n_flag(as->cpsr);
            } else if (result != 0){ //not equal values
                as->cpsr = clear_z_flag(as->cpsr);
                if(result < 0){
                    as->cpsr = set_n_flag(as->cpsr);
                } else if (result > 0){
                    as->cpsr = clear_n_flag(as->cpsr);
                }
            }

        }
    }

    as->regs[PC] = as->regs[PC] + 4;
}

bool iw_is_bx_instruction(unsigned int iw) {
    return ((iw >> 4) & 0xFFFFFF) == 0b000100101111111111110001;
}

bool iw_is_data_proc_instruction(unsigned int iw){
    return ((iw >> 26) &0b11) == 0b00;
}

bool iw_is_mem_instruction(unsigned int iw) {
    return ((iw >> 26) & 0b11)  == 0b01;
}

void execute_mem_instruction_test(struct arm_state *as, unsigned int iw) {
    unsigned int rn;
    unsigned int rn_offset; // rn after adding the offset, accounts for when I keep the old base value 
    unsigned int rd;
    unsigned int rm;
    unsigned int i;
    unsigned int p;
    unsigned int u;
    unsigned int b;
    unsigned int w;
    unsigned int l;
    unsigned int sh;
    unsigned int fourth_bit;
    unsigned int shamt5;
    unsigned int rs;
    unsigned int temp_reg;
    unsigned int offset;
    unsigned int *a;
    unsigned int target_address;
    bool cond;

    cond = is_cond_met(iw, as->cpsr);

    if(cond) {
        rd = (iw >> 12) &0b1111;
        rn = (iw >> 16) &0b1111;
        i = (iw >> 25) &0b1;
        p = (iw >> 24) &0b1;
        u = (iw >> 23) &0b1;
        b = (iw >> 22) &0b1;
        w = (iw >> 21) &0b1;
        l = (iw >> 20) &0b1;

        if(i == 0){ // immediate value
            offset = (iw) &0b111111111111;
        } else if (i == 1){ // offset register
            fourth_bit = (iw << 4) & 0b1;
            rm = (iw) &0b1111; // offset register BEFORE shift 
            sh = get_shift_type(iw);

            if(fourth_bit == 0){
                shamt5 = (iw >> 7) &0b11111;
   
                if(sh == 0) { // lsl
                    offset = as->regs[rm] << shamt5;
                } else if (sh == 1){ // lsr
                    offset = as->regs[rm] >> shamt5;
                }

            } else if (fourth_bit == 1){
                rs = (iw >> 8) &0b1111;
                
                if(sh == 0){ // lsl
                    offset = as->regs[rm] << as->regs[rs];
                } else if (sh == 1){ // lsr
                    offset = as->regs[rm] >> as->regs[rs];
                }
            }
        }

        if(u == 0){ // subtract offset from base
            if(w == 0){ // no write-back of modified base value back into base
                if(p == 1){ // sub offset before transfer
                    rn_offset = as->regs[rn] - offset;
                }
            } else if (w == 1){ // writes new value into base, page 26 of manual
                if(p == 1){ // sub offset before transfer
                    as->regs[rn] = as->regs[rn] - offset;
                }
            }
        } else if (u == 1){ // add offset to base
            if(w == 0){ // no write-back
                if(p == 1){// add offset before transfer
                    rn_offset = as->regs[rn] + offset;
                }
            } else if (w == 1){ // writes new value into base
                if(p == 1){ // add offset before transfer
                    as->regs[rn] = as->regs[rn] + offset;
                }
            }
        }

        if(l == 0){ // store in memory
            if(b == 0){ // transfer word quantity (str)
                if(w == 0){ // indicates that I need to use rn_offset
                   target_address = rn_offset;
                   * (unsigned int *) target_address = (unsigned int) as->regs[rd];
                } else if (w == 1){ // indicates that I need to use as->regs[rn]
                    target_address = as->regs[rn];
                    * (unsigned int *) target_address = (unsigned int) as->regs[rd];
                }

            } else if(b == 1){ // transfer byte quantity (strb)
                * (unsigned char *) target_address = (unsigned char) as->regs[rd];
            }

        } else if (l == 1){ // load from memory
            if(b == 0){ // transfer word quantity (ldr)
                if(w == 0) { // indicates that I need to use rn_offset
                    target_address = rn_offset;
                    as->regs[rd] = * (unsigned int *) target_address;
                } else if (w == 1){ // indicates that I need to use as->regs[rn]
                    target_address = as->regs[rn];
                    as->regs[rd] = * (unsigned int *) target_address;
                }

            } else if (b == 1){ // transfer byte quantity (ldrb)
                unsigned char *a;
                if(w == 0){ // indicates that I need to use rn_offset
                    target_address = rn_offset;
                    a = (unsigned char *) target_address;
                    as->regs[rd] = (unsigned int) *a;
                } else if (w == 1){ // indicates that I need to use as->regs[rn]
                    target_address = as->regs[rn];
                    a = (unsigned char *) target_address;
                    as->regs[rd] = (unsigned int) *a;
                }
            }
        }
    }

    as->regs[PC] = as->regs[PC] + 4;
}


void execute_bx_instruction(struct arm_state *as, unsigned int iw) {
    unsigned int rn;
    
    rn = iw & 0b1111;

    as->regs[PC] = as->regs[LR];
    as->regs[LR] = 0;
}

bool iw_is_bl_instruction(unsigned int iw){
    unsigned int op;
    unsigned int l;

    op = (iw >> 25) & 0b111;
    l = (iw >> 24) & 0b1;
    return (op == 5) && (l == 1);
} 

bool iw_is_b_instruction(unsigned int iw){
    unsigned int op;
    unsigned int l;

    op = (iw >> 25) & 0b111;
    l = (iw >> 24) & 0b1;
    return (op == 5) && (l == 0);
}

void execute_b_instruction(struct arm_state *as, unsigned int iw){
    int imm24;
    unsigned int cond;
    unsigned int msb;

    cond = is_cond_met(iw, as->cpsr);
    if(cond){ 

        imm24 = iw & 0xFFFFFF;
        msb = imm24 >> 23 &0b1;

        if(msb == 1){
            imm24 = imm24 | 0xFF000000;
        }

        imm24 = imm24 << 2;
        as->regs[PC] = as->regs[PC] + 8 + imm24;
    
    } else {
        as->regs[PC] = as->regs[PC] + 4;
    }
}

void execute_bl_instruction(struct arm_state *as, unsigned int iw){
    int imm24;
    unsigned int cond;
    unsigned int sign_extend;
    unsigned int msb;

    cond = is_cond_met(iw, as->cpsr);

    if(cond){       
        imm24 = iw & 0xFFFFFF;
        msb = imm24 >> 23 &0b1;

        //Sign extension
        if(msb == 1){
            imm24 = imm24 | 0xFF000000;
        }
        
        imm24 = imm24 << 2;
        as->regs[LR] = as->regs[PC] + 4;
        as->regs[PC] = as->regs[PC] + 8 + imm24;
    } else {
        as->regs[PC] = as->regs[PC] + 4;
    }
}

void state_analysis(struct arm_state *as){
    printf("\n");
    printf("                    Data Analysis               \n");
    printf("        Data Processing Instructions: %d\n", as->data_proc_instr_counter);
    printf("        Memory Processing Instructions: %d\n", as->memory_instr_counter);
    printf("        Branch Instructions: %d\n", as->branch_instr_counter);
    printf("        Total Number of Instructions: %d\n", as->num_instructions_total);
    printf("\n");
}

void arm_state_execute_one(struct arm_state *as) {
    unsigned int iw;
    unsigned int *pc;

    pc = (unsigned int *) as->regs[PC];
    iw = *pc;

   
    if(iw_is_bx_instruction(iw)) {
        execute_bx_instruction(as, iw);
        as->branch_instr_counter++;
    } else if (iw_is_mem_instruction(iw)){
        execute_mem_instruction_test(as, iw);
        as->memory_instr_counter++;
    } else if (iw_is_b_instruction(iw)){
        execute_b_instruction(as, iw);
        as->branch_instr_counter++; 
    } else if (iw_is_bl_instruction(iw)){
        execute_bl_instruction(as, iw);
        as->branch_instr_counter++;     
    } else if (iw_is_data_proc_instruction(iw)){
        execute_data_proc_instruction(as, iw);
        as->data_proc_instr_counter++;
    }

    as->num_instructions_total++;
}

unsigned int arm_state_execute(struct arm_state *as) {

    while (as->regs[PC] != 0) {
        arm_state_execute_one(as);
    }

    state_analysis(as);

    return as->regs[0];
}

void print_int_bits(unsigned int instr){
    int i;
    int len = 32;
    int b;
    for (i = 0; i < len; i++) {
        b = (instr >> ((len - 1) - i)) & 0b1;
        printf("%d", b);
    } 

    printf("\n");
    printf("\n");
}

// Takes as an argument: 1) the array, 2) the length, and 3) the starting value of the array.
void populate_array(int *array, int n, int st){
    int i;

    for(i = 0; i < n; i++) {
        array[i] = st;
        st++;
    }
}

void sum_array_test(){
    struct arm_state *state;
    int result;
    int length = 2000;
    int start = -200;
    int arr[] = {0, 101, 102, 300, 4};

    state = arm_state_new(1024, (unsigned int *) sum_array_a, (unsigned int) arr, 5, 0, 0);

    printf("**********************************************************************************\n");
    printf("SUM ARRAY TESTS: \n");
    printf("**********************************************************************************\n");
    printf("Testing on simple array arr[] \n");
    printf("arr = {0, 101, 102, 300, 4}\n");

    result = arm_state_execute(state);
    arm_state_free(state);
    printf("[N]: %d\n", sum_array_a(arr, 5));
    printf("[E]: %d\n", result);
    printf("\n");

    printf("----------------------------------------------------------------------------------\n");
    populate_array(tarr, length, start);
    state = arm_state_new(1024, (unsigned int *) sum_array_a, (unsigned int) tarr, length, 0, 0);
    printf("Testing an array with the following attributes: \n");
    printf("Start value: %d\n", start);
    printf("Length: %d\n", length);

    result = arm_state_execute(state);
    arm_state_free(state);
    printf("[N]: %d\n", sum_array_a(tarr, length));
    printf("[E]: %d\n", result);
    printf("\n");
    printf("----------------------------------------------------------------------------------\n");

    printf("----------------------------------------------------------------------------------\n");
    start = 0;
    populate_array(tarr, length, start);
    state = arm_state_new((1024), (unsigned int *) sum_array_a, (unsigned int) tarr, length, 0, 0);
    printf("Testing an array with the following attributes: \n");
    printf("Start value: %d\n", start);
    printf("Length: %d\n", length);

    result = arm_state_execute(state);
    arm_state_free(state);
    printf("[N]: %d\n", sum_array_a(tarr, length));
    printf("[E]: %d\n", result);
    printf("\n");
    printf("----------------------------------------------------------------------------------\n");

    printf("----------------------------------------------------------------------------------\n");
    start = -3000;
    populate_array(tarr, length, start);
    state = arm_state_new((1024), (unsigned int *) sum_array_a, (unsigned int) tarr, length, 0, 0);
    printf("Testing an array with the following attributes: \n");
    printf("Start value: %d\n", start);
    printf("Length: %d\n", length);

    result = arm_state_execute(state);
    arm_state_free(state);
    printf("[N]: %d\n", sum_array_a(tarr, length));
    printf("[E]: %d\n", result);
    printf("\n");
    printf("----------------------------------------------------------------------------------\n");

    printf("----------------------------------------------------------------------------------\n");
    start = 200;
    populate_array(tarr, length, start);
    state = arm_state_new((1024), (unsigned int *) sum_array_a, (unsigned int) tarr, length, 0, 0);
    printf("Testing an array with the following attributes: \n");
    printf("Start value: %d\n", start);
    printf("Length: %d\n", length);

    result = arm_state_execute(state);
    arm_state_free(state);
    printf("[N]: %d\n", sum_array_a(tarr, length));
    printf("[E]: %d\n", result);
    printf("\n");
    printf("----------------------------------------------------------------------------------\n");

    printf("**********************************************************************************\n");
}

void find_max_test(){
    struct arm_state *state;
    int result;
    int length = 2000;
    int start = -200;
    int arr[] = {0, 101, 102, 300, 4};

    state = arm_state_new(1024, (unsigned int *) find_max_a, (unsigned int) arr, 5, 0, 0);

    printf("**********************************************************************************\n");
    printf("Find MAX TEST: \n");
    printf("**********************************************************************************\n");
    printf("Testing on simple array arr[] \n");
    printf("arr = {0, 101, 102, 300, 4}\n");

    result = arm_state_execute(state);
    arm_state_free(state);
    printf("[N]: %d\n", find_max_a(arr, 5));
    printf("[E]: %d\n", result);
    printf("\n");

    printf("----------------------------------------------------------------------------------\n");
    populate_array(tarr, length, start);
    state = arm_state_new(1024, (unsigned int *) find_max_a, (unsigned int) tarr, length, 0, 0);
    printf("Testing an array with the following attributes: \n");
    printf("Start value: %d\n", start);
    printf("Length: %d\n", length);
    result = arm_state_execute(state);
    arm_state_free(state);
    printf("[N]: %d\n", find_max_a(tarr, length));
    printf("[E]: %d\n", result);
    printf("\n");
    printf("----------------------------------------------------------------------------------\n");

    printf("----------------------------------------------------------------------------------\n");
    start = 0;
    populate_array(tarr, length, start);
    state = arm_state_new(1024, (unsigned int *) find_max_a, (unsigned int) tarr, length, 0, 0);
    printf("Testing an array with the following attributes: \n");
    printf("Start value: %d\n", start);
    printf("Length: %d\n", length);
    result = arm_state_execute(state);
    arm_state_free(state);
    printf("[N]: %d\n", find_max_a(tarr, length));
    printf("[E]: %d\n", result);
    printf("\n");
    printf("----------------------------------------------------------------------------------\n");

    printf("----------------------------------------------------------------------------------\n");
    start = -3000;
    populate_array(tarr, length, start);
    state = arm_state_new(1024, (unsigned int *) find_max_a, (unsigned int) tarr, length, 0, 0);
    printf("Testing an array with the following attributes: \n");
    printf("Start value: %d\n", start);
    printf("Length: %d\n", length);
    printf("Placing value 12000 in tarr[200]\n");
    tarr[200] = 12000;
    result = arm_state_execute(state);
    arm_state_free(state);
    printf("[N]: %d\n", find_max_a(tarr, length));
    printf("[E]: %d\n", result);
    printf("\n");
    printf("----------------------------------------------------------------------------------\n");

    printf("----------------------------------------------------------------------------------\n");
    start = 300;
    populate_array(tarr, length, start);
    state = arm_state_new(1024, (unsigned int *) find_max_a, (unsigned int) tarr, length, 0, 0);
    printf("Testing an array with the following attributes: \n");
    printf("Start value: %d\n", start);
    printf("Length: %d\n", length);
    result = arm_state_execute(state);
    arm_state_free(state);
    printf("[N]: %d\n", find_max_a(tarr, length));
    printf("[E]: %d\n", result);
    printf("\n");
    printf("----------------------------------------------------------------------------------\n");

    printf("**********************************************************************************\n");
}

void fib_iter_test(){
    struct arm_state *state;
    int result;
    int i;

    printf("**********************************************************************************\n");
    printf("FIB ITER TEST: \n");
    printf("**********************************************************************************\n");
    
    for(i = 0; i < 7; i++){
        printf("----------------------------------------------------------------------------------\n");
        state = arm_state_new(1024, (unsigned int *) fib_iter_a_v2, i, 0, 0, 0);
        result = arm_state_execute(state);
        arm_state_free(state);
        printf("[N] fib number %d: %d\n", i, fib_iter_a_v2(i));
        printf("[E] fib number %d: %d\n", i, result);
        printf("\n");
        printf("----------------------------------------------------------------------------------\n");
    }

    printf("**********************************************************************************\n");
}

void fib_rec_test(){
    struct arm_state *state;
    int result;
    int i;

    printf("**********************************************************************************\n");
    printf("FIB REC TEST: \n");
    printf("**********************************************************************************\n");

    for(i = 0; i < 7; i++){
        printf("----------------------------------------------------------------------------------\n");
        state = arm_state_new(1024, (unsigned int *) fib_rec_a, i, 0, 0, 0);
        result = arm_state_execute(state);
        arm_state_free(state);
        printf("[N] fib number %d: %d\n", i, fib_iter_a_v2(i));
        printf("[E] fib number %d: %d\n", i, result);
        printf("\n");
        printf("----------------------------------------------------------------------------------\n");
    }

    printf("**********************************************************************************\n");
}

void find_str_test(){
    struct arm_state *state;
    int result;
    int i;

    printf("**********************************************************************************\n");
    printf("FIND STR TESTS: \n");
    printf("**********************************************************************************\n");


    char s[] = {'H', 'e', '\0'};
    char arr[] = {'H', 'e', 'l', 'l', 'o', '\0'};
    char arr2[] = {'c', 'o', 'o', 'k', '\0'};

    char s2[] = {'o', 'c', '\0'};
    char s3[] = {'o', 'o', '\0'};
    char s4[] = {'o', 'k', '\0'};
    char s5[] = {'c', 'o', '\0'};
    char s6[] = {'l', 'o', '\0'};
    
    printf("----------------------------------------------------------------------------------\n");
    state = arm_state_new(1024, (unsigned int *) find_str_a, (unsigned int) arr, (unsigned int) s, 0, 0);
    printf("Look for He in Hello\n");
    result = arm_state_execute(state);
    arm_state_free(state);
    printf("[N]: %d\n", find_str_a(arr, s));
    printf("[E]: %d\n", result);
    printf("\n");
    printf("----------------------------------------------------------------------------------\n");

    printf("----------------------------------------------------------------------------------\n");
    state = arm_state_new(1024, (unsigned int *) find_str_a, (unsigned int) arr2, (unsigned int) s2, 0, 0);
    printf("Look for oc in cook\n");
    result = arm_state_execute(state);
    arm_state_free(state);
    printf("[N]: %d\n", find_str_a(arr2, s2));
    printf("[E]: %d\n", result);
    printf("\n");
    printf("----------------------------------------------------------------------------------\n");

    printf("----------------------------------------------------------------------------------\n");
    state = arm_state_new(1024, (unsigned int *) find_str_a, (unsigned int) arr2, (unsigned int) s3, 0, 0);
    printf("Look for oo in cook\n");
    result = arm_state_execute(state);
    arm_state_free(state);
    printf("[N]: %d\n", find_str_a(arr2, s3));
    printf("[E]: %d\n", result);
    printf("\n");
    printf("----------------------------------------------------------------------------------\n");

    printf("----------------------------------------------------------------------------------\n");
    state = arm_state_new(1024, (unsigned int *) find_str_a, (unsigned int) arr2, (unsigned int) s4, 0, 0);
    printf("Looking for ok in cook\n");
    result = arm_state_execute(state);
    arm_state_free(state);
    printf("[N]: %d\n", find_str_a(arr2, s4));
    printf("[E]: %d\n", result);
    printf("\n");
    printf("----------------------------------------------------------------------------------\n");

    printf("----------------------------------------------------------------------------------\n");
    state = arm_state_new(1024, (unsigned int *) find_str_a, (unsigned int) arr, (unsigned int) s6, 0, 0);
    printf("Looking for lo in hello\n");
    result = arm_state_execute(state);
    arm_state_free(state);
    printf("[N]: %d\n", find_str_a(arr, s6));
    printf("[E]: %d\n", result);
    printf("\n");
    printf("----------------------------------------------------------------------------------\n");

    printf("**********************************************************************************\n");
}

void test_functions(){
    sum_array_test();
    printf("\n");

    find_max_test();
    printf("\n");

    fib_iter_test();
    printf("\n");

    fib_rec_test();
    printf("\n");

    find_str_test();
    printf("\n");
}


int main(int argc, char **argv){
    test_functions();
}

