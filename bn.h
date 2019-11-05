#pragma once
// ���� bn.h
struct bn_s;
typedef struct bn_s bn;

enum bn_codes {
BN_OK, BN_NULL_OBJECT, BN_NO_MEMORY, BN_DIVIDE_BY_ZERO
};

bn *bn_new(); // ������� ����� BN (+)
bn *bn_init(bn const *orig); // ������� ����� ������������� BN (+)

// ���������������� �������� BN ���������� �������������� ������ (+)
int bn_init_string(bn *t, const char *init_string);

// ���������������� �������� BN �������������� ������ � ������� ��������� radix
int bn_init_string_radix(bn *t, const char *init_string, int radix);

// ���������������� �������� BN �������� ����� ������ (+)
int bn_init_int(bn *t, int init_int);

// ���������� BN (���������� ������) (+)
int bn_delete(bn *t);

// ��������, ����������� +=, -=, *=, /=, %=
int bn_add_to(bn *t, bn const *right); // (+)
int bn_sub_to(bn *t, bn const *right); // (+)
int bn_mul_to(bn *t, bn const *right);
int bn_div_to(bn *t, bn const *right);
int bn_mod_to(bn *t, bn const *right);

// �������� ����� � ������� degree
int bn_pow_to(bn *t, int degree);

// ������� ������ ������� reciprocal �� BN (�������� �������)
int bn_root_to(bn *t, int reciprocal);

// ������� �������� x = l+r (l-r, l*r, l/r, l%r)
bn* bn_add(bn const *left, bn const *right);
bn* bn_sub(bn const *left, bn const *right);
bn* bn_mul(bn const *left, bn const *right);
bn* bn_div(bn const *left, bn const *right);
bn* bn_mod(bn const *left, bn const *right);

// ������ ������������� BN � ������� ��������� radix � ���� ������
// ������ ����� ������������� ����������� �������.
const char *bn_to_string(bn const *t, int radix);

// ���� ����� ������, ������� <0; ���� �����, ������� 0; ����� >0 (+)
int bn_cmp(bn const *left, bn const *right);

int bn_neg(bn *t); // �������� ���� �� ���������������
int bn_abs(bn *t); // ����� ������
int bn_sign(bn const *t); //-1 ���� t<0; 0 ���� t = 0, 1 ���� t>0
