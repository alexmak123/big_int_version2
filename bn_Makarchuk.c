#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <malloc.h>
#include <string.h>
#include <locale.h>

// enough to fit in 100000 symbols
const int MAX_SIZE = 12000;
// numeric system
const int MOD = 1000000000;
// max amount of digits in one cell of our massive body
const int NUM_DIGITS = 9;

struct bn_s {
    // our body
    int *body;
    // number of cells that create number, without leading zeros; if bigint is zero, then it would be zero
    int size;
    // 0 if number is positive, 1 if negative
    int is_neg;
};

typedef struct bn_s bn;

int bn_delete(bn *t) {
    t -> size = 0;
    t -> is_neg = 0;
    free(t -> body);
    t -> body = NULL;
    free(t);
    return 0;
}

//remove zeros from the end
void resize_t_excluding_zeros (bn *t) {
    while (t -> body[t -> size - 1] == 0) {
        t -> size -= 1;
    }
}

// sets bigint value to zero
void set_zero(bn *t) {
    t -> size = 0;
    t -> is_neg = 0;
    for (int i = 0; i < MAX_SIZE; i++) {
        t -> body[i] = 0;
    }
}

// convert from base MOD to base 10
// both representations are in reverse order
int *convert_to_base_10(const bn *t) {
    // allocate enough memory to fit digits in new base
    int *base_10 = calloc((t -> size) * NUM_DIGITS, sizeof(int));

    int offset = 0;
    for (int i = 0; i < t -> size; i++) {
        int digit = t -> body[i];
        for (int j = 0; j < NUM_DIGITS; j++) {
            base_10[offset] = digit % 10;
            digit /= 10;
            offset++;
        }
    }
    return base_10;
}

// returns a string representation of bn
char *my_bn_to_string(const bn *t) {
    // allocate enough to fit number and maybe the minus sign
    char *res = calloc(((t -> size) * NUM_DIGITS + 1), sizeof(char));

    if (t -> size == 0) {
        return "0";
    }

    int i = 0;
    if (t -> is_neg) {
        res[0] = '-';
        i = 1;
    }

    int *base_10 = convert_to_base_10(t);

    // remove leading zeros
    int j = (t -> size) * NUM_DIGITS - 1;
    while (j >= 0 && base_10[j] == 0) {
        j--;
    }

    // reverse
    while (j >= 0) {
        res[i] = base_10[j] + '0';
        i++;
        j--;
    }

    free(base_10);
    return res;
}

// create a new bn that represents zero
bn *bn_new() {
    bn *t = malloc(sizeof(bn));
    if (t == NULL) {
        return NULL;
    }
    t -> is_neg = 0;
    t -> body = malloc(sizeof(int) * MAX_SIZE);
    if (t -> body == NULL) {
        free(t);
        return NULL;
    }
    set_zero(t);
    return t;
}

// create existing bn with allocating memory
bn *bn_init(bn const *orig) {
    bn *t = malloc(sizeof(bn));
    if (t == NULL) {
        return NULL;
    }
    t -> body = malloc(sizeof(int) * MAX_SIZE);
        if (t -> body == NULL) {
        free(t);
        return NULL;
    }
    set_zero(t);
    t -> is_neg = orig -> is_neg;
    t -> size = orig -> size;
    for (int i = 0; i < orig -> size; i++) {
        t -> body[i] = orig -> body[i];
    }
    return t;
}

// initialize the value of bn with a given integer
// assumes pointer is valid
int bn_init_int(bn *t, int init_int) {
    set_zero(t);
    t -> is_neg = 0;
    if (init_int < 0) {
        t -> is_neg = 1;
        init_int *= -1;
    }
    int i = 0;
    while (init_int != 0) {
        t -> body[i] = init_int % MOD;
        init_int /= MOD;
        i++;
    }
    t -> size = i;
    return 0;
}

