#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <malloc.h>
#include <string.h>
#include <locale.h>

struct bn_s {
    // our body
    int *body;
    // number of cells that create number, without leading zeros; if bigint is zero, then it would be zero
    int size;
    // 0 if number is positive, 1 if negative
    int is_neg;
};

typedef struct bn_s bn;
typedef unsigned long long int my_ulong;

// enough to fit in 100000 symbols
const int MAX_SIZE = 12000;
// numeric system
const int MOD = 100000000;
// max amount of digits in one cell of our massive body
const int NUM_DIGITS = 8;

int max(int a, int b) {
    if (a > b) {
        return a;
    }
    return b;
}

int bn_delete(bn *t) {
    t -> size = 0;
    t -> is_neg = 0;
    free(t -> body);
    t -> body = NULL;
    free(t);
    return 0;
}

// recalculates size of t by skipping leading zeros
void recalculate_bn_size(bn *t) {
    int i = MAX_SIZE - 1;
    while (i >= 0 && t -> body[i] == 0) {
        i--;
    }
    t -> size = i + 1;
}

// sets bigint value to zero
void set_zero(bn *t) {
    t -> size = 0;
    t -> is_neg = 0;
    for (int i = 0; i < MAX_SIZE; i++) {
        t -> body[i] = 0;
    }
}

// NOT WORKING CORRECTLY!!!!
// convert from base MOD to base 10
// both representations are in reverse order
int *convert_to_base_radix(const bn *t, int radix) {
    // allocate enough memory to fit digits in new base
    int *base_radix = calloc((t -> size) * NUM_DIGITS * 10, sizeof(int));

    int offset = 0;
    for (int i = 0; i < t -> size; i++) {
        int digit = t -> body[i];
        for (int j = 0; j < radix; j++) {
            base_radix[offset] = digit % radix;
            digit /= radix;
            offset++;
        }
    }
    return base_radix;
}
// NOT WORKING CORRECTLY!!!!
// Return the representation of BN in the radix number system as a string
// The line after use needs to be deleted.
char *bn_to_string(bn const *t, int radix) {
    if (t -> size == 0) {
        return "0";
    }

    // allocate enough to fit number and maybe the minus sign
    char *res = calloc(((t -> size) * NUM_DIGITS * 10 + 1), sizeof(char));

    int i = 0;
    if (t -> is_neg) {
        res[0] = '-';
        i = 1;
    }

    int *base_radix = convert_to_base_radix(t, radix);

    // remove leading zeros
    int j = (t -> size) * NUM_DIGITS * 10 - 1;
    while (j >= 0 && base_radix[j] == 0) {
        j--;
    }

    // reverse
    while (j >= 0) {
        res[i] = base_radix[j] + '0';
        i++;
        j--;
    }

    free(base_radix);
    return res;

}

// create a new bn that equals zero
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

// copies content from orig into t
// assumes that both bn have memory allocated
void bn_copy(bn *t, bn const *orig) {
    t -> is_neg = orig -> is_neg;
    t -> size = orig -> size;
    for (int i = 0; i < MAX_SIZE; i++) {
        t -> body[i] = orig -> body[i];
    }
}

// return a copy of bn orig
bn *bn_init(bn const *orig) {
    bn *t = bn_new();
    bn_copy(t, orig);
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
        }
        else {
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
    while (i < t -> size) {
        int digit = t -> body[i] - right -> body[i];
        if (minus_one) {
            digit--;
        }
        if (digit < 0) {
            minus_one = 1;
            digit += MOD;

        }
        else {
            minus_one = 0;
        }
        t -> body[i] = digit;
        i++;
    }
    //resize excluding zeros
    recalculate_bn_size(t);
    return 0;
}

// comp by abs, if the left is less, return < 0; if equal, return 0; otherwise > 0
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

// if the left is less, return < 0; if equal, return 0; otherwise > 0
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

// operation t += right
int bn_add_to(bn *t, bn const *right) {
    int comp = abs_bn_cmp(t, right);
    if (t -> is_neg == right -> is_neg) {
        abs_bn_add_to(t, right);
        return 0;
    }
    // different signs
    if (comp == 0) {
        set_zero(t);
        return 0;
    }
    if (comp == 1) {
        abs_bn_sub_to(t, right);
    }
    else {
        bn *temp = bn_init(right);
        abs_bn_sub_to(temp, t);
        bn_copy(t, temp);
        bn_delete(temp);
    }
    return 0;
}

// operation t -= right
int bn_sub_to(bn *t, bn const *right) {
    t -> is_neg = 1 - t -> is_neg;
    bn_add_to(t, right);
    t -> is_neg = 1 - t -> is_neg;
    return 0;
}

