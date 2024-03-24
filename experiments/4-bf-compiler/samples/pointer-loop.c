#include <stdlib.h>
int main() {
  char *data = (char *)calloc(30000, sizeof(char));
  char *ptr = data;

  ++*ptr;
  ++*ptr;

  while (*ptr) {
    --*ptr;
  }
}
