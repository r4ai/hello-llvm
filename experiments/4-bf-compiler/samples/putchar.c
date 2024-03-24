#include <stdio.h>

int main() {
  char character = 'a';
  char *ptr = &character;
  putchar(*ptr);
}
