#pragma once
// file bn.h
struct bn_s;
typedef struct bn_s bn;

enum bn_codes {
BN_OK, BN_NULL_OBJECT, BN_NO_MEMORY, BN_DIVIDE_BY_ZERO
};

bn *bn_new(); // create new BN (+)
bn *bn_init(bn const *orig); // create existing BN (+)

// Initialize the value of BN with the decimal representation of the string (+)
int bn_init_string(bn *t, const char *init_string);

// Initialize the value of BN by representing the string in radix
int bn_init_string_radix(bn *t, const char *init_string, int radix);

// Initialize the value of BN with a given integer (+)
int bn_init_int(bn *t, int init_int);

// Destroy BN (free memory) (+)
int bn_delete(bn *t);

// operations +=, -=, *=, /=, %=
int bn_add_to(bn *t, bn const *right);
int bn_sub_to(bn *t, bn const *right);
int bn_mul_to(bn *t, bn const *right);
int bn_div_to(bn *t, bn const *right);
int bn_mod_to(bn *t, bn const *right);

// Raise the number to the degree degree
int bn_pow_to(bn *t, int degree);

// Extract the root of the reciprocal degree from BN (bonus function)
int bn_root_to(bn *t, int reciprocal);

// operations x = l+r (l-r, l*r, l/r, l%r)
bn* bn_add(bn const *left, bn const *right); //(+)
bn* bn_sub(bn const *left, bn const *right); //(+)
bn* bn_mul(bn const *left, bn const *right);
bn* bn_div(bn const *left, bn const *right);
bn* bn_mod(bn const *left, bn const *right);

// Return the representation of BN in the radix number system as a string
// The line after use needs to be deleted.
const char *bn_to_string(bn const *t, int radix);

// If the left is less, return <0; if equal, return 0; otherwise> 0 (+)
int bn_cmp(bn const *left, bn const *right);

int bn_neg(bn *t); // Change the sign to the opposite
int bn_abs(bn *t); // Take module
int bn_sign(bn const *t); //-1 if t <0; 0 if t = 0, 1 if t> 0
