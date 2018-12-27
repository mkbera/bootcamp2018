#include <stdio.h>
void __func_entry(char *fname) {
  printf("Function entry: %s\n", fname);
}

void __func_exit(char *fname) {
  printf("Function exit: %s\n", fname);
}
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
