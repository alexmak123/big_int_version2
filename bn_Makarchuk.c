#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <malloc.h>
#include <locale.h>
//#include "bn.h"

/*
1) representation: an array of digits in decimal notation, written in reverse order and separately sign in the structure*/
struct bn_s {
    int *body; //body
    int bodysize; //bodysize
    int sign; // sing 0 is -, 1 is +
};

typedef struct bn_s bn;

/*//print
void print_bn (bn *r) {
    for (int i = r -> bodysize - 1; i >= 0; i--) {
        printf("%d", r -> body[i]);
    }
}*/

//Create a new bn such that bn * a = bn_new () will give a = 0
bn *bn_new() {
    //allocate memory for one element of type bn
    bn *r = (bn *)malloc (sizeof (bn));
    //if we do not have a working pointer
    if (r == NULL) {
        return NULL;
    }
    r -> bodysize = 1;
    r -> sign = 1;
    r -> body = (int *)malloc (sizeof (int) * r -> bodysize);
    //same
    if (r -> body == NULL) {
        free(r);
        return NULL;
    }
    r -> body[0] = 0;
    return r;
}

//Create a copy of the existing bn, bn * b = bn_init (a); we get a copy of a in the form b
bn *bn_init(bn const *orig) {
    //we allocate memory for one element of type bn (there may be a snag, because if orig has a pointer to an array of elements of type bn it does not work)
    bn *r = (bn *)malloc (sizeof (bn));
    //if we do not have a working pointer
    if (r == NULL) {
        return NULL;
    }
    r -> bodysize = orig -> bodysize;
    r -> sign = orig -> sign;
    r -> body = (int *)malloc(sizeof (int) * orig -> bodysize);
    //same
    if (r -> body == NULL) {
        free(r);
        return NULL;
    }
    for (int i = 0; i < orig -> bodysize; i++) {
        r -> body[i] = orig -> body[i];
    }
    return r;
}

// Initialize the value of bn with the decimal representation of the string
int bn_init_string(bn *t, const char *init_string) {
    //delete t -> body
    free (t -> body);
    //delete t
    free(t);
    //create new t
    t = (bn *)malloc (sizeof (bn));
    // pointer is not working
    if (t == NULL) {
        free(t);
        return 1;
    }
    //same
    if (init_string == NULL) {
        free(t);
        return 1;
    }
    //define t -> bodysize and t -> sign
    int does_it_has_minos = 0;
    if (strlen(init_string) > 0 && init_string[0] == '-') {
        does_it_has_minos = 1;
    }
    t -> sign = 1 - does_it_has_minos;
    t -> bodysize = strlen(init_string) - does_it_has_minos;
    //if 0 elem in t -> bodysize return error
    if (t -> bodysize == 0) {
        free(t);
        return 1;
    }
    //allocate memory
    t -> body = (int *)malloc(sizeof(int) * t -> bodysize);
    //not working pointer
    if (t -> body == NULL) {
        free(t -> body);
        free(t);
        return 1;
    }
    //write in t -> body all the elements in the reverse order and check that all the elements are in accordance with numbers
    for (int i = 0; i < t -> bodysize; i++) {
        if (init_string[strlen(init_string) - 1 - i] >= '0' && init_string[strlen(init_string) - 1 - i] <= '9') {
            t -> body[i] = init_string[strlen(init_string) - 1 - i] - '0';
        }
        else {
            free(t -> body);
            free(t);
            return 1;
        }
    }
    return 0;
}

//Initialize the value of bn with a given integer
int bn_init_int(bn *t, int init_int) {
    //delete t -> body
    free(t -> body);
    //delete t
    free(t);
    //create new t
    t = (bn *)malloc (sizeof (bn));
    // not working pointer
    if (t == NULL) {
        free(t);
        return 1;
    }
    //define t -> bodysize and t -> sign
    t -> sign = 1;
    if (init_int < 0) {
        t -> sign = 0;
    }
    t -> bodysize = 0;
    int temp1 = init_int;
    int temp2 = init_int;
    while (temp1 != 0) {
        temp1 /= 10;
        t -> bodysize += 1;
    }
    //if init_int == 0
    if (init_int == 0) {
        t -> bodysize = 1;
    }
    //allocate memory t -> body
    t -> body = (int *)malloc(sizeof(int) * t -> bodysize);
    //not working pointer
    if (t -> body == NULL) {
        free(t -> body);
        free(t);
        return 1;
    }
    //write to t -> body all elements in reverse order
    int i = 0;
    while (temp2 != 0) {
        t -> body[i] = temp2 % 10;
        if (t -> sign == 0) {
            t -> body[i] *= -1;
        }
        temp2 /= 10;
        i++;
    }
    //if init_int == 0
    if (init_int == 0) {
        t -> body[i] = 0;
    }
    return 0;
}

