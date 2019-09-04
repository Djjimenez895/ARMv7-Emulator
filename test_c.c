#include<stdlib.h>
#include<stdio.h>

void print_binary_value(unsigned int val){
    int i;
    int b;

    printf("0b");

    for(i = 31; i >= 0; i--){
        b = (val >> i) &0b1;
        printf("%d", b);
    }
    
    printf("\n");
    return;
}

unsigned int set_bit_test(unsigned int val){
    val |= 1 << 31;
    return val;
}

unsigned int clear_bit_test(unsigned int val){
    val &= ~(1 << 31);
    return val;
}

int main(int argc, char **argv){

    unsigned int v = 1;
    unsigned int be;
    char arr2[] = {'t', 'e', 's', 't'};

    unsigned int test;
    unsigned int test2;

    test = (unsigned int) arr2;
    print_binary_value(test);

    printf("%d\n", arr2);
    printf("%d\n", *arr2);
    test = 0;
    printf("%d\n", test);
    printf("%d\n", &test);

    /*
    print_binary_value(v);
    v = set_bit_test(v);
    print_binary_value(v);
    v = clear_bit_test(v);
    print_binary_value(v);
    */

    //print_binary_value(v);    
    //be = v &0b11111111;
    //print_binary_value(be);
    //be = be << 23;
    //print_binary_value(be);
    
}
