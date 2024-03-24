#include <stdio.h>
#include <stdlib.h>

int main() {
  char *data = (char *)calloc(30000, sizeof(char));
  char *ptr = data;

  *ptr = 65;

  putchar(*ptr);

  free(data);
  return 0;
}