// delete bn
int bn_delete(bn *t) {
    free(t -> body);
    free(t);
    return 0;
}

// If the left is less, return <0; if equal, return 0; otherwise> 0, compare modulo first
int abs_bn_cmp(bn const *left, bn const *right) {
    if (left -> bodysize < right -> bodysize) {
        return -1;
    }
    else if (left -> bodysize > right -> bodysize) {
        return 1;
    }
    else {
        for (int i = right -> bodysize - 1; i >= 0; i--) {
            if (left -> body[i] > right -> body[i]) {
                return 1;
            }
            else if (left -> body[i] < right -> body[i]) {
                return -1;
            }
        }
        return 0;
    }
}

// If the left is less, return <0; if equal, return 0; otherwise> 0
int bn_cmp(bn const *left, bn const *right) {
    if (left -> sign == 0 && right -> sign == 1) {
        return -1;
    }
    else if (left -> sign == 1 && right -> sign == 0) {
        return 1;
    }
    else if (left -> sign == 0 && right -> sign == 0) {
        return abs_bn_cmp(right, left);
    }
    else {
        return abs_bn_cmp(left, right);
    }
}

// The operation x = l + r modulo, we assume that | left | > = | right | and we know that the pointers are normal
bn* abs_bn_add(bn const *left, bn const *right) {
    // allocate memory for one element of type bn
    bn *r = (bn *)malloc (sizeof (bn));
    // not working pointer
    if (r == NULL) {
        free(r);
        return NULL;
    }

    // sign r -> sign = 1, because we have modulo addition, always the "+" sign, respectively
    r -> sign = 1;
    // initially make the size left -> bodysize, then add 1 if necessary
    r -> bodysize = left -> bodysize;
    // allocate memory under r -> body: (left -> bodysize) + (1);
    r -> body = (int *)malloc (sizeof (int) * (left -> bodysize + 1));
    // not working pointer
    if (r -> body == NULL) {
        free(r);
        free(r -> body);
        return NULL;
    }
    // this variable is ten which is carried over with addition
    int additional_one = 0;
    for (int i = 0; i < left -> bodysize + 1; i++) {
        int digit = 0;
        if (i < right -> bodysize) {
            digit = left -> body[i] + right -> body[i];
            if (additional_one) {
                // transfer the previous ten
                digit++;
            }
            if (digit >= 10) {
                //transfer of ten
                additional_one = 1;
                digit -= 10;
            }
            else {
                additional_one = 0;
            }
            r -> body[i] = digit;
        }
        else if (i >= right -> bodysize && i < left -> bodysize) {
            digit = left -> body[i];
            if (additional_one) {
                //transfer the previous ten
                digit++;
            }
            if (digit >= 10) {
                //transfer of ten
                additional_one = 1;
                digit -= 10;
            }
            else {
                additional_one = 0;
            }
            r -> body[i] = digit;
        }
        else if (i == left -> bodysize) {
            // if received ten more than left
            if (additional_one) {
                r -> bodysize += 1;
                r -> body[i] = 1;
            }
        }
    }
    return r;
}

// remove zeros at the end
void remove_zeros_from_the_end (bn *r) {
    while (r -> bodysize > 0 && r -> body[r -> bodysize - 1] == 0) {
        r -> bodysize -= 1;
    }
}


