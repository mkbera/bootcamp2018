#include <stdio.h>

void func1(int iter) {
  int a = 0;
  int b = 10;
  if (iter < 5) {
    return;
  }
  printf("func1 called: %d\n", iter);
}

int main() {
  int a = 1;
  for (; a <= 10; a++) {
    func1(a);
  }
}
