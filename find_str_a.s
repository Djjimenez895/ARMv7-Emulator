.global find_str_a
.func find_str_a

/* r0 stores the char array */
/* r1 stores the substring array */
/* i will be r4 */
/* j will be r5 */
/* sub_len will be r6 */
/* s_len will be r7 */
/* sub_first_index will be r8 */

find_str_a:
    /* Allocating memory on stack for preserved registers */
    /* 6 registers are put on the stack (24 bytes) */
    /* multiples of 8 convention */

    /* r0 is put on the stack since it holds the */
    /* originial char array and a function needs to be called */
    sub sp, sp, #40
    str r0, [sp]
    str r1, [sp, #4]
    str r4, [sp, #8]
    str r5, [sp, #12]
    str r6, [sp, #16]
    str r7, [sp, #20]
    str r8, [sp, #24]
    str r9, [sp, #28]
    str r10, [sp, #32]
    
    mov r4, #0 /* i */
    mov r5, #0 /* j */

/*loop for finding the string length */
str_length_loop:
    /* r4 will be used to find length */
    /* r2 will load in each byte */
    ldrb r2, [r0]
    cmp r2, #0
    beq find_length_sub /*if equal, it's the end of the string */
    add r4, r4, #1 /* increments length by 1 */
    add r0, r0, #1 /* moves over a byte in the string */
    b str_length_loop

/*loop for finding substring length */
sub_length_loop:
    ldrb r2, [r0]
    cmp r2, #0
    beq find_str_continue
    add r4, r4, #1 /*increments length by 1 */
    add r0, r0, #1 /*moves over a byte in the string */
    b sub_length_loop

/* Label used for cleaning up some registers before finding substring length */
find_length_sub:
    mov r7, r4 /*puts the length in r7 (s_len) */
    mov r4, #0 /*reset counter*/
    mov r0, r1 /*moves substring into r0 */
    b sub_length_loop

find_str_continue:
    /* mov r0, r7 and then returning seems to work correctly*/
    mov r6, r4 /*moving length of substring into r6 (sub_len) */
    ldr r0, [sp] /*load in the original r0 into r0*/
    ldr r1, [sp, #4] /* load in the original r1 into r1*/
    ldrb r9, [r0] /* loads in first byte of r0 */
    ldrb r10, [r1] /* loads in first byte of r1*/
    mov r4, #0 /* resetting i */
    mov r5, #0 /* resetting j */
    mvn r8, #0 /* sub_first_index*/
    //b find_str_loop
    
find_str_loop:

    /* Stopping conditions of the for loop */
    cmp r4, r7 /* check if i < s_len */
    bge done 
    cmp r5, r6 /* checks if j < sub_len */
    bge done
    
    /*Keep an eye on these four lines */
    cmp r9, r10 /* s[i] == sub[j] comparison */
    beq equal_chars /* branches if the values are equal; the if condition */
    cmpne r5, #0 /*else if j != 0 */
    bne unequal_chars

    /*if neither condition is met */
    add r4, r4, #1 /*i++ */
    add r0, r0, #1 /* shift over one byte in r0 */
    ldrb r9, [r0] /* loads in the next byte in r0 */
    b find_str_loop

/*Since I know this is the condition that is met, the loop continues */
/*when it branches back to find_str_loop */    
equal_chars:
    cmp r5, #0 /*if j == 0 condition */
    moveq r8, r4 /* sub_first_index = i  */
    add r5, r5, #1 /* j++ */
    add r1, r1, #1 /* shift over one byte in r1*/
    ldrb r10, [r1] /* loading in next element in substring */
    /*because j is being incremented by 1 */

    /* increments i before going to the start of the loop again */
    add r4, r4, #1 /* i++ */
    add r0, r0, #1 /* shift over one byte in r0*/
    ldrb r9, [r0] /* loads in the next byte in r0 */
    b find_str_loop

unequal_chars:
    ldr r1, [sp, #4] /* resets to the beginning of the substring */
    /*so that r1 points at the first address in the substring */
    ldrb r10, [r1]
    mov r5, #0 /*j = 0, resets j */
    mvn r8, #0 /* sub_first_index = -1 */
    cmp r9, r10 /*if s[i] == sub[j] */
    moveq r8, r4 /*sub_first_index = i */
    addeq r5, r5, #1 /* j++ */
    addeq r1, r1, #1 /* move over one byte*/
    ldrb r10, [r1] /* load in the byte */

    /* increments i before going to the start of the loop again */
    add r4, r4, #1 /* i++ */
    add r0, r0, #1 /* shifting one byte over*/
    ldrb r9, [r0] /* moves over one address in r0 */
    b find_str_loop
            
done:
    cmp r5, r6 /*this comparison is used to check the case where */
    /*the string has ended and the substring wasn't found, but */
    /*r8 (sub_first_index) has not been reset*/
    mvnne r8, #0
    mov r0, r8 /* r0 = sub_first_index so that it will be returned */
    
    ldr r1, [sp, #4] 
    ldr r4, [sp, #8]
    ldr r5, [sp, #12]
    ldr r6, [sp, #16]
    ldr r7, [sp, #20]
    ldr r8, [sp, #24]
    ldr r9, [sp, #28]
    ldr r10, [sp, #32]
        
    add sp, sp, #40
    bx lr