// operation x = left + right
bn* bn_add(bn const *left, bn const *right) {
    bn *temp = bn_init(left);
    bn_add_to(temp, right);
    return temp;
}

// operations x = left - right
bn* bn_sub(bn const *left, bn const *right) {
    bn *temp = bn_init(left);
    bn_sub_to(temp, right);
    return temp;
}

// initialize the value of BN with the decimal representation of the string
int bn_init_string(bn *t, const char *init_string) {
    set_zero(t);
    int p = strlen(init_string);
    int string_start = 0;
    if (init_string[0] == '-') {
        t -> is_neg = 1;
        p -= 1;
        // actual number starts from 1
        string_start = 1;
    }
    else {
        t -> is_neg = 0;
    }

    t -> size = p / NUM_DIGITS;
    if (p % NUM_DIGITS != 0) {
        t -> size += 1;
    }

    // string index - up to NUM_DIGITS before it belong to cell
    // can be less then NUM_DIGITS if it is last cell
    int string_i = strlen(init_string);
    int cell_i = 0;
    while (string_i >= string_start) {
        int cell = 0;
        for (int i = string_i - NUM_DIGITS; i < string_i; i++) {
            if (i < string_start) {
                // last cell that has less then NUM_DIGITS, skip some of them
                continue;
            }
            cell *= 10;
            cell += init_string[i] - '0';
        }
        string_i -= NUM_DIGITS;

        t -> body[cell_i] = cell;
        cell_i++;
    }

    return 0;
}

// operation x = |left| * |right|
bn *abs_bn_mul(bn const *left, bn const *right) {
    int length = left -> size + right -> size + 1;

    my_ulong *mult_res = calloc(length, sizeof(my_ulong));

    for (int i = 0; i < left -> size; i++) {
        for (int j = 0; j < right -> size; j++) {
            mult_res[i + j] += (my_ulong) left -> body[i] * (my_ulong) right -> body[j];
        }
    }
    for (int i = 0; i < length - 1; i++) {
        mult_res[i + 1] += mult_res[i] / MOD;
        mult_res[i] %= MOD;
    }

    bn *res = bn_new();
    for (int i = 0; i < length; i++) {
        res -> body[i] = mult_res[i];
    }
    free(mult_res);
    recalculate_bn_size(res);
    return res;
}

// operation x = left * right
bn* bn_mul(bn const *left, bn const *right) {
    bn *temp = abs_bn_mul(left, right);
    if (left -> is_neg == right -> is_neg) {
        temp -> is_neg = 0;
    }
    else {
        temp -> is_neg = 1;
    }
    return temp;
}

// operation t *= right
int bn_mul_to(bn *t, bn const *right) {
    bn *temp = bn_mul(t, right);
    bn_copy(t, temp);
    bn_delete(temp);
    return 0;
}

// operation bn *t /= 2
void divide_by_2(bn *t) {
    for (int i = t -> size - 1; i >= 0; i--) {
        if (i) {
            t -> body[i - 1] += (t -> body[i] % 2) * MOD;
        }
        t -> body[i] /= 2;
    }
    recalculate_bn_size(t);
}

// operation to find square root
// t has to be non negative
bn *square_root(bn const *t) {
    bn *lower = bn_new();
    bn *upper = bn_init(t);

    bn *one = bn_new();
    bn_init_int(one, 1);

    bn *ans = NULL;

    while (ans == NULL) {
        bn *mid = bn_add(lower, upper);
        divide_by_2(mid);
        bn *mid_plus_one = bn_add(mid, one);

        bn* square_mid = bn_mul(mid, mid);
        bn* square_mid_plus_one = bn_mul(mid_plus_one, mid_plus_one);

        int comp_mid = abs_bn_cmp(square_mid, t);
        int comp_mid_plus_one = abs_bn_cmp(square_mid_plus_one, t);

        if (comp_mid_plus_one < 0) {
            bn_delete(lower);
            lower = bn_init(mid_plus_one);
        }
        else if (comp_mid > 0) {
            bn_delete(upper);
            upper = bn_init(mid);
        }
        else if (comp_mid_plus_one == 0) {
            ans = bn_init(mid_plus_one);
        }
        else {
            ans = bn_init(mid);
        }

        // common clean up
        bn_delete(mid);
        bn_delete(mid_plus_one);
        bn_delete(square_mid);
        bn_delete(square_mid_plus_one);
    }

    bn_delete(lower);
    bn_delete(upper);
    bn_delete(one);

    return ans;
}

