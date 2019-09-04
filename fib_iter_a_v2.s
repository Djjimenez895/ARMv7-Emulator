.global fib_iter_a_v2
.func fib_iter_a_v2

/*r0 = nth_val, can function as length too?? */

fib_iter_a_v2:

    /* Stack space needed for registers */
    /* Additional stack space for the three values that'll be on the stack */
    sub sp, sp, #24
    str r4, [sp]
    str r5, [sp, #4]
    str r6, [sp, #8]
    str r7, [sp, #12]
    str r8, [sp, #16]
    str r9, [sp, #20]

    mov r4, #0 /* i */
    mov r5, #0 /* first_val */
    mov r6, #1 /* second_val */
    /*r7 will be next_val */

loop:
    cmp r4, r0 /*r0 can be used to check length*/
    bgt done
    cmp r4, #1
    movle r7, r4
    addgt r7, r5, r6
    movgt r5, r6
    movgt r6, r7
    add r4, r4, #1
    b loop 
    
done:

    mov r0, r7 /* Moving next val into r0 */

    ldr r4, [sp]
    ldr r5, [sp, #4]
    ldr r6, [sp, #8]
    ldr r7, [sp, #12]
    ldr r8, [sp, #16]
    ldr r9, [sp, #20]
    add sp, sp, #24

    bx lr
    

