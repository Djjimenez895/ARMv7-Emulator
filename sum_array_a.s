.global sum_array_a
.func sum_array_a

/* int *arr is in r0 */
/* int len is in r1 */
/* The counter (i) is in r4 */
/* sum is in r5 */

sum_array_a:
    sub sp, sp, #16
    str r4, [sp]
    str r5, [sp, #4]
    str r6, [sp, #8]
    str r7, [sp, #12]
    mov r4, #0
    mov r5, #0

loop:
    cmp r4, r1 /* Compares incrementer to length */
    bge done
    ldr r6, [r0, r4, lsl #2] /* Loading in array element */
    add r5, r5, r6 /*sum = sum + array[i] */
    add r4, r4, #1 
    b loop

done:
    mov r0, r5

    ldr r4, [sp]
    ldr r5, [sp, #4]
    ldr r6, [sp, #8]
    ldr r7, [sp, #12]
    add sp, sp, #16

    bx lr   
 