// operation x = l / r by modulo
bn* abs_bn_div(bn const *left, bn const *right) {
    bn *lower = bn_new();
    bn *upper = bn_init(left);

    bn *one = bn_new();
    bn_init_int(one, 1);

    bn *ans = NULL;

    while (ans == NULL) {
        bn *mid = bn_add(lower, upper);
        divide_by_2(mid);
        bn *mid_plus_one = bn_add(mid, one);

        bn* mult_mid_right = bn_mul(mid, right);
        bn* mult_mid_plus_one_right = bn_mul(mid_plus_one, right);

        int comp_mid = abs_bn_cmp(mult_mid_right, left);
        int comp_mid_plus_one = abs_bn_cmp(mult_mid_plus_one_right , left);

        if (comp_mid_plus_one < 0) {
            bn_delete(lower);
            lower = bn_init(mid_plus_one);
        }
        else if (comp_mid > 0) {
            bn_delete(upper);
            upper = bn_init(mid);
        }
        else if (comp_mid_plus_one == 0) {
            ans = bn_init(mid_plus_one);
        }
        else {
            ans = bn_init(mid);
        }

        // common clean up
        bn_delete(mid);
        bn_delete(mid_plus_one);
        bn_delete(mult_mid_right );
        bn_delete(mult_mid_plus_one_right);
    }

    bn_delete(lower);
    bn_delete(upper);
    bn_delete(one);

    return ans;
}

// operation x = l / r
bn* bn_div(bn const *left, bn const *right) {
    bn *res = NULL;
    if (left -> is_neg == 0 && right -> is_neg == 0) {
        res = abs_bn_div(left, right);
    }
    else if (left -> is_neg == 1 && right -> is_neg == 1) {
        bn *temp1 = bn_init(left);
        bn *temp2 = bn_init(right);
        temp1 -> is_neg = 0;
        temp2 -> is_neg = 0;
        res = abs_bn_div(temp1, temp2);
        bn_delete(temp1);
        bn_delete(temp2);
    }
    else if (left -> is_neg == 0 && right -> is_neg == 1){
        res = abs_bn_div(left, right);
        bn *one = bn_new();
        bn_init_int(one, 1);
        bn_add_to(res, one);
        res -> is_neg = 1;
    }
    else {
        bn *temp1 = bn_init(left);
        bn *temp2 = bn_init(right);
        temp1 -> is_neg = 0;
        temp2 -> is_neg = 0;
        res = abs_bn_div(temp1, temp2);
        bn_delete(temp1);
        bn_delete(temp2);
        bn *one = bn_new();
        bn_init_int(one, 1);
        bn_add_to(res, one);
        res -> is_neg = 1;
        bn_delete(one);
    }
    return res;
}


//operation x = l%r
bn* bn_mod(bn const *left, bn const *right) {
    bn *div = bn_div(left, right);
    bn_mul_to(div, right);
    bn *res = bn_sub(left, div);
    bn_delete(div);
    return res;
}

// Change the sign to the opposite
int bn_neg(bn *t) {
    t -> is_neg = 1 - t -> is_neg;
    return 0;
}

// Take module
int bn_abs(bn *t) {
    t -> is_neg = 0;
    return 0;
}

//-1 if t <0; 0 if t = 0, 1 if t> 0
int bn_sign(bn const *t) {
    if (t -> size == 0) {
        return 0;
    }
    if (t -> is_neg == 0) {
        return 1;
    }
    if (t -> is_neg == 1) {
        return -1;
    }
    return 0;
}

// t /= right
int bn_div_to(bn *t, bn const *right) {
    bn *temp = bn_div(t, right);
    bn_copy(t, temp);
    bn_delete(temp);
    return 0;
}

// t %= right
int bn_mod_to(bn *t, bn const *right) {
    bn *temp = bn_mod(t, right);
    bn_copy(t, temp);
    bn_delete(temp);
    return 0;
}

// Raise the number to the degree degree
int bn_pow_to(bn *t, int degree) {
    if (degree == 0) {
        bn *one = bn_new();
        bn_init_int(one, 1);
        bn_copy(t, one);
        bn_delete(one);
        return 0;
    }
    bn *temp = bn_init(t);
    for (int i = 0; i < degree; i++) {
        bn_mul_to(t, temp);
    }
    bn_delete(temp);
    return 0;
}

/*// Initialize the value of BN by representing the string in radix
int bn_init_string_radix(bn *t, const char *init_string, int radix)*/


/*// functions which are left
// Extract the root of the reciprocal degree from BN (bonus function)
int bn_root_to(bn *t, int reciprocal);*/
