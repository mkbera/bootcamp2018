#include <stdio.h>

int main() {
  for (int i = 0; i < 10; i++) {
    printf("Value of i: %d\n", i);
  }

  for (int i = 0; i < 10; i++) {
    for (int j = 0; j < i; j++) {
      printf("Value of i:%d j:%d\n", i, j);
    }
  }

  return 0;
}
