#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <malloc.h>
#include <locale.h>
//#include "bn.h"

/*1)представление: массив из цифр в десятичной системе счисления, записанных в обратном порядке и отдельно знак в структуре*/
struct bn_s {
    int *body; //Тело
    int bodysize; // Размер массива body
    int sign; // Знак числа: 0 это -, 1 это +
};

typedef struct bn_s bn;

/*//распечатать
void print_bn (bn *r) {
    for (int i = r -> bodysize - 1; i >= 0; i--) {
        printf("%d", r -> body[i]);
    }
}*/

// Создать новое BN, такое что bn *a = bn_new() даст a = 0
bn *bn_new() {
    //выделяем память под один элемент типа bn
    bn *r = (bn *)malloc (sizeof (bn));
    // если у нас не рабочий указатель
    if (r == NULL) {
        return NULL;
    }
    r -> bodysize = 1;
    r -> sign = 1;
    r -> body = (int *)malloc (sizeof (int) * r -> bodysize);
    //аналогично
    if (r -> body == NULL) {
        free(r);
        return NULL;
    }
    r -> body[0] = 0;
    return r;
}

// Создать копию существующего BN bn *b = bn_init(a); получаем копию a в виде b
bn *bn_init(bn const *orig) {
    //выделяем память под один элемент типа bn (тут может быть загвоздка, т.к. если orig указатель на массив из элементов типа bn оно не работает)
    bn *r = (bn *)malloc (sizeof (bn));
    //если у нас не рабочий указатель
    if (r == NULL) {
        return NULL;
    }
    r -> bodysize = orig -> bodysize;
    r -> sign = orig -> sign;
    r -> body = (int *)malloc(sizeof (int) * orig -> bodysize);
    //аналогично
    if (r -> body == NULL) {
        free(r);
        return NULL;
    }
    for (int i = 0; i < orig -> bodysize; i++) {
        r -> body[i] = orig -> body[i];
    }
    return r;
}

