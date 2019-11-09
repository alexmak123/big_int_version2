/*  WHAT IS IMPORTANT AND WHAT TO PAY ATTENTION
    1)memory allocation and leak
    2) correct pointers
    3) availability of assert for each block (maximum decomposition)
    4) make the normalize function, which will bring all numbers to a single standard, for example
    removes leading zeros, converts -0 -> 0 and we will call it at the very end immediately before the output of the number
    ()
    5) make a check function that checks if all digits are less than the base of the number system and> = 0, etc. (...)
    6) in main, all tests, everything else in bn_Makarchuk.c
    7) the written functions in bn_Makarchuk.c should not input or output anything!
    8) all error situations should be processed without displaying anything on the screen
    9) functions that return pointers should return NULL on error,
    other functions should return 0 if successful completion of the operation and an error code from the enumerated type bn_codes if unsuccessful
    10) for division, we have such a table:
        17/10 = 1
        17% 10 = 7
        -17 / 10 = -2
        -17% 10 = 3
        17 / -10 = -2
        17% -10 = -3
        -17 / -10 = 1
        -17% -10 = -7
    11) check pointers for operability bn * r = (bn *) malloc (sizeof (bn));
        if (r == NULL) {
            return NULL;
        }
*/

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "bn.h"

struct bn_s {
    int *body;
    int sign;
};

//print our structure, can be printed with leading zeros if we do not delete them
void my_bn_to_string(char *res, const bn *r, int max_size) {
    if (r -> sign == 0) {
        printf("-");
    }
    int j = max_size - 1;
    while (r -> body[j] == 0) {
        j--;
    }
    for (int i = j; i >= 0; i--) {
        res[j - i] = r -> body[i] + '0';
    }
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
    char *res = malloc(sizeof(char) * 50000);
    my_bn_to_string(res, b, 50000);
    printf("%s", res);
    return 0;
}
