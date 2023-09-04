#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
  char *m = (char *)malloc(5 * sizeof(char));
  strcpy(m, "hello");
  int n;
  printf("%s - %p\n", m, m);
  scanf("%d", &n);
  printf("%s - %p\n", m, m);

  return 0;
}
