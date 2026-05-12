#include <tommath.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static char * big_num_op(const char* number1, size_t size1, const char* number2, size_t size2, char op) {
  mp_int x1, x2, x3, x4;
  int err;

  // Init mp_int
  err = mp_init_multi(&x1, &x2, &x3, &x4, NULL);
  if (err != MP_OKAY) { return NULL; }

  // Copy input char array and add '\0' to the end
  size_t bufferSizeInput = size1 > size2 ? size1 : size2;
  bufferSizeInput++;
  char num1_array[bufferSizeInput];
  char num2_array[bufferSizeInput];
  strncpy(num1_array, number1, size1);
  num1_array[size1] = '\0';
  strncpy(num2_array, number2, size2);
  num2_array[size2] = '\0';

  // Read from input string
  err = mp_read_radix(&x1,num1_array,10);
  if (err != MP_OKAY) { return NULL; }
  err = mp_read_radix(&x2,num2_array,10);
  if (err != MP_OKAY) { return NULL; }

  // Calculation
  if      (op == '+') { err = mp_add(&x1, &x2, &x3); }
  else if (op == '-') { err = mp_sub(&x1, &x2, &x3); }
  else if (op == '*') { err = mp_mul(&x1, &x2, &x3); }
  else if (op == '/') { err = mp_div(&x1, &x2, &x3, &x4); }
  else if (op == '%') { err = mp_div(&x1, &x2, &x4, &x3); }
  else                { err = mp_add(&x1, &x2, &x3); }

  if (err != MP_OKAY) { return NULL; }

  // Get output number array size
  int bufferSizeOutput = 0;
  size_t written_size;
  err = mp_radix_size (&x3, 10, &bufferSizeOutput);
  if (err != MP_OKAY) { return NULL; }

  // Write to output number array
  char * num3_array = (char *) malloc(bufferSizeOutput * sizeof(char));
  err = mp_to_radix(&x3, num3_array, bufferSizeOutput, &written_size, 10);
  if (err != MP_OKAY) { return NULL; }

  // clear mp_int
  mp_clear_multi(&x1, &x2, &x3, &x4, NULL);
  return num3_array;
}

int main() {
  mp_int big1;
  if (mp_init(&big1) != MP_OKAY) {
    printf("ERRRRR!!!\n");
  }
  mp_err err = mp_read_radix(&big1, "128947918649574985723985238579324254643643", 10);
  if (err != MP_OKAY) {
    printf("ERRRRR!!!\n");
  }
  int bufferSize = 0;
  err = mp_radix_size (&big1, 10, &bufferSize);
  if (err != MP_OKAY) {
    printf("ERRRRR!!!\n");
  }
  char buffer[bufferSize];
  size_t written_size;
  err = mp_to_radix (&big1, buffer, bufferSize, &written_size, 10);
  if (err != MP_OKAY) {
    printf("ERRRRR!!!\n");
  }
  printf("test: %s\n", buffer);
  mp_clear(&big1);


  char * res = big_num_op("123456789987654321", 18, "987654321123456789", 18, '+');
  printf("test2: %s\nstrlen: %ld\n", res, strlen(res));
  free(res);

  return 0;
}