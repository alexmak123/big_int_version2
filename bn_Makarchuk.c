#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <malloc.h>
#include <string.h>
#include <locale.h>
#include <assert.h>
#include <string.h>

typedef struct bn_s bn;
typedef unsigned int u_int;
typedef unsigned long long int my_u_long;
const int RADIX_2_MOD = 30;

struct bn_s {
    // our body
    u_int *body;
    // number of cells that create number, without leading zeros; if bigint is zero, then it would be zero
    int size;
    // 0 if number is positive, 1 if negative
    int is_neg;
};

// enough to fit in 100000 symbols
const int MAX_SIZE = 12000;
// numeric system
const int MOD = 100000000;
// max amount of digits in one cell of our massive body
const int NUM_DIGITS = 8;

u_int max(u_int a, u_int b) {
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

// operation |t| /= num, ignores the sign of t
// 1 <= num <= 36 should hold
void abs_divide_by_int(bn *t, int num) {
    assert(1 <= num && num <= 36);
    for (int i = t -> size - 1; i >= 0; i--) {
        if (i) {
            // because MOD = 10^8 and MOD * () % radix <= 36 * 10^8
            // and unsigned int ~42 * 10^8 - this will not overflow
            t -> body[i - 1] += (t -> body[i] % num) * MOD;
        }
        t -> body[i] /= num;
    }
    recalculate_bn_size(t);
}

// converts digit in radix system to char
// 0 <= radix_digit < 36
char radix_digit_to_char(int radix_digit) {
    assert(0 <= radix_digit && radix_digit < 36);
    if (radix_digit < 10) {
        return '0' + radix_digit;
    }
    return 'A' + radix_digit - 10;
}

// converts char in radix system to int
// ('0' <= c <= '9') || ('A' <= c <= 'Z')
int radix_char_to_digit(char c) {
    assert(('0' <= c && c <= '9') || ('A' <= c && c <= 'Z'));

    if ('0' <= c && c <= '9') {
        return c - '0';
    }
    return c - 'A' + 10;
}

// create a new bn that equals zero
bn *bn_new() {
    bn *t = calloc(1, sizeof(bn));
    assert(t != NULL);
    t -> is_neg = 0;
    t -> body = calloc(MAX_SIZE, sizeof(MAX_SIZE));
    assert(t -> body != NULL);
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
        init_int = -init_int;
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
        u_int digit = t -> body[i] + right -> body[i];
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
    for (int i = 0; i < t -> size; i++) {
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
        t -> body[i] = (u_int)digit;
    }
    //resize excluding zeros
    recalculate_bn_size(t);
    return 0;
}

// compare by abs, if the left is less, return < 0; if equal, return 0; otherwise > 0
int abs_bn_cmp(bn const *left, bn const *right) {
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

// operation x = |left| * |right|
bn *abs_bn_mul(bn const *left, bn const *right) {
    int length = left -> size + right -> size + 1;

    my_u_long *mult_res = calloc(length, sizeof(my_u_long));

    for (int i = 0; i < left -> size; i++) {
        for (int j = 0; j < right -> size; j++) {
            mult_res[i + j] += (my_u_long) left -> body[i] * (my_u_long) right -> body[j];
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

// Raise the number to the degree degree
int bn_pow_to(bn *t, int degree) {
    bn *one = bn_new();
    bn_init_int(one, 1);
    while (degree) {
        if (degree % 2 != 0) {
            bn_mul_to(one, t);
            degree -= 1;
        }
        else {
            bn_mul_to(t, t);
            degree /= 2;
        }
    }
    bn_copy(t, one);
    bn_delete(one);
    return 0;
}

int bn_root_to(bn *t, int reciprocal) {
    bn *lower = bn_new();
    bn *upper = bn_init(t);

    bn *one = bn_new();
    bn_init_int(one, 1);

    bn *ans = NULL;

    while (ans == NULL) {
        bn *mid = bn_add(lower, upper);
        abs_divide_by_int(mid, 2);
        bn *mid_plus_one = bn_add(mid, one);

        bn* pow_mid = bn_init(mid);
        bn_pow_to(pow_mid, reciprocal);
        bn* pow_mid_plus_one = bn_init(mid_plus_one);
        bn_pow_to(pow_mid_plus_one, reciprocal);

        int comp_mid = abs_bn_cmp(pow_mid, t);
        int comp_mid_plus_one = abs_bn_cmp(pow_mid_plus_one, t);

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
        bn_delete(pow_mid);
        bn_delete(pow_mid_plus_one);
    }

    bn_copy(t, ans);
    bn_delete(lower);
    bn_delete(upper);
    bn_delete(one);
    bn_delete(ans);
    return 0;
}

void reverse_string(char *s, int size) {
    for (int i = 0; 2 * i < size; i++) {
        char temp = s[i];
        s[i] = s[size - i - 1];
        s[size - i - 1] = temp;
    }
}

// returns a string representation of bn
const char *bn_to_string(const bn *const_t, int radix) {
    assert(2 <= radix && radix <= 36);

    if (const_t -> size == 0) {
        char *res = calloc(1, sizeof(char));
        res[0] = '0';
        return res;
    }

    // make a copy of t that we can modify
    bn *t = bn_init(const_t);

    // creates constant bn that equals to zero
    bn *bn_zero = bn_new();
    // creates constant bn that equals to radix
    bn *bn_radix = bn_new();
    bn_init_int(bn_radix, radix);

    // abs_bn_cmp will return 0 only when t is zero
    int is_positive = abs_bn_cmp(bn_zero, t);

    // allocate enough to fit number and maybe the minus sign
    char *res = calloc((const_t -> size) * 30 + 1, sizeof(char));
    int i = 0;
    while (is_positive) {
        bn *t_div = bn_init(t);
        abs_divide_by_int(t_div, radix);

        bn *t_div_mult = abs_bn_mul(t_div, bn_radix);

        // mod now in t and is guaranteed to be < 36 - thus there is only one cell
        abs_bn_sub_to(t, t_div_mult);
        assert(t -> size <= 1);
        int mod = t -> body[0];

        assert(0 <= mod && mod < radix);

        res[i] = radix_digit_to_char(mod);
        i++;

        bn_delete(t);
        bn_delete(t_div_mult);

        t = t_div;
        is_positive = abs_bn_cmp(bn_zero, t);
    }
    if (t -> is_neg) {
        res[i] = '-';
        i++;
    }
    reverse_string(res, i);

    bn_delete(t);
    bn_delete(bn_zero);
    bn_delete(bn_radix);
    return res;
}

// initialize the value of BN with the decimal representation of the string
int bn_init_string(bn *t, const char *init_string) {
    set_zero(t);
    if (strlen(init_string) == 1 && init_string[0] == '0') {
        return 0;
    }
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

// Initialize the value of BN by representing the string in radix
int bn_init_string_radix(bn *t, const char *init_string, int radix) {
    assert(2 <= radix && radix <= 36);

    set_zero(t);

    bn *bn_radix = bn_new();
    bn_init_int(bn_radix, radix);

    for (int i = 0; i < strlen(init_string); i++) {
        bn_mul_to(t, bn_radix);
        bn *bn_digit = bn_new();
        bn_init_int(bn_digit, radix_char_to_digit(init_string[i]));
        bn_add_to(t, bn_digit);
        bn_delete(bn_digit);
    }

    bn_delete(bn_radix);
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
        res -> is_neg = 1;
    }
    else {
        res = abs_bn_div(left, right);
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

int main()
{
    char *a = malloc(sizeof(char) * 100000);
    char *b = malloc(sizeof(char) * 100000);
    char symbol;
    scanf("%s", a);
    scanf("\n %c \n", &symbol);
    scanf("%s", b);
    bn *first = bn_new();
    bn *second = bn_new();
    bn *res = NULL;
    bn_init_string(first, a);
    bn_init_string(second, b);
    if (symbol == '/') {
        res = bn_div(first, second);
    }
    char *otv = bn_to_string(res, 10);
    printf("%s\n", otv);

    free(otv);
    free(a);
    free(b);
    bn_delete(first);
    bn_delete(second);
    bn_delete(res);
    return 0;
}
