.global test_a
.func test_a

test_a:
    //Add and subtract test
    //add r0, r1, #1
    //sub r0, r0, #1
    //cmp r0, #1
    //moveq r0, #12
    //cmp r0, r1
    //movne r0, #11
    //add r0, r0, #1

    //Sub test
    //sub r0, r0, #1
    //sub r0, r0, #1
    //mov r0, r1
    //cmp r0, r1
    //addeq r0, r0, r0
    //sub sp, sp, #8
    //str r0, [sp]
    mov r0, #1
    mov r1, #5
    cmp r0, r1
    movge r0, #23
    
    //Mov and cmp test
    //mov r4, #12
    //mov r0, r4
    //cmp r0, r4
    //addge r0, r0, #1
    //str r4, [sp]
    //mov r4, #1
    //mov r5, #3
    //str r4, [sp]
    //str r5, [sp, #4]
    //mov r1, #16
    //ldr r6, [r0, r4, lsl #2]
    //mov r0, r6
    //bl test
    //b done
    
    //Arr test

    //str r1, [r0]
    //mov r1, #5
    //ldr r1, [r0]
    //mov r0, #12
    //ldr r0, [sp]
    //mov r0, #40
    //str r1, [r0]
    //mov r3, #12
    //ldr r3, [r0]
    //add r0, r0, #1
    //ldrb r1, [r0]
    //add r0, r0, #4
    //ldrb r1, [r0]
    //add r0, r0, #1
    //ldrb r1, [r0]
    //mov r0, r1

    //Branch test
    //b test
    //add r1, r1, #4

//Test Function to branch to
//test:
    //mov r0, #12
    //addeq r1, r1, #1
    //mov r0, r1
//done:
    bx lr
