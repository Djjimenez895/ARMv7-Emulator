.global find_max_a
.func find_max_a

/*r0 will store *array, r1 will store the length */
/*r4 will store i */
/* Max value will be stored in r5 */

find_max_a:
    sub sp, sp, #16
    str r4, [sp]
    str r5, [sp, #4]
    str r6, [sp, #8]
    str r7, [sp, #12]

    mov r4, #0 /* i */
    ldr r5, [r0] /* max */

loop:
    cmp r4, r1
    beq done
    ldr r6, [r0, r4, lsl #2]
    cmp r6, r5
    movgt r5, r6
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