// The operation x = l + r modulo, we assume that | left | > = | right | and we know that the pointers are normal
bn* abs_bn_sub(bn const *left, bn const *right) {
    // if equal in absolute value, return 0
    if (abs_bn_cmp(left, right) == 0) {
         bn *r = bn_new();
         return r;
    }
    // allocate memory
    bn *r = (bn *)malloc (sizeof (bn));
    // not working pointer
    if (r == NULL) {
        free(r);
        return NULL;
    }
    //r -> sign = 1 always
    r -> sign = 1;
    // initially do the size left -> bodysize, then we will decrease by 1 until at the end 0
    r -> bodysize = left -> bodysize;
    //allocate memory
    r -> body = (int *)malloc (sizeof (int) * (left -> bodysize));
    // not working pointer
    if (r -> body == NULL) {
        free(r);
        free(r -> body);
        return NULL;
    }

    // This variable is a dozen, which decreases with subtraction
    int minus_one = 0;
    for (int i = 0; i < left -> bodysize; i++) {
        int digit = 0;
        if (i < right -> bodysize) {
            digit = left -> body[i] - right -> body[i];
            if (minus_one) {
                digit--;
            }
            if (digit < 0) {
                digit += 10;
                minus_one = 1;
            }
            else {
                minus_one = 0;
            }
            r -> body[i] = digit;
        }
        else if (i >= right -> bodysize && i < left -> bodysize) {
            digit = left -> body[i];
            if (minus_one) {
                digit--;
            }
            if (digit < 0) {
                digit += 10;
                minus_one = 1;
            }
            else {
                minus_one = 0;
            }
            r -> body[i] = digit;
        }
    }
    // function that removes zeros at the end
    remove_zeros_from_the_end(r);
    return r;
}

// x = l+r
bn* bn_add(bn const *left, bn const *right) {
    bn *r;
    if (left -> sign == 1 && right -> sign == 1) {
        if (abs_bn_cmp(left, right) >= 0) {
            r = abs_bn_add(left, right);
        }
        else {
            r = abs_bn_add(right, left);
        }
        r -> sign = 1;
    }
    else if (left -> sign == 1 && right -> sign == 0) {
        if (abs_bn_cmp(left, right) >= 0) {
            r = abs_bn_sub(left, right);
            r -> sign = 1;
        }
        else {
            r = abs_bn_sub(right, left);
            r -> sign = 0;
        }
    }
    else if (left -> sign == 0 && right -> sign == 1) {
        if (abs_bn_cmp(left, right) >= 0) {
            r = abs_bn_sub(left, right);
            r -> sign = 0;
        }
        else {
            r = abs_bn_sub(right, left);
            r -> sign = 1;
        }
    }
    else {
        if (abs_bn_cmp(left, right) >= 0) {
            r = abs_bn_add(left, right);
        }
        else {
            r = abs_bn_add(right, left);
        }
        r -> sign = 0;
    }
    return r;
}

// x = l-r
bn* bn_sub(bn const *left, bn const *right) {
    bn *r;
    if (left -> sign == 1 && right -> sign == 1) {
        if (abs_bn_cmp(left, right) >= 0) {
            r = abs_bn_sub(left, right);
            r -> sign = 1;
        }
        else {
            r = abs_bn_sub(right, left);
            r -> sign = 0;
        }
    }
    else if (left -> sign == 1 && right -> sign == 0) {
        if (abs_bn_cmp(left, right) >= 0) {
            r = abs_bn_add(left, right);
        }
        else {
            r = abs_bn_add(right, left);
        }
        r -> sign = 1;
    }
    else if (left -> sign == 0 && right -> sign == 1) {
        if (abs_bn_cmp(left, right) >= 0) {
            r = abs_bn_add(left, right);
        }
        else {
            r = abs_bn_add(right, left);
        }
        r -> sign = 0;
    }
    else {
        if (abs_bn_cmp(left, right) >= 0) {
            r = abs_bn_sub(left, right);
            r -> sign = 0;
        }
        else {
            r = abs_bn_sub(right, left);
            r -> sign = 1;
        }
    }
    return r;
}

/*int main () {
    int n;
    scanf("%d", &n);
    bn *first = bn_new();
    bn *second = bn_new();
    int a = 1;
    int b = 1;
    bn_init_int(first, a);
    bn_init_int(second, b);
    for (int i = 2; i < n; i++) {
         bn *temp = bn_init(second);
         bn_delete(second);
         second = bn_add(first, temp);
         bn_delete(first);
         first = bn_init(temp);
         bn_delete(temp);
    }
    print_bn(second);
    return 0;
}*/
