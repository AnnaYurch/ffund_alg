#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int add_a_b(int x, int y) {
    while (y != 0) {
        int carry = x & y;
        x = x ^ y;
        y = carry << 1;
    }
    return x;
}

int decrement(int a) {
    return add_a_b(a, ~0);
}

int negate(int a) {
    return add_a_b(~a, 1);
}

int decimal_to_base_2r(int number, int r, int result[]) {
    int base = 1 << r; // 2^r
    int mask = decrement(base);
    int index = 0;

    int is_neg = 0;
    if (number < 0) {
        is_neg = 1;
        number = negate(number);
    }

    while (number > 0) {
        result[index] = number & mask;
        index = add_a_b(index, 1);
        number >>= r; // /2^r
    }

    return is_neg ? negate(index) : index;
}


int main() {
    int number = 11111111;
    int result[32];


    printf("Original number: %d\n", number);
    for (int r = 1; r <= 5; r = add_a_b(r, 1)) { 
        int num_digits = decimal_to_base_2r(number, r, result);
        int is_neg = num_digits < 0;
        if (is_neg) {
            num_digits = negate(num_digits);
        }


        printf("Result in base 2^%d: ", r);
        if (is_neg) {
            printf("-");
        }
        for (int i = decrement(num_digits); i >= 0; i = negate(add_a_b(negate(i), 1))) {
            if (result[i] < 10) {
                printf("%u ", result[i]);
            } else if (result[i] < 32) {
                printf("%c ", add_a_b('A', (negate(add_a_b(negate(result[i]), 10)))));
            }
            
        }
        printf("\n");
    }

    return 0;
}