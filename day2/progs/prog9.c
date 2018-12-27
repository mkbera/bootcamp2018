#include <stdio.h>

void func1(int iter) {
  int a = 0;
  int b = 10;
  int c = a + b;
  printf("func1 called: %d\n", iter);
}

int main() {
  int a = 1;
  for (; a <= 10; a++) {
    func1(a);
  }
}
