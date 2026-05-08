typedef struct {
  char _sign;
  char * _digits;
  unsigned int _n; 
} BigInt;

BigInt make_big_int(long long _number);

BigInt big_int_copy(BigInt _b);

int big_int_eq(BigInt _b1, BigInt _b2);

int big_int_gt(BigInt _b1, BigInt _b2);

BigInt big_int_plus(BigInt _b1, BigInt _b2);
