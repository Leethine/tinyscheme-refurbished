#include "bigint.h"
#include <string.h>

#ifndef _STDLIB_H
  #include <stdlib.h>
#endif
#include <stdio.h>

#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) > (b) ? (b) : (a))

#define MAX_LENGTH 1000

BigInt make_big_int(long long _number) {
  BigInt b;
  long long num_abs = _number;
  if (_number < 0) {
    b._sign = '-';
    num_abs = 0 - num_abs;
  }
  else {
    b._sign = '+';
  }
  char temp[30];
  if (num_abs == 0) {
    b._digits = (char *) malloc(sizeof(char) * 1);
    b._digits[0] = '0';
    b._n = 1;
    return b;
  }
  b._n = 0;
  while(num_abs / 10 != 0) {
    long long remainder = num_abs % 10;
    num_abs /= 10;
    temp[b._n++] = remainder + '0';
    ;
  }
  temp[b._n++] = num_abs + '0';
  b._digits = (char *) malloc(sizeof(char) * b._n);
  for (int i = 0; i < b._n; i++) {
    b._digits[i] = temp[b._n - i - 1];
  }
  return b;
}

BigInt big_int_copy(BigInt _b) {
  BigInt b1;
  b1._n = _b._n;
  b1._sign = _b._sign;
  b1._digits = (char *) malloc(sizeof(char) * _b._n);
  for (int i = 0; i < _b._n; i++) {
    b1._digits[i] = _b._digits[i];
  }
  return b1;
}

int big_int_eq(BigInt _b1, BigInt _b2) {
  if (_b1._sign == _b2._sign && _b1._n == _b2._n) {
    for (int i = 0; i < _b1._n; i++) {
      if (_b1._digits[i] != _b2._digits[i]) {
        return 0;
      }
    }
    return 1;
  }
  return 0;
}

int big_int_gt(BigInt _b1, BigInt _b2) {
  if (_b1._sign == '-' && _b2._sign == '+') {
    return 0;
  }
  else if (_b1._sign == '+' && _b2._sign == '-') {
    return 1;
  }
  if (_b1._n > _b2._n) {
    return _b1._sign == '+' ? 1 : 0;
  }
  else if (_b1._n < _b2._n) {
    return _b1._sign == '+' ? 0 : 1;
  }
  for (int i = 0; i < _b1._n; i++) {
    if (_b1._digits[i] > _b2._digits[i]) {
      return 1;
    }
    else if (_b1._digits[i] < _b2._digits[i]) {
      return 0;
    }
  }
  return 0;
}

BigInt big_int_plus(BigInt _b1, BigInt _b2) {
  BigInt b3;
  char temp[MAX(_b1._n, _b2._n) + 1];
  int j = 0;
  int i1 = _b1._n - 1;
  int i2 = _b2._n - 1;
  int n_max = MAX(_b1._n, _b2._n);
  int digit = 0;
  int next = 0;
  while (n_max-- > 0) {
    int d1 = 0;
    int d2 = 0;
    if (i1 >= 0) {
      d1 = _b1._digits[i1--] - '0';
    }
    if (i2 >= 0) {
      d2 = _b2._digits[i2--] - '0';
    }
    digit = d1 + d2 + next;
    next = digit / 10;
    digit = digit % 10;
    temp[j++] = digit + '0';
  }
  temp[j++] = next + '0';
  b3._n = j;
  b3._digits = (char *) malloc(sizeof(char) * b3._n);
  for (int i = 0; i < b3._n; i++) {
    b3._digits[i] = temp[b3._n - i - 1];
  }
  return b3;
}

int main(void) {
  BigInt big1 = make_big_int(999);
  BigInt big2 = big_int_copy(big1);
  BigInt big3 = make_big_int(112);
  BigInt big4 = big_int_plus(big1, big3);
  printf("Result: ");
  for (int i=0; i<big4._n; i++) {
    printf("%c", big4._digits[i]);
  }
  printf("\nDigits: %i\n", big4._n);
  return 0;
}