/*// Initialize the value of BN with the decimal representation of the string
int bn_init_string(bn *t, const char *init_string) {
    set_zero(t);
    int p = strlen(init_string), i = 0;
    if (init_string[0] == '-') {
        t -> is_neg = 1;
        for (int j = 0; j < p; j++) {
            if (p - i - 1 == 0) {
                break;
            }
            t -> body[j] = init_string[p - i - 1] - '0';
            i++;
        }
    }
}*/

// operation x += y by absolute value, ignores the sign
// assumes that pointers are valid
int abs_bn_add_to(bn *t, bn const *right) {
    int additional_one = 0;
    int i = 0;
    while (i < t -> size || i < right -> size) {
        int digit = t -> body[i] + right -> body[i];
        if (additional_one) {
            digit++;
        }
        if (digit >= MOD) {
            additional_one = 1;
            digit -= MOD;
        } else {
            additional_one = 0;
        }
        t -> body[i] = digit;
        i++;
    }
    t -> size = i;
    if (additional_one) {
        t -> body[i] = 1;
        t -> size++;
    }
    return 0;
}

// operation x -= y by absolute value, ignores the sign, |x| >= |y|
// assumes that pointers are valid
int abs_bn_sub_to(bn *t, bn const *right) {
    int minus_one = 0;
    int i = 0;
    while (i < t -> size || i < right -> size) {
        int digit = t -> body[i] - right -> body[i];
        if (minus_one) {
            digit--;
        }
        if (digit < 0) {
            minus_one = 1;
            digit += MOD;

        } else {
            minus_one = 0;
        }
        t -> body[i] = digit;
        i++;
    }
    //resize excluding zeros
    resize_t_excluding_zeros(t);
    return 0;
}

// Comp by abs, if the left is less, return < 0; if equal, return 0; otherwise > 0
int abs_bn_cmp (bn const *left, bn const *right) {
    for (int i = MAX_SIZE - 1; i >= 0; i--) {
        if (left -> body[i] > right -> body[i]) {
            return 1;
        }
        else if (left -> body[i] < right -> body[i]) {
            return -1;
        }
    }
    return 0;
}

// If the left is less, return < 0; if equal, return 0; otherwise > 0
int bn_cmp(bn const *left, bn const *right) {
    if (left -> is_neg == 0 && right -> is_neg == 1) {
        return -1;
    }
    else if (left -> is_neg == 1 && right -> is_neg == 0) {
        return 1;
    }
    else if (left -> is_neg == 0 && right -> is_neg == 0) {
        return abs_bn_cmp(right, left);
    }
    else {
        return abs_bn_cmp(left, right);
    }
}

// operation +=
int bn_add_to(bn *t, bn const *right) {
    int comp = abs_bn_cmp(t, right);
    if (t -> is_neg == right -> is_neg) {
        abs_bn_add_to(t, right);
        return 0;
    }
    // different signs
    if (comp == 0) {
        set_zero(t);
    }
    if (comp == 1) {
        abs_bn_sub_to(t, right);
    }
    else {
        bn *temp = bn_init(right);
        abs_bn_sub_to(temp, t);
        //bn_delete(t);
        t = temp;
    }
    return 0;
}

// operation -=
int bn_sub_to(bn *t, bn const *right) {
    bn *temp = bn_init(right);
    temp -> is_neg = 1 - right -> is_neg;
    bn_add_to(t, temp);
    bn_delete(temp);
    return 0;
}

// operations x = l+r
bn* bn_add(bn const *left, bn const *right) {
    bn *temp = bn_init(left);
    bn_add_to(temp, right);
    return temp;
}

// operations x = l-r
bn* bn_sub(bn const *left, bn const *right) {
    bn *temp = bn_init(left);
    bn_sub_to(temp, right);
    return temp;
}

/*int main()
{
    int n;
    scanf("%d", &n);
    bn *a = bn_new(), *b = bn_new();
    bn_init_int(a, 1000);
    bn_init_int(b, 1);
    abs_bn_sub_to(a, b);
    char *res = bn_to_string(a);
    //we have a problem during valgrind compilation because it thinks that we can have res == NULL when we printf it
    printf("%s\n", res);
    free(res);
    bn_delete(a);
    bn_delete(b);
    return 0;
}*/


