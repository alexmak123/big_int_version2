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
    9) functions that return pointers should return NULL on error, other functions should return 0 if successful completion of the operation and an error code from the enumerated type bn_codes if unsuccessful
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
    int bodysize;
    int sign;
};

void print_bn (bn *r) {
    for (int i = r -> bodysize - 1; i >= 0; i--) {
        printf("%d", r -> body[i]);
    }
}

void test_basic_konstructors () {
    bn *a = bn_new();
    bn *b = bn_init(a);

    assert(a -> body[0] == 0);
    assert(a -> bodysize == 1);
    assert(a -> sign == 1);
    assert(sizeof (*(a -> body)) * a -> bodysize == sizeof(int) * 1);
    assert(sizeof (*a) == sizeof (bn));

    assert(a -> body[0] == b -> body[0]);
    assert(a -> bodysize == b -> bodysize);
    assert(a -> sign == b -> sign);
    assert(sizeof (*(a -> body)) * a -> bodysize == sizeof (*(b -> body)) * b -> bodysize);
    assert(sizeof (*a) == sizeof (*b));

    assert(a != b);
    assert(a -> body != b -> body);

    printf("ok for basic konstructors\n");
}

void test_string_initialization () {
    bn *a = bn_new();


    char *init_string1 = "123456789";
    char *init_string2 = "-123456789";
    char *init_string3 = "";
    char *init_string4 = "-0";
    char *init_string5 = "%45/";
    char *init_string6 = "-000123";
    char *init_string7 = "123-456789";
    char *init_string8 = "-";
    char *init_string9 = NULL;


    int res1 = bn_init_string(a, init_string1);
    assert(res1 == 0);
    assert(a -> bodysize == 9);
    assert(a -> sign == 1);
    for (int i = 0; i < a -> bodysize; i++) {
        assert(a -> body[i] == 10 - i - 1);
    }
    print_bn(a);


    int res2 = bn_init_string(a, init_string2);
    assert(res2 == 0);
    assert(a -> bodysize == 9);
    assert(a -> sign == 0);
    for (int i = 0; i < a -> bodysize; i++) {
        assert(a -> body[i] == 10 - i - 1);
    }


    int res3 = bn_init_string(a, init_string3);
    assert(res3 == 1);


    int res4 = bn_init_string(a, init_string4);
    assert(res4 == 0);
    assert(a -> bodysize == 1);
    assert (a -> sign == 0);
    assert(a -> body[0] == 0);


    int res5 = bn_init_string(a, init_string5);
    assert(res5 == 1);


    int res6 = bn_init_string(a, init_string6);
    assert(res6 == 0);
    assert(a -> bodysize == 6);
    assert(a -> sign == 0);
    assert(a -> body[0] == 3);
    assert(a -> body[1] == 2);
    assert(a -> body[2] == 1);
    assert(a -> body[3] == 0);
    assert(a -> body[4] == 0);
    assert(a -> body[5] == 0);


    int res7 = bn_init_string(a, init_string7);
    assert(res7 == 1);


    int res8 = bn_init_string(a, init_string8);
    assert(res8 == 1);


    int res9 = bn_init_string(a, init_string9);
    assert(res9 == 1);


    printf("ok for string initialization\n");
}

void test_number_initialization () {
    bn *a = bn_new();


    int init_int1 = 123456789;
    int init_int2 = -123456789;
    //int init_int3;
    int init_int4 = -0;
    int init_int5 = 0;
    int init_int6 = -123000;


    int res1 = bn_init_int(a, init_int1);
    assert(res1 == 0);
    assert(a -> bodysize == 9);
    assert(a -> sign == 1);
    for (int i = 0; i < a -> bodysize; i++) {
        assert(a -> body[i] == 10 - i - 1);
    }


    int res2 = bn_init_int(a, init_int2);
    assert(res2 == 0);
    assert(a -> bodysize == 9);
    assert(a -> sign == 0);
    for (int i = 0; i < a -> bodysize; i++) {
        assert(a -> body[i] == 10 - i - 1);
    }


    /*int res3 = bn_init_int(a, init_int3);
    assert(res3 == 1);*/


    int res4 = bn_init_int(a, init_int4);
    assert(res4 == 0);
    assert(a -> bodysize == 1);
    assert (a -> sign == 1);
    assert(a -> body[0] == 0);


    int res5 = bn_init_int(a, init_int5);
    assert(res5 == 0);
    assert(a -> bodysize == 1);
    assert (a -> sign == 1);
    assert(a -> body[0] == 0);


    int res6 = bn_init_int(a, init_int6);
    assert(res6 == 0);
    assert(a -> bodysize == 6);
    assert (a -> sign == 0);
    assert(a -> body[0] == 0);
    assert(a -> body[1] == 0);
    assert(a -> body[2] == 0);
    assert(a -> body[3] == 3);
    assert(a -> body[4] == 2);
    assert(a -> body[5] == 1);


    printf("ok for number initialization\n");
}

void test_freeing_up_memory () {
    bn *a = bn_new();
    bn_init_int(a, 123456789);


    int res1 = bn_delete(a);
    assert(res1 == 0);
    assert(a -> bodysize != 9);

    printf("ok for freeing up memory\n");
}

/*void test_operation_add_first_to_second () {
    bn *a = bn_new();
    bn *b = bn_new();


    bn_init_string(a, "");
    bn_init_string(b, "");
    bn *c = bn_add(a, b);
    assert (c == );


    bn_init_string(a, "");
    bn_init_string(b, "");
    bn *c = bn_add(a, b);
    assert (c == );


    bn_init_string(a, "");
    bn_init_string(b, "");
    bn *c = bn_add(a, b);
    assert (c == );


    bn_init_string(a, "");
    bn_init_string(b, "");
    bn *c = bn_add(a, b);
    assert (c == );


    printf("ok for operation add to first\n");
}

void test_operation_sub_first_from_second () {
    bn *a = bn_new();
    bn *b = bn_new();


    bn_init_string(a, "");
    bn_init_string(b, "");
    bn *c = bn_sub(a, b);
    assert (c == );


    bn_init_string(a, "");
    bn_init_string(b, "");
    bn *c = bn_sub(a, b);
    assert (c == );


    bn_init_string(a, "");
    bn_init_string(b, "");
    bn *c = bn_sub(a, b);
    assert (c == );


    bn_init_string(a, "");
    bn_init_string(b, "");
    bn *c = bn_sub(a, b);
    assert (c == );
    printf("ok for operation sub from first\n");
}*/

int main()
{
    //test_basic_konstructors();
    //test_string_initialization();
    //test_number_initialization();
    //test_freeing_up_memory ();
    //test_operation_add_first_to_second ();
    //test_operation_sub_first_from_second ();
    //printf("ok all");
    int n;
    scanf("%d", &n);
    bn *first = bn_new();
    bn *second = bn_new();
    bn_init_int(first, 1);
    bn_init_int(second, 1);
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
}
