#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <malloc.h>
#include <locale.h>
//#include "bn.h"

/*1)�������������: ������ �� ���� � ���������� ������� ���������, ���������� � �������� ������� � �������� ���� � ���������*/
struct bn_s {
    int *body; //����
    int bodysize; // ������ ������� body
    int sign; // ���� �����: 0 ��� -, 1 ��� +
};

typedef struct bn_s bn;

/*//�����������
void print_bn (bn *r) {
    for (int i = r -> bodysize - 1; i >= 0; i--) {
        printf("%d", r -> body[i]);
    }
}*/

// ������� ����� BN, ����� ��� bn *a = bn_new() ���� a = 0
bn *bn_new() {
    //�������� ������ ��� ���� ������� ���� bn
    bn *r = (bn *)malloc (sizeof (bn));
    // ���� � ��� �� ������� ���������
    if (r == NULL) {
        return NULL;
    }
    r -> bodysize = 1;
    r -> sign = 1;
    r -> body = (int *)malloc (sizeof (int) * r -> bodysize);
    //����������
    if (r -> body == NULL) {
        free(r);
        return NULL;
    }
    r -> body[0] = 0;
    return r;
}

// ������� ����� ������������� BN bn *b = bn_init(a); �������� ����� a � ���� b
bn *bn_init(bn const *orig) {
    //�������� ������ ��� ���� ������� ���� bn (��� ����� ���� ���������, �.�. ���� orig ��������� �� ������ �� ��������� ���� bn ��� �� ��������)
    bn *r = (bn *)malloc (sizeof (bn));
    //���� � ��� �� ������� ���������
    if (r == NULL) {
        return NULL;
    }
    r -> bodysize = orig -> bodysize;
    r -> sign = orig -> sign;
    r -> body = (int *)malloc(sizeof (int) * orig -> bodysize);
    //����������
    if (r -> body == NULL) {
        free(r);
        return NULL;
    }
    for (int i = 0; i < orig -> bodysize; i++) {
        r -> body[i] = orig -> body[i];
    }
    return r;
}

