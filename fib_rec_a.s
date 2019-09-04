.global fib_rec_a
.func fib_rec_a

/*r0 = nth number */
/* */

fib_rec_a:
    sub sp, sp, #8
    str lr, [sp, #4]
    str r0, [sp]

    /*Checks for the cases of 0 and 1 */
    cmp r0, #0
    moveq r0, #0
    beq fib_ret
    cmp r0, #1
    moveq r0, #1
    beq fib_ret
    b fib_else
    
fib_else:
    sub sp, sp, #16
    str r0, [sp] /*sp is already minus 8 */
    str r4, [sp, #4]
    str r5, [sp, #8]
    sub r0, r0, #1 /*n = n - 1 */
    bl fib_rec_a
    mov r4, r0 /*fib_rec_a(n-1) value returned through r0 and stored */
    ldr r0, [sp]
    sub r0, r0, #2 /*n = n - 2, popped off stack */
    bl fib_rec_a
    mov r5, r0 /*fib_rec_a(n-2)*/
    add r0, r5, r4

    ldr r4, [sp, #4]
    ldr r5, [sp, #8]
    add sp, sp, #16

fib_ret:

    ldr lr, [sp, #4]
    add sp, sp, #8
    bx lr
       
    