// Инициалицировать значение BN десятичным представлением строки
int bn_init_string(bn *t, const char *init_string) {
    //удаляем t -> body
    free (t -> body);
    //удаляем t
    free(t);
    //создаем новое t
    t = (bn *)malloc (sizeof (bn));
    // если у нас получился не рабочий указатель, возвращаем ошибку
    if (t == NULL) {
        free(t);
        return 1;
    }
    //если подали не рабочий указатель, возвращаем ошибку
    if (init_string == NULL) {
        free(t);
        return 1;
    }
    //определим нужный размер t -> bodysize и знак t -> sign
    int does_it_has_minos = 0;
    if (strlen(init_string) > 0 && init_string[0] == '-') {
        does_it_has_minos = 1;
    }
    t -> sign = 1 - does_it_has_minos;
    t -> bodysize = strlen(init_string) - does_it_has_minos;
    //если у нас получилось 0 элементов в t -> bodysize то выводим ошибку
    if (t -> bodysize == 0) {
        free(t);
        return 1;
    }
    //выделяем память нужного размера под t -> body
    t -> body = (int *)malloc(sizeof(int) * t -> bodysize);
    //если у нас получился не рабочий указатель, возвращаем ошибку
    if (t -> body == NULL) {
        free(t -> body);
        free(t);
        return 1;
    }
    //записываем в t -> body все элементы в обратном порядке и проверка что все элементы соотв цифрам
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

// Инициалицировать значение BN заданным целым числом
int bn_init_int(bn *t, int init_int) {
    //удаляем t -> body
    free(t -> body);
    //удаляем t
    free(t);
    //создает новое t
    t = (bn *)malloc (sizeof (bn));
    // если у нас получился не рабочий указатель, возвращаем ошибку
    if (t == NULL) {
        free(t);
        return 1;
    }
    //определим нужный размер t -> bodysize и знак t -> sign
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
    //если у нас init_int == 0
    if (init_int == 0) {
        t -> bodysize = 1;
    }
    //выделяем память нужного размера под t -> body
    t -> body = (int *)malloc(sizeof(int) * t -> bodysize);
    //если у нас получился не рабочий указатель, возвращаем ошибку
    if (t -> body == NULL) {
        free(t -> body);
        free(t);
        return 1;
    }
    //записываем в t -> body все элементы в обратном порядке
    int i = 0;
    while (temp2 != 0) {
        t -> body[i] = temp2 % 10;
        if (t -> sign == 0) {
            t -> body[i] *= -1;
        }
        temp2 /= 10;
        i++;
    }
    //если у нас init_int == 0
    if (init_int == 0) {
        t -> body[i] = 0;
    }
    return 0;
}

// Уничтожить BN (освободить память)
int bn_delete(bn *t) {
    //удаляем t -> body
    free(t -> body);
    //удаляет t
    free(t);
    return 0;
}

// Если левое меньше, вернуть <0; если равны, вернуть 0; иначе >0, сравниваем по модулю сначала
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

// Если левое меньше, вернуть <0; если равны, вернуть 0; иначе >0
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

// Операция x = l+r по модулю, считаем что |left| >= |right| и знаем что указатели нормальные
bn* abs_bn_add(bn const *left, bn const *right) {
    // выделяем память под один элемент типа bn
    bn *r = (bn *)malloc (sizeof (bn));
    // если у нас не рабочий указатель получился возвращаем NULL
    if (r == NULL) {
        free(r);
        return NULL;
    }
    // знак r -> sign = 1, т.к. у нас сложение по модулю, всегда знак "+" соответственно
    r -> sign = 1;
    // изначально размер сделаем left -> bodysize, потом добавим 1 если потребуется
    r -> bodysize = left -> bodysize;
    //выделяем память под r -> body : (left -> bodysize) + (1);
    r -> body = (int *)malloc (sizeof (int) * (left -> bodysize + 1));
    // если у нас не рабочий указатель получился возвращаем NULL
    if (r -> body == NULL) {
        free(r);
        free(r -> body);
        return NULL;
    }
    //эта переменная есть десяток который переносится со сложением
    int additional_one = 0;
    //цикл
    for (int i = 0; i < left -> bodysize + 1; i++) {
        int digit = 0;
        if (i < right -> bodysize) {
            digit = left -> body[i] + right -> body[i];
            if (additional_one) {
                //перенос предыдущего десятка
                digit++;
            }
            if (digit >= 10) {
                //происходит перенос десятка
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
                //перенос предыдущего десятка
                digit++;
            }
            if (digit >= 10) {
                //происходит перенос десятка
                additional_one = 1;
                digit -= 10;
            }
            else {
                additional_one = 0;
            }
            r -> body[i] = digit;
        }
        else if (i == left -> bodysize) {
            //если получили на один десяток боьше чем left
            if (additional_one) {
                r -> bodysize += 1;
                r -> body[i] = 1;
            }
        }
    }
    return r;
}

//удаляем нули в конце
void remove_zeros_from_the_end (bn *r) {
    while (r -> bodysize > 0 && r -> body[r -> bodysize - 1] == 0) {
        r -> bodysize -= 1;
    }
}

// Операция x = l-r по модулю, считаем что |left| >= |right| и знаем что указатели нормальные
bn* abs_bn_sub(bn const *left, bn const *right) {
    //если равны по модулю, возвращаем 0
    if (abs_bn_cmp(left, right) == 0) {
         bn *r = bn_new();
         return r;
    }
    // выделяем память под один элемент типа bn
    bn *r = (bn *)malloc (sizeof (bn));
    // если у нас не рабочий указатель получился возвращаем NULL
    if (r == NULL) {
        free(r);
        return NULL;
    }
    // знак r -> sign = 1, т.к. у нас вычитание по модулю, всегда знак "+" соответственно
    r -> sign = 1;
    // изначально размер сделаем left -> bodysize, потом будем уменьшать по 1 пока в конце 0
    r -> bodysize = left -> bodysize;
    //выделяем память под r -> body : (left -> bodysize)
    r -> body = (int *)malloc (sizeof (int) * (left -> bodysize));
    // если у нас не рабочий указатель получился возвращаем NULL
    if (r -> body == NULL) {
        free(r);
        free(r -> body);
        return NULL;
    }
    // Эта переменная есть десяток, который уменьшается с вычитанием
    int minus_one = 0;
    //цикл
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
    //функция убирающая в конце нули
    remove_zeros_from_the_end(r);
    return r;
}

// Операция x = l+r
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

// Операция x = l-r
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