// ���������������� �������� BN ���������� �������������� ������
int bn_init_string(bn *t, const char *init_string) {
    //������� t -> body
    free (t -> body);
    //������� t
    free(t);
    //������� ����� t
    t = (bn *)malloc (sizeof (bn));
    // ���� � ��� ��������� �� ������� ���������, ���������� ������
    if (t == NULL) {
        free(t);
        return 1;
    }
    //���� ������ �� ������� ���������, ���������� ������
    if (init_string == NULL) {
        free(t);
        return 1;
    }
    //��������� ������ ������ t -> bodysize � ���� t -> sign
    int does_it_has_minos = 0;
    if (strlen(init_string) > 0 && init_string[0] == '-') {
        does_it_has_minos = 1;
    }
    t -> sign = 1 - does_it_has_minos;
    t -> bodysize = strlen(init_string) - does_it_has_minos;
    //���� � ��� ���������� 0 ��������� � t -> bodysize �� ������� ������
    if (t -> bodysize == 0) {
        free(t);
        return 1;
    }
    //�������� ������ ������� ������� ��� t -> body
    t -> body = (int *)malloc(sizeof(int) * t -> bodysize);
    //���� � ��� ��������� �� ������� ���������, ���������� ������
    if (t -> body == NULL) {
        free(t -> body);
        free(t);
        return 1;
    }
    //���������� � t -> body ��� �������� � �������� ������� � �������� ��� ��� �������� ����� ������
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

// ���������������� �������� BN �������� ����� ������
int bn_init_int(bn *t, int init_int) {
    //������� t -> body
    free(t -> body);
    //������� t
    free(t);
    //������� ����� t
    t = (bn *)malloc (sizeof (bn));
    // ���� � ��� ��������� �� ������� ���������, ���������� ������
    if (t == NULL) {
        free(t);
        return 1;
    }
    //��������� ������ ������ t -> bodysize � ���� t -> sign
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
    //���� � ��� init_int == 0
    if (init_int == 0) {
        t -> bodysize = 1;
    }
    //�������� ������ ������� ������� ��� t -> body
    t -> body = (int *)malloc(sizeof(int) * t -> bodysize);
    //���� � ��� ��������� �� ������� ���������, ���������� ������
    if (t -> body == NULL) {
        free(t -> body);
        free(t);
        return 1;
    }
    //���������� � t -> body ��� �������� � �������� �������
    int i = 0;
    while (temp2 != 0) {
        t -> body[i] = temp2 % 10;
        if (t -> sign == 0) {
            t -> body[i] *= -1;
        }
        temp2 /= 10;
        i++;
    }
    //���� � ��� init_int == 0
    if (init_int == 0) {
        t -> body[i] = 0;
    }
    return 0;
}

// ���������� BN (���������� ������)
int bn_delete(bn *t) {
    //������� t -> body
    free(t -> body);
    //������� t
    free(t);
    return 0;
}

// ���� ����� ������, ������� <0; ���� �����, ������� 0; ����� >0, ���������� �� ������ �������
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

// ���� ����� ������, ������� <0; ���� �����, ������� 0; ����� >0
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

// �������� x = l+r �� ������, ������� ��� |left| >= |right| � ����� ��� ��������� ����������
bn* abs_bn_add(bn const *left, bn const *right) {
    // �������� ������ ��� ���� ������� ���� bn
    bn *r = (bn *)malloc (sizeof (bn));
    // ���� � ��� �� ������� ��������� ��������� ���������� NULL
    if (r == NULL) {
        free(r);
        return NULL;
    }
    // ���� r -> sign = 1, �.�. � ��� �������� �� ������, ������ ���� "+" ��������������
    r -> sign = 1;
    // ���������� ������ ������� left -> bodysize, ����� ������� 1 ���� �����������
    r -> bodysize = left -> bodysize;
    //�������� ������ ��� r -> body : (left -> bodysize) + (1);
    r -> body = (int *)malloc (sizeof (int) * (left -> bodysize + 1));
    // ���� � ��� �� ������� ��������� ��������� ���������� NULL
    if (r -> body == NULL) {
        free(r);
        free(r -> body);
        return NULL;
    }
    //��� ���������� ���� ������� ������� ����������� �� ���������
    int additional_one = 0;
    //����
    for (int i = 0; i < left -> bodysize + 1; i++) {
        int digit = 0;
        if (i < right -> bodysize) {
            digit = left -> body[i] + right -> body[i];
            if (additional_one) {
                //������� ����������� �������
                digit++;
            }
            if (digit >= 10) {
                //���������� ������� �������
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
                //������� ����������� �������
                digit++;
            }
            if (digit >= 10) {
                //���������� ������� �������
                additional_one = 1;
                digit -= 10;
            }
            else {
                additional_one = 0;
            }
            r -> body[i] = digit;
        }
        else if (i == left -> bodysize) {
            //���� �������� �� ���� ������� ����� ��� left
            if (additional_one) {
                r -> bodysize += 1;
                r -> body[i] = 1;
            }
        }
    }
    return r;
}

//������� ���� � �����
void remove_zeros_from_the_end (bn *r) {
    while (r -> bodysize > 0 && r -> body[r -> bodysize - 1] == 0) {
        r -> bodysize -= 1;
    }
}

// �������� x = l-r �� ������, ������� ��� |left| >= |right| � ����� ��� ��������� ����������
bn* abs_bn_sub(bn const *left, bn const *right) {
    //���� ����� �� ������, ���������� 0
    if (abs_bn_cmp(left, right) == 0) {
         bn *r = bn_new();
         return r;
    }
    // �������� ������ ��� ���� ������� ���� bn
    bn *r = (bn *)malloc (sizeof (bn));
    // ���� � ��� �� ������� ��������� ��������� ���������� NULL
    if (r == NULL) {
        free(r);
        return NULL;
    }
    // ���� r -> sign = 1, �.�. � ��� ��������� �� ������, ������ ���� "+" ��������������
    r -> sign = 1;
    // ���������� ������ ������� left -> bodysize, ����� ����� ��������� �� 1 ���� � ����� 0
    r -> bodysize = left -> bodysize;
    //�������� ������ ��� r -> body : (left -> bodysize)
    r -> body = (int *)malloc (sizeof (int) * (left -> bodysize));
    // ���� � ��� �� ������� ��������� ��������� ���������� NULL
    if (r -> body == NULL) {
        free(r);
        free(r -> body);
        return NULL;
    }
    // ��� ���������� ���� �������, ������� ����������� � ����������
    int minus_one = 0;
    //����
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
    //������� ��������� � ����� ����
    remove_zeros_from_the_end(r);
    return r;
}

// �������� x = l+r
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

// �������� x = l-r
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
