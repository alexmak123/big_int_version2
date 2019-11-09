#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <malloc.h>
#include <locale.h>

int max_size = 20000;

/*1) representation: an array of digits in decimal notation, written in reverse order and separately sign in the structure*/
struct bn_s {
    int *body; //body
    int sign; // sing 0 is -, 1 is +
};

typedef struct bn_s bn;

//print our structure, can be printed with leading zeros if we do not delete them
void print_bn (const bn *r, int max_size) {
    if (r -> sign == 0) {
        printf("-");
    }
    int j = max_size - 1;
    while (r -> body[j] == 0) {
        j--;
    }
    for (int i = j; i >= 0; i--) {
        printf("%d", r -> body[i]);
    }
}

//init body with zeros
void init_with_zeroes(bn *r, int size) {
    for (int i = 0; i < size; i++) {
        r -> body[i] = 0;
    }
}

//Create a new bn such that bn * a = bn_new () will give massive of zeros
bn *bn_new() {
    //allocate memory for one element of type bn
    bn *r = malloc (sizeof (bn));
    //if we do not have a working pointer
    if (r == NULL) {
        return NULL;
    }
    r -> sign = 1;
    r -> body = malloc(sizeof (int) * max_size);
    //same
    if (r -> body == NULL) {
        free(r);
        return NULL;
    }
    init_with_zeroes(r, max_size);
    return r;
}

//Initialize the value of bn with a given integer and we do not need to create it
int bn_init_int(bn *t, int init_int) {
    //if we do not have a working pointer
    if (t == NULL) {
        return 1;
    }
    int temp = init_int;
    t -> sign = 1;
    if (init_int < 0) {
        t -> sign = 0;
        temp *= -1;
    }
    //we know that memory is not allocated, that is why we need to allocate it
    t -> body = malloc(sizeof (int) * max_size);
    //pointer is not working
    if (t -> body == NULL) {
        return 1;
    }
    init_with_zeroes(t, max_size);
    int i = 0;
    while (temp != 0) {
        t -> body[i] = temp % 10;
        temp /= 10;
        i++;
    }
    return 0;
}

// The operation x += y modulo, we assume that |x| >= |y| and we know that the pointers are normal
int abs_bn_add_to(bn *t, bn const *right) {
    int additional_one = 0;
    for (int i = 0; i < max_size; i++) {
        int digit = t -> body[i] + right -> body[i];
        if (additional_one) {
            digit++;
        }
        if (digit >= 10) {
            additional_one = 1;
            digit -= 10;
        }
        else {
            additional_one = 0;
        }
        t -> body[i] = digit;
    }
    return 0;
}

int main()
{
    int n;
    scanf("%d", &n);
    bn *a = bn_new(), *b = bn_new();
    bn_init_int(a, 1);
    bn_init_int(b, 1);
    for (int i = 2; i < n; i++) {
        abs_bn_add_to(a, b);
        bn *temp = a;
        a = b;
        b = temp;
    }
    print_bn(b, 20000);
    return 0;
}
